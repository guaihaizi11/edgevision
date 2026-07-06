# EdgeVision - Edge AI Video Behavior Analysis System

> **RK3588 Edge Video Behavior Analysis** - Real-time detection of phone usage, hardhat absence, and smoking in construction sites and factories.

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)
[![Platform](https://img.shields.io/badge/Platform-RK3588-blue)](https://www.rock-chips.com/)
[![Language](https://img.shields.io/badge/Language-C%2B%2B17-lightgrey)](https://isocpp.org/)
[![Python](https://img.shields.io/badge/Python-3.10+-blue)](https://www.python.org/)
[![Vue](https://img.shields.io/badge/Frontend-Vue.js-42b883)](https://vuejs.org/)

## Overview

EdgeVision is an **open-source edge-deployed video behavior analysis system** purpose-built for the **Rockchip RK3588** embedded AI platform. It provides real-time detection of safety violations in construction sites, factories, warehouses, and other industrial environments - all processed locally with zero cloud dependency.

### Key Features

| Feature | Description |
|---------|-------------|
| Video Ingestion | RTSP/ONVIF camera stream pulling with hardware decoding (GStreamer + V4L2 m2m) |
| Object Detection | YOLOv8/v11 with RKNN/NPU INT8 acceleration |
| Multi-Object Tracking | ByteTrack algorithm for stable ID assignment |
| Behavior Analysis | Phone usage, hardhat absence, smoking detection with temporal logic |
| Alert Management | SQLite local storage + MQTT/HTTP reporting |
| Management API | RESTful API + WebSocket real-time push |
| System Monitoring | Real-time CPU/NPU/memory/temperature monitoring |

### Detected Behaviors

| Behavior | Detection Logic | Alert Level |
|----------|----------------|-------------|
| Phone Usage | Person + CellPhone spatial overlap >= 3 seconds | Warning |
| Hardhat Absence | Person detected but HardHat missing >= 2 seconds | Critical |
| Smoking | Person + Cigarette spatial overlap | Critical |

## Architecture

RTSP Cameras (1-16 channels) --> [Video Ingestion] --> [Frame Queue]
[GStreamer + V4L2 m2m]
[Video Ingestion] --> [Preprocessing] --> [Inference Engine]
[RGA 2D GPU Acceleration] --> [RKNN Runtime + YOLOv8/v11 INT8]
[Inference Engine] --> [Multi-Obj Tracking] --> [Behavior Rule Engine]
[ByteTrack] --> [YAML DSL: Phone/Hardhat/Smoking]
[Behavior Rule Engine] --> [Alerts] [Snapshots] [System Monitor]
[SQLite] [JPEG] [CPU/NPU/Memory/Temperature]
[System Monitor] --> [Management API] --> [Frontend UI]
[FastAPI + WebSocket] --> [Vue.js + Element Plus]

## Quick Start

### Prerequisites

- **Hardware**: Rockchip RK3588 (4xA78 + 4xA55 + 6TOPS NPU), >= 4GB RAM
- **OS**: Debian 12 / Ubuntu 22.04 (Rockchip BSP)
- **Cross-compile host**: x86_64 Linux/Windows

### Dependencies

| Component | Version | Purpose |
|-----------|---------|---------|
| CMake | >= 3.18 | Build system |
| GCC | >= 9.4 | C++ compiler |
| RKNN-Toolkit2 | >= 2.2.0 | NPU inference SDK |
| GStreamer | >= 1.22 | Video processing |
| OpenCV | >= 4.7 | Image processing |
| yaml-cpp | >= 0.7 | YAML config parsing |
| SQLite3 | >= 3.39 | Data storage |
| FastAPI | >= 0.100 | Python API framework |

### Build & Run

    cd edgevision
    mkdir build && cd build
    cmake .. -DCMAKE_BUILD_TYPE=Release
    make -j$(nproc)

    cd ../api
    pip install -r requirements.txt
    python main.py

### Cross-Compile for RK3588

    mkdir build_rk3588 && cd build_rk3588
    cmake .. -DCMAKE_TOOLCHAIN_FILE=../toolchain-rk3588.cmake
    make -j4


## Project Structure

    edgevision/
    +- src/                    # C++ core modules
    |   +- core/              # EventBus, FrameQueue
    |   +- video/             # RTSP source (GStreamer/V4L2)
    |   +- preprocess/        # Image preprocessing (RGA)
    |   +- inference/         # RKNN/ONNX inference engine
    |   +- tracking/          # ByteTrack multi-object tracking
    |   +- behavior/          # Behavior rule engine (YAML DSL)
    |   +- alert/             # Alert management
    |   +- snapshot/          # Snapshot management
    |   +- config/            # Configuration management
    |   +- storage/           # SQLite storage
    |   +- monitor/           # System monitoring
    |   +- pipeline/          # Analysis pipeline orchestration
    +- api/                   # Python FastAPI server
    |   +- main.py            # REST API + WebSocket
    |   +- routes/            # API route handlers
    |   +- models/            # Pydantic models
    +- web/                   # Vue.js frontend
    |   +- src/               # Components, views, stores
    |   +- public/            # Static assets
    +- config/                # YAML configuration files
    |   +- channels.yaml      # Camera channel config
    |   +- behaviors.yaml     # Behavior rules config
    |   +- system.yaml        # System settings
    +- tests/                 # Unit tests
    +- scripts/               # Build & deploy scripts
    +- CMakeLists.txt         # CMake build config
    +- README.md              # This file

## API Endpoints

| Method | Endpoint | Description |
|--------|----------|-------------|
| GET | /health | Health check |
| GET | /api/channels | List all channels |
| POST | /api/channels | Create channel |
| PUT | /api/channels/{id} | Update channel |
| DELETE | /api/channels/{id} | Delete channel |
| GET | /api/channels/{id}/snapshot | Get live snapshot |
| WS | /ws/alerts | Real-time alert stream |
| GET | /api/alerts | Query alerts |
| PUT | /api/alerts/{id}/status | Update alert status |
| GET | /api/stats | System statistics |
| GET | /api/models | List detection models |
| POST | /api/models | Upload model |
| DELETE | /api/models/{id} | Delete model |
| GET | /api/behaviors | List behavior rules |
| PUT | /api/behaviors/{id} | Update behavior rule |
| DELETE | /api/behaviors/{id} | Delete behavior rule |

## Performance Targets

| Metric | Target |
|--------|--------|
| End-to-end Latency | <= 200ms |
| Single Channel FPS | >= 15fps (1080p) |
| Memory Usage | <= 2GB |
| Power Consumption | <= 10W |
| NPU Temperature | <= 80C |
| Multi-stream Concurrency | 8-16 channels |

## Technology Stack

| Layer | Technology |
|-------|-----------|
| Core Language | C++17 |
| Video Decoding | GStreamer + V4L2 m2m (RK3588 VPU) |
| Inference Framework | RKNN Runtime (NPU) |
| Detection Model | YOLOv8n/v11n (INT8 quantized) |
| Tracking Algorithm | ByteTrack |
| Data Storage | SQLite3 |
| Alert Reporting | MQTT / HTTP |
| Management API | FastAPI + Uvicorn |
| Frontend | Vue.js 3 + Element Plus |
| Build System | CMake 3.18+ |

## Development Roadmap

| Phase | Status | Description |
|-------|--------|-------------|
| Core Pipeline | Done | Video ingest -> preprocess -> detect -> track -> analyze |
| REST API | Done | Full CRUD for channels, alerts, models, behaviors |
| Frontend UI | Done | Vue.js dashboard with 8 pages |
| RKNN Integration | In Progress | Replace simulation with real NPU inference |
| GStreamer HW Decode | In Progress | V4L2 m2m hardware decoding pipeline |
| RGA Acceleration | Planned | 2D GPU image preprocessing |
| WebSocket Push | Planned | Real-time alert notifications |
| Model Training | Planned | Scene-specific YOLO models with INT8 quantization |

## License

This project is licensed under the MIT License.

## Contributing

Contributions are welcome! Please read CONTRIBUTING.md for guidelines.

## Documentation

- Product Requirements (PRD) - EdgeVision_PRD.md
- Functional Design - EdgeVision_FD.md
- Development Guide - docs/DEVELOPMENT.md
- System Requirements - 需求.md

## Reference

Based on [beixiaocai/rebekah](https://github.com/beixiaocai/rebekah) v5 architecture design.
