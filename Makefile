CXX ?= g++
# Static linking bakes libstdc++ into the binary so Linux releases run anywhere.
# macOS has no usable -static, so we link dynamically there.
ifeq ($(shell uname),Darwin)
CXXFLAGS ?= -O3 -std=c++23 -pthread
else
CXXFLAGS ?= -O3 -std=c++23 -pthread -static
endif
TARGET = mbox2eml
SRC = mbox2eml.cc
PREFIX ?= /usr/local

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC)

test: $(TARGET)
	./tests/regression.sh

install: $(TARGET)
	install -d $(DESTDIR)$(PREFIX)/bin
	install -m 755 $(TARGET) $(DESTDIR)$(PREFIX)/bin/$(TARGET)

clean:
	rm -f $(TARGET)

.PHONY: all test install clean
