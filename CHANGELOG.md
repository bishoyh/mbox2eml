# Changelog

All notable changes to `mbox2eml` are documented here. The format follows
[Keep a Changelog](https://keepachangelog.com/), and the project uses
[semantic versioning](https://semver.org/) driven by git tags (`vX.Y.Z`).

## [Unreleased]

### Changed
- Build now targets **C++23** consistently. The Makefile used `-std=c++20`
  while the README and source header advertised C++23; all three now agree.
- Makefile hardened: `CXX`/`CXXFLAGS`/`PREFIX` are overridable, targets are
  marked `.PHONY`, and a proper `install` target replaces the ad-hoc `cp` in
  `install.sh`.
- CI workflow (`build.yml`) modernized: GitHub Actions pinned to full commit
  SHAs, an explicit test step (`make test`) added on the Unix runners, and
  releases now trigger on pushed `v*` tags (using the tag name as the version)
  instead of cutting a release on every branch push.
- Source header comment corrected: dropped the obsolete `-lstdc++fs` link flag
  (unnecessary since GCC 9).

### Added
- Jekyll + Just-the-Docs documentation site under `docs/` (Home, Usage,
  How it works).
- Project icon at `docs/assets/icon.png`.
- `CHANGELOG.md` and `TODO.md`.

### Fixed
- `.gitignore` now covers `.DS_Store`, `build/`, and object files, so the macOS
  Finder metadata that kept getting committed stays out of the tree.

## [1.41.0] and earlier

Pre-modernization releases. Highlights reconstructed from git history:

- **1.41.0** — Extract emails from Google Takeout mbox files; parsing hardening.
- Hardened mbox boundary parsing and added the `tests/regression.sh` suite
  (preamble handling, false-positive `From ` body lines, missing-input and
  write-failure error paths, multi-message round trip).
- Upgraded CI artifact actions to v4.
- Multi-threaded `.eml` writing across all CPU cores.
