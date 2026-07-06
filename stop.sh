#!/bin/bash

echo "========================================"
echo " EdgeVision - Stop All Services"
echo "========================================"

# Kill processes on ports 8080 and 3000
fuser -k 8080/tcp 2>/dev/null && echo "Stopped API (port 8080)" || echo "  Not running"
fuser -k 3000/tcp 2>/dev/null && echo "Stopped Web (port 3000)" || echo "  Not running"

echo ""
echo "All services stopped."