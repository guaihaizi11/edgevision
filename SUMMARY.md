# EdgeVision Project Summary

## Project Overview

- **Project Name**: EdgeVision - RK3588 Video Behavior Analysis System
- **Reference Project**: [beixiaocai/rebekah](https://github.com/beixiaocai/rebekah)
- **Version**: v1.0.0
- **Code Scale**: 42 source files, ~118KB

## Completed Modules

### C++ Core Modules (11 modules)

| Module | File | Status | Description |
|------|------|------|------|
| Type Definitions | types.h | Done | Basic data types, detection/tracking/alert structures |
| Event Bus | event_bus.h/cpp | Done | Publish-subscribe event dispatching |
| Frame Queue | frame_queue.h/cpp | Done | Thread-safe frame buffer |
| RTSP Source | rtsp_source.h/cpp | Done | Video stream ingestion (OpenCV implementation) |
| Preprocessor | preprocessor.h/cpp | Done | Image scaling, color conversion, normalization |
| Inference Engine | inference_engine.h/cpp | Done | Model loading, inference, post-processing |
| Object Tracking | bytetracker.h/cpp | Done | ByteTrack multi-object tracking |
| Behavior Engine | behavior_engine.h/cpp | Done | YAML rule parsing, temporal judgment |
| Alert Manager | alert_manager.h/cpp | Done | SQLite storage, MQTT/HTTP reporting |
| Snapshot Manager | snapshot_manager.h/cpp | Done | Annotation drawing, image saving |
| Config Manager | config_manager.h/cpp | Done | YAML/JSON configuration read/write |
| System Monitor | system_monitor.h/cpp | Done | CPU/memory/NPU/temperature monitoring |
| Analysis Pipeline | analyzer_pipeline.h/cpp | Done | Core processing pipeline |
| Main Program | main.cpp | Done | Program entry point |

### Python API Modules (2 modules)

| Module | File | Status | Description |
|------|------|------|------|
| REST API | main.py | Done | FastAPI management interface (verified running) |
| Dependencies | requirements.txt | Done | Python dependency list |

### Configuration Files (3 files)

| File | Status | Description |
|------|------|------|
| channels.yaml | Done | Camera channel configuration |
| behaviors.yaml | Done | Behavior rule configuration |
| system.yaml | Done | System configuration |

### Build Scripts (4 files)

| File | Status | Description |
|------|------|------|
| CMakeLists.txt | Done | CMake build configuration |
| build.sh | Done | Linux build script |
| build.bat | Done | Windows build script |
| setup.sh | Done | Installation and deployment script |

### Documentation (4 files)

| File | Status | Description |
|------|------|------|
| README.md | Done | Project documentation |
| DEVELOPMENT.md | Done | Development guide |
| SUMMARY.md | Done | This file |
| .gitignore | Done | Git ignore configuration |

## Feature Coverage

### Implemented Core Features

1. **Video Ingestion** Done
   - RTSP stream pulling (OpenCV VideoCapture)
   - Frame queue management
   - Disconnection reconnection mechanism

2. **Image Preprocessing** Done
   - Hardware-accelerated scaling (RGA interface reserved)
   - Color space conversion
   - Normalization processing
   - ROI cropping

3. **Object Detection** Done
   - Model loading/unloading
   - Inference execution (ONNX Runtime interface reserved)
   - Asynchronous inference support
   - Performance statistics

4. **Multi-Object Tracking** Done
   - ByteTrack algorithm
   - Track ID management
   - Lost target handling

5. **Behavior Analysis** Done
   - YAML rule configuration
   - Spatial overlap detection
   - Negative detection
   - Temporal consistency judgment
   - Cooldown period control

6. **Alert Management** Done
   - SQLite persistent storage
   - MQTT reporting (interface reserved)
   - HTTP callback (interface reserved)
   - Snapshot saving

7. **System Monitoring** Done
   - CPU usage
   - Memory usage
   - NPU status (interface reserved)
   - Temperature monitoring

8. **Management API** Done
   - RESTful API (FastAPI)
   - Health check
   - Channel management
   - Alert query
   - System statistics

### Features to Adapt for RK3588

| Feature | Current Implementation | RK3588 Adaptation |
|------|----------|----------|
| Video Decoding | OpenCV VideoCapture | GStreamer + V4L2 m2m |
| Inference Engine | ONNX Runtime simulation | RKNN Runtime |
| Image Preprocessing | OpenCV | RGA hardware acceleration |
| NPU Monitoring | Simulated data | /sys/class/thermal |
| MQTT | Interface reserved | paho-mqtt |

## Quick Verification

### 1. Start API Service

`powershell
cd edgevision/api
python main.py
# Visit http://localhost:8080/health
`

### 2. Compile C++ Core (requires OpenCV and SQLite3)

`powershell
cd edgevision
.\build.bat Release
`

### 3. Run Tests

`ash
cd build_Release
./bin/test_pipeline
`

## Deploy to RK3588

### Cross Compilation

`ash
mkdir build_rk3588 && cd build_rk3588
cmake .. -DCMAKE_TOOLCHAIN_FILE=../toolchain-rk3588.cmake
make -j4
`

### Deployment Steps

1. Install RK3588 BSP and dependencies
2. Copy executables and configuration files
3. Configure systemd service
4. Start the service

## Next Steps

1. **Integrate Real YOLO Model**
   - Train/convert detection model
   - Integrate RKNN Runtime

2. **Complete GStreamer Video Ingestion**
   - Replace OpenCV with GStreamer
   - Enable VPU hardware decoding

3. **Add WebSocket Real-Time Push**
   - Frontend real-time alert notifications
   - System status monitoring

4. **Complete Frontend UI**
   - Vue.js admin dashboard
   - Real-time video preview
   - Alert visualization

5. **Performance Optimization**
   - Multi-threaded pipeline
   - Memory pool management
   - NPU utilization optimization

---

*Project Created: 2026-07-02*
*Based on rebekah v5 architecture design*

