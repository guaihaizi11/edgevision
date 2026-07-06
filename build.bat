@echo off
REM EdgeVision Windows Build Script

set BUILD_TYPE=%1
if "%BUILD_TYPE%"=="" set BUILD_TYPE=Release

echo ============================================
echo   EdgeVision Build Script
echo   Build Type: %BUILD_TYPE%
echo ============================================

REM Create build directory
if not exist build_%BUILD_TYPE% mkdir build_%BUILD_TYPE%
cd build_%BUILD_TYPE%

REM CMake configuration
cmake .. -DCMAKE_BUILD_TYPE=%BUILD_TYPE%

REM Compile
cmake --build . --config %BUILD_TYPE%

echo.
echo Build complete!
echo Executable: build_%BUILD_TYPE%\bin\%BUILD_TYPE%\edgevision.exe
echo.
echo To run:
echo   cd build_%BUILD_TYPE%
echo   bin\%BUILD_TYPE%\edgevision.exe