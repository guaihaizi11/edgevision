#!/usr/bin/env python3
import subprocess, os, sys

for line in subprocess.run(['netstat','-ano'], capture_output=True, text=True, encoding='gbk', errors='ignore').stdout.splitlines():
    if ':8080' in line and 'LISTENING' in line:
        pid = line.strip().split()[-1]
        if pid.isdigit() and int(pid) != os.getpid():
            subprocess.run(['taskkill','/F','/PID',pid], capture_output=True)
            print(f"Stopped API (PID {pid})")
    if ':3000' in line and 'LISTENING' in line:
        pid = line.strip().split()[-1]
        if pid.isdigit() and int(pid) != os.getpid():
            subprocess.run(['taskkill','/F','/PID',pid], capture_output=True)
            print(f"Stopped Web (PID {pid})")

print("All services stopped.")
