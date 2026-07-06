# RK3588 Cross-Compilation Toolchain
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR arm)

# Cross-compiler path (adjust to your toolchain)
set(CROSS_COMPILE /opt/gcc-arm-8.3-2019.03-x86_64-aarch64-none-linux-gnu)
set(CMAKE_C_COMPILER /bin/aarch64-none-linux-gnu-gcc)
set(CMAKE_CXX_COMPILER /bin/aarch64-none-linux-gnu-g++)

# Sysroot
set(CMAKE_SYSROOT /aarch64-none-linux-gnu)

# Find programs
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

# RK3588 specific flags
set(CMAKE_C_FLAGS \"-march=armv8-a+crc -mtune=cortex-a73.cortex-a53\" CACHE STRING \"\" FORCE)
set(CMAKE_CXX_FLAGS \"-march=armv8-a+crc -mtune=cortex-a73.cortex-a53\" CACHE STRING \"\" FORCE)