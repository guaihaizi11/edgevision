#!/bin/bash
# EdgeVision Build Script

set -e

BUILD_TYPE=${1:-Release}
BUILD_DIR="build_${BUILD_TYPE}"

echo "============================================"
echo "  EdgeVision Build Script"
echo "  Build Type: $BUILD_TYPE"
echo "============================================"

# Create build directory
mkdir -p $BUILD_DIR
cd $BUILD_DIR

# CMake configuration
cmake .. -DCMAKE_BUILD_TYPE=$BUILD_TYPE

# Compile
make -j

echo ""
echo "Build complete!"
echo "Executable: /bin/edgevision"
echo ""
echo "To run:"
echo "  cd $BUILD_DIR"
echo "  ./bin/edgevision"