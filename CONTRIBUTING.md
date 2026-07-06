# Contributing to EdgeVision

Thank you for your interest in contributing to EdgeVision!

## Getting Started

1. Fork the repository
2. Clone your fork and create a feature branch
3. Make your changes and test thoroughly

## Development Setup

### Prerequisites
- CMake 3.18+
- GCC 9.4+ (or cross-compiler for RK3588)
- Python 3.10+
- OpenCV 4.7+
- SQLite3

### Building
    mkdir build && cd build
    cmake .. -DCMAKE_BUILD_TYPE=Release
    make

### Running Tests
    cd build
    ./bin/test_pipeline

## Code Style

### C++
- Follow Google C++ Style Guide
- Use C++17 features
- Document public APIs

### Python
- Follow PEP 8
- Use type hints
- Docstrings for all public functions

### Vue.js
- Use Composition API
- Follow Vue.js style guide

## Pull Request Process

1. Update documentation as needed
2. Add or update tests
3. Ensure all tests pass
4. Update CHANGELOG.md
5. Submit PR with clear description

## Labels

| Label | Description |
|-------|-------------|
| bug | Something is not working |
| enhancement | New feature or improvement |
| documentation | Improvements to documentation |
| good first issue | Good for newcomers |
| help wanted | Extra attention needed |
| rknn | RKNN/NPU related |
| frontend | Vue.js frontend changes |
| api | Python API changes |

## Questions?

Feel free to open an issue or start a discussion!
