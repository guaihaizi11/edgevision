#!/bin/bash
# EdgeVision Installation Script
# For installation on RK3588 or x86 Linux

set -e

echo \"============================================\"
echo \"  EdgeVision Setup Script\"
echo \"============================================\"

# Detect system
if [ -f /etc/os-release ]; then
    . /etc/os-release
    OS=\"\"
    echo \"Detected OS: \"
fi

# Install system dependencies
install_system_deps() {
    echo \"Installing system dependencies...\"

    if command -v apt-get &> /dev/null; then
        sudo apt-get update
        sudo apt-get install -y \
            build-essential \
            cmake \
            git \
            pkg-config \
            libopencv-dev \
            libsqlite3-dev \
            libcurl4-openssl-dev \
            libgstreamer1.0-dev \
            libgstreamer-plugins-base1.0-dev \
            gstreamer1.0-plugins-bad \
            gstreamer1.0-libav \
            gstreamer1.0-gl \
            libgstrtspserver-1.0-dev \
            libyaml-cpp-dev \
            libboost-all-dev \
            python3-pip \
            python3-venv
    elif command -v yum &> /dev/null; then
        sudo yum install -y \
            gcc-c++ \
            cmake \
            opencv-devel \
            sqlite-devel \
            gstreamer1-devel \
            yaml-cpp-devel
    fi
}

# Install Python dependencies
install_python_deps() {
    echo \"Installing Python dependencies...\"
    cd api
    python3 -m venv venv
    source venv/bin/activate
    pip install -r requirements.txt
}

# Build C++ core
build_cpp() {
    echo \"Building C++ core...\"
    mkdir -p build && cd build
    cmake .. -DCMAKE_BUILD_TYPE=Release
    make -j
    cd ..
}

# Install to system
install_system() {
    echo \"Installing to system...\"
    sudo cp build/bin/edgevision /usr/local/bin/
    sudo cp -r config /etc/edgevision/
    sudo cp scripts/edgevision.service /etc/systemd/system/
    sudo systemctl daemon-reload
    sudo systemctl enable edgevision
}

# Main flow
case \"\" in
    deps)
        install_system_deps
        ;;
    python)
        install_python_deps
        ;;
    build)
        build_cpp
        ;;
    install)
        install_system
        ;;
    all)
        install_system_deps
        build_cpp
        install_python_deps
        ;;
    *)
        echo \"Usage:  {deps|python|build|install|all}\"
        exit 1
        ;;
esac

echo \"\"
echo \"Setup complete!\"
echo \"To start the service:\"
echo \"  sudo systemctl start edgevision\"
echo \"  # or manually:\"
echo \"  edgevision --config /etc/edgevision\"