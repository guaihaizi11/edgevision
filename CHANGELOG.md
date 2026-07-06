# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Added
- Config encoding support module (api/config_encoding.py)

### Changed
- Improved README with full project documentation

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
