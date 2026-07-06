#!/bin/bash

echo "========================================"
echo " EdgeVision - One-Click Startup Script"
echo "========================================"
echo

# Kill existing processes on ports 8080 and 3000
echo "[1/3] Cleaning up old processes..."
fuser -k 8080/tcp 2>/dev/null
fuser -k 3000/tcp 2>/dev/null
sleep 1

echo "[2/3] Starting backend API (port 8080)..."
cd "$(dirname "$0")/api"
python3 main.py &
API_PID=$!
sleep 2

echo "[3/3] Starting frontend page (port 3000)..."
cd "$(dirname "$0")/web"
python3 -m http.server 3000 &
WEB_PID=$!
sleep 1

# Open browser
if [[ "$OSTYPE" == "darwin"* ]]; then
    open "http://localhost:3000"
elif [[ "$OSTYPE" == "linux-gnu"* ]]; then
    xdg-open "http://localhost:3000" 2>/dev/null || echo "Please visit http://localhost:3000 manually"
fi

echo ""
echo "========================================"
echo " Startup Complete!"
echo "  Frontend: http://localhost:3000"
echo "  Backend:  http://localhost:8080"
echo "  Backend PID: $API_PID"
echo "  Frontend PID: $WEB_PID"
echo "========================================"