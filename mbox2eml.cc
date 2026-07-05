//////////////////////////////////////////////////////
///// main entry point for mbox2eml.cc
/////////////////////////////////////////////////////
// Copyright (c) Bishoy H.
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
// Description:
// mbox files dump thousands of emails into one massive text block. This makes them portable but useless in modern email clients.
// mbox2eml fixes this. It slices the mbox file at every new email boundary and writes out individual `.eml` files to a target directory.
// It uses every core on your CPU to chew through gigabytes quickly, provided you have the RAM to hold the input file.
//
// Compile with: g++ -O3 -std=c++23 -pthread -o mbox2eml mbox2eml.cc
// (or just run `make`). std::filesystem is part of libstdc++ since GCC 9, so
// the old -lstdc++fs link flag is no longer needed.


#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <thread>
#include <mutex>
#include <atomic>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <filesystem>

namespace fs = std::filesystem;

// Holds the raw string content of a single email, ready to dump to a file.
struct Email {
  std::string content;
};

// Returns true if a string contains nothing but numeric digits.
// Used to validate dates and years in mbox headers.
bool isAllDigits(const std::string& value) {
  return !value.empty() &&
         std::all_of(value.begin(), value.end(),
                     [](unsigned char ch) { return std::isdigit(ch) != 0; });
}

// Scans a line to see if it looks like the start of a new email in the mbox format.
// The classic mbox boundary line looks like this:
// "From sender Sat Jan  1 00:00:00 2022"
bool isLikelyMboxSeparator(const std::string& line) {
  if (!line.starts_with("From ")) {
    return false;
  }

  // Break the line into space-separated chunks.
  std::istringstream iss(line);
  std::string from;
  std::string sender;
  std::string day_of_week;
  std::string month;
  std::string day_of_month;
  std::string time_of_day;
  std::string year;

  if (!(iss >> from >> sender >> day_of_week >> month >> day_of_month >> time_of_day >> year)) {
    return false;
  }

  if (from != "From") {
    return false;
  }

  // The day must be a number, 1 or 2 digits long.
  if (!isAllDigits(day_of_month) || day_of_month.size() > 2) {
    return false;
  }

  // The time must look like HH:MM:SS, which means exactly two colons.
  if (std::count(time_of_day.begin(), time_of_day.end(), ':') != 2) {
    return false;
  }

  // Some systems inject a timezone offset right before the year.
  // e.g., "From sender Wed Mar 25 09:23:47 +0000 2026"
  // If we spot a + or - followed by numbers, skip it and grab the real year next.
  if (!year.empty() && (year[0] == '+' || year[0] == '-') &&
      year.size() >= 2 && isAllDigits(year.substr(1))) {
    std::string actual_year;
    if (!(iss >> actual_year)) {
      return false;
    }
    year = actual_year;
  }

  // The year must be exactly a 4-digit number.
  if (!isAllDigits(year) || year.size() != 4) {
    return false;
  }

  return true;
}

// Reads the entire mbox file into memory and slices it into individual Email structs.
// Throws std::runtime_error if the file won't open or reading fails.
std::vector<Email> extractEmails(const std::string& mbox_file) {
  std::vector<Email> emails;
  std::ifstream file(mbox_file);
  if (!file.is_open()) {
    throw std::runtime_error("failed to open mbox file: " + mbox_file);
  }

  std::string line;
  Email current_email;
  bool in_message = false;

  while (std::getline(file, line)) {
    // Strip Windows-style carriage returns if they exist.
    if (!line.empty() && line.back() == '\r') {
      line.pop_back();
    }
    
    // When we hit a new "From " boundary, save the current email and start a new one.
    if (isLikelyMboxSeparator(line)) {
      if (in_message && !current_email.content.empty()) {
        emails.push_back(current_email);
      }
      current_email.content = line + "\n";
      in_message = true;
    } else {
      // Append the line to the active email. 
      // We skip lines before the first valid "From " boundary.
      if (in_message) {
        current_email.content += line + "\n";
      }
    }
  }

  // Don't forget to push the very last email in the file.
  if (in_message && !current_email.content.empty()) {
    emails.push_back(current_email);
  }

  if (file.bad()) {
    throw std::runtime_error("I/O error while reading mbox file: " + mbox_file);
  }

  return emails;
}

// Dumps an Email struct to disk as a numbered .eml file.
// Returns false if the file could not be created or written.
bool saveEmail(const Email& email, const std::string& output_dir, std::size_t email_count) {
  std::string filename = output_dir + "/email_" + std::to_string(email_count) + ".eml";
  std::ofstream outfile(filename, std::ios::binary);
  if (!outfile.is_open()) {
    return false;
  }
  outfile << email.content;
  return outfile.good();
}

// Runs on a background thread. Grabs a slice of the extracted emails array 
// and writes them to disk sequentially. Threads share a mutex just to print safely.
void workerThread(const std::vector<Email>& emails, const std::string& output_dir, 
                  std::size_t start_index, std::size_t end_index, std::mutex& log_mutex,
                  std::atomic<int>& failed_writes) {
  for (std::size_t i = start_index; i < end_index; ++i) {
    if (saveEmail(emails[i], output_dir, i + 1)) {
      std::lock_guard<std::mutex> lock(log_mutex);
      std::cout << "Saved email_" << i + 1 << ".eml" << std::endl;
      continue;
    }

    failed_writes.fetch_add(1, std::memory_order_relaxed);
    std::lock_guard<std::mutex> lock(log_mutex);
    std::cerr << "Error: Failed to write email_" << i + 1 << ".eml" << std::endl;
  }
}

int main(int argc, char* argv[]) {
  // Enforce usage constraints.
  if (argc != 3) {
    std::cerr << "mbox2eml: Extract individual email messages from an mbox file and save them as separate .eml files." << std::endl;
    std::cerr << "Error: Incorrect number of arguments." << std::endl;
    std::cerr << "Usage: " << argv[0] << " <mbox_file> <output_directory>" << std::endl;
    return 1;
  }

  std::string mbox_file = argv[1];
  std::string output_dir = argv[2];

  if (!fs::exists(mbox_file)) {
    std::cerr << "Error: mbox file does not exist: " << mbox_file << std::endl;
    return 1;
  }
  if (!fs::is_regular_file(mbox_file)) {
    std::cerr << "Error: mbox path is not a regular file: " << mbox_file << std::endl;
    return 1;
  }

  // Create the output directory if missing. Bomb out if a file exists with that name.
  try {
    if (fs::exists(output_dir)) {
      if (!fs::is_directory(output_dir)) {
        std::cerr << "Error: output path exists but is not a directory: " << output_dir << std::endl;
        return 1;
      }
    } else {
      fs::create_directories(output_dir);
    }
  } catch (const std::exception& e) {
    std::cerr << "Error creating output directory: " << e.what() << std::endl;
    return 1;
  }

  // Load everything into memory.
  std::vector<Email> emails;
  try {
    emails = extractEmails(mbox_file);
  } catch (const std::exception& e) {
    std::cerr << "Error reading mbox file: " << e.what() << std::endl;
    return 1;
  }

  std::cout << "Extracted " << emails.size() << " emails." << std::endl;

  if (emails.empty()) {
    std::cout << "Finished processing all emails." << std::endl;
    return 0;
  }

  // Max out CPU cores for the disk write phase.
  std::size_t num_threads = std::thread::hardware_concurrency();
  if (num_threads == 0) {
    num_threads = 2; // Fallback if hardware query fails
  }
  num_threads = std::min<std::size_t>(num_threads, emails.size());

  std::size_t emails_per_thread = emails.size() / num_threads;
  std::size_t remaining_emails = emails.size() % num_threads;

  std::vector<std::thread> threads;
  std::mutex log_mutex;
  std::atomic<int> failed_writes{0};
  std::size_t start_index = 0;

  // Distribute chunks of emails to background threads.
  for (std::size_t i = 0; i < num_threads; ++i) {
    std::size_t end_index = start_index + emails_per_thread;
    if (i < remaining_emails) {
      end_index++;
    }
    threads.emplace_back(workerThread, std::ref(emails), output_dir, start_index, end_index, 
                         std::ref(log_mutex), std::ref(failed_writes));
    start_index = end_index;
  }

  // Block until all files are dumped to disk.
  for (auto& thread : threads) {
    thread.join();
  }

  if (failed_writes.load(std::memory_order_relaxed) > 0) {
    std::cerr << "Finished with " << failed_writes.load(std::memory_order_relaxed)
              << " failed email write(s)." << std::endl;
    return 1;
  }

  std::cout << "Finished processing all emails." << std::endl;
  return 0;
}
