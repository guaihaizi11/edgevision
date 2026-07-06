@echo off
REM Start EdgeVision API server
cd /d "%~dp0api"
echo Starting EdgeVision API on port 8080...
python main.py