# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Added
- Security policy documentation (docs/SECURITY.md)
- UTF-8 encoding support for config files (api/config_encoding.py)

### Changed
- Comprehensive README rewrite with use cases, ecosystem, and architecture
- Improved documentation structure and navigation

## [1.2.0] - 2026-07-09

### Added
- **Use Cases Section**: Construction sites, manufacturing factories, warehouses, smart parks, school campuses
- **Ecosystem & Compatibility Table**: RK3588, RKNN Toolkit2, GStreamer, RGA, MQTT brokers, RTSP/ONVIF cameras
- **Technology Stack Table**: Full stack overview from C++17 core to Vue.js frontend
- **Performance Targets**: Detailed metrics (latency, FPS, memory, power, temperature)
- **GitHub Badges**: License, platform, language, release, commit activity, stars
- **Development Roadmap**: Phase-by-phase status tracking with milestones
- **API Endpoint Table**: Complete 16-endpoint documentation with methods and descriptions
- **Project Structure Diagram**: Full directory tree with module descriptions

### Fixed
- Config file encoding issues with Chinese characters (Issue #2)
- README organization and completeness

### Documentation
- SECURITY.md: Vulnerability reporting and supported versions
- CONTRIBUTING.md: Contribution guidelines and code style
- CODE_OF_CONDUCT.md: Community standards
- CHANGELOG.md: Version history tracking

## [1.1.0] - 2026-07-06

### Added
- UTF-8 encoding support for configuration files (fixes #2)
- New config_encoding.py module with YAML/JSON Unicode handling
- Comprehensive documentation updates
- Added CHANGELOG.md with full version history
- Added CONTRIBUTING.md with contribution guidelines
- Added LICENSE (MIT)
- Added CODE_OF_CONDUCT.md
- Improved README with architecture diagram and API documentation
- Created milestone v1.1.0 for RKNN integration tracking

## [1.0.0] - 2026-07-06

### Added
- **Core Pipeline**: Complete video analysis pipeline (C++17)
  - RTSP video ingestion with frame queue
  - Image preprocessing with scaling and normalization
  - Object detection inference engine (YOLOv8/v11)
  - ByteTrack multi-object tracking
  - Behavior rule engine with YAML DSL
  - Alert management with SQLite storage
  - Snapshot management with annotation
  - System monitoring (CPU, memory, NPU, temperature)
- **API Server**: FastAPI REST API with 11 endpoints
  - Channel CRUD operations
  - Alert query and status management
  - Model management (upload, list, delete)
  - Behavior rule management
  - System statistics
  - Health check endpoint
- **Frontend**: Vue.js 3 dashboard with 8 pages
  - Dashboard overview with statistics
  - Real-time monitor view
  - Camera channel management
  - Algorithm/model management
  - Behavior rule configuration
  - Alert center with filtering
  - System settings
  - User login
- **Build System**: CMake configuration
  - Cross-compilation support for RK3588
  - Build scripts for Windows (.bat) and Linux (.sh)
  - Setup and deployment scripts
- **Configuration**: YAML-based system configuration
  - Channel definitions (RTSP URLs, FPS, credentials)
  - Behavior rules (conditions, thresholds, cooldown)
  - System settings (logging, monitoring intervals)
- **Documentation**: Comprehensive project documentation
  - README with architecture and setup guide
  - Product Requirements Document (PRD)
  - Functional Design Document
  - Development Guide
  - Project Summary

### Technical Details
- 14 C++ core modules (~3,200 lines)
- Python API server (~500 lines)
- Vue.js frontend (~1,800 lines)
- Total: 54 files, ~8,200 lines of code
- MIT License
- Target platform: Rockchip RK3588

## [0.1.0] - 2026-07-02

- Architecture design based on rebekah v5
- Project structure defined
- Requirements and PRD documents created
