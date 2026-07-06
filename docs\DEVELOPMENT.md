# EdgeVision Development Guide

## Quick Start

### 1. Environment Setup

#### Windows (x86)
`powershell
# Install Visual Studio 2019/2022
# Install CMake
choco install cmake -y

# Install OpenCV (use vcpkg)
vcpkg install opencv:x64-windows

# Install SQLite3
vcpkg install sqlite3:x64-windows
`

#### Linux (x86/RK3588)
`ash
# Ubuntu/Debian
sudo apt install build-essential cmake git pkg-config \
    libopencv-dev libsqlite3-dev libcurl4-openssl-dev \
    libgstreamer1.0-dev libyaml-cpp-dev python3-pip

# RK3588 additional dependencies
sudo apt install gstreamer1.0-plugins-bad \
    gstreamer1.0-rockchip librknn-runtime
`

### 2. Build Project

`ash
cd edgevision

# Windows
.\build.bat Release

# Linux
chmod +x build.sh
./build.sh Release
`

### 3. Run

`ash
# Run core program
cd build_Release
./bin/edgevision

# Run API service
cd api
pip install -r requirements.txt
python main.py
`

## Code Structure

`
edgevision/
├── src/                    # C++ source code
│   ├── core/              # Core infrastructure
│   ├── video/             # Video ingestion
│   ├── preprocess/        # Image preprocessing
│   ├── inference/         # Inference engine
│   ├── tracking/          # Object tracking
│   ├── behavior/          # Behavior analysis
│   ├── alert/             # Alert management
│   ├── snapshot/          # Snapshot management
│   ├── config/            # Configuration management
│   ├── storage/           # Data storage
│   ├── monitor/           # System monitoring
│   ├── pipeline/          # Analysis pipeline
│   └── main.cpp           # Main program entry
├── api/                   # Python REST API
├── config/                # Configuration files
├── tests/                 # Test code
├── CMakeLists.txt         # Build configuration
└── README.md
`

## Adding New Features

### Adding a New Detection Category

1. Modify the category list in inference_engine.h
2. Update the rule definitions in behaviors.yaml
3. Retrain the model

### Adding a New Behavior Rule

Edit config/behaviors.yaml:

`yaml
behavior_rules:
  - rule_id: behavior_new_rule
    name: New Rule Name
    conditions:
      - type: spatial_overlap
        objects: [object1, object2]
        iou_threshold: 0.1
        duration_frames: 15
    action:
      alert_level: warning
      mqtt_topic: edgevision/alert/new_rule
`

## Debugging Tips

### Enable Verbose Logging

`ash
export EV_LOG_LEVEL=debug
./edgevision
`

### Use Test Video

`ash
# Create test video
ffmpeg -f lavfi -i testsrc=duration=10:size=1920x1080:rate=25 test.mp4

# Run with test video
./edgevision --demo
`

## Performance Optimization

### x86 Optimization
- Use -O3 compile flags
- Enable SIMD instruction sets
- Multi-threaded parallel processing

### RK3588 Optimization
- Use RKNN INT8 quantization
- Enable RGA hardware acceleration
- Use VPU hardware decode
- Optimize memory allocation strategy

## Common Issues

### Q: Compilation fails, cannot find OpenCV
A: Ensure OpenCV development libraries are installed, and set the correct CMAKE_PREFIX_PATH

### Q: RTSP connection failed
A: Check if the camera IP, port, username, and password are correct

### Q: Memory usage is too high
A: Reduce the frame queue size, increase the sampling interval
