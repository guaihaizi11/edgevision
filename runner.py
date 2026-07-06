#!/usr/bin/env python3
"""EdgeVision One-Click Launcher"""
import subprocess
import sys
import os
import time
import urllib.request
import signal

BASE = os.path.dirname(os.path.abspath(__file__))
API_DIR = os.path.join(BASE, 'api')
WEB_DIR = os.path.join(BASE, 'web')
PYTHON = sys.executable

def kill_port(port):
    try:
        result = subprocess.run(
            ['netstat', '-ano'],
            capture_output=True, text=True,
            encoding='gbk', errors='ignore'
        )
        for line in result.stdout.splitlines():
            if ':' + str(port) in line and 'LISTENING' in line:
                parts = line.strip().split()
                pid = parts[-1]
                if pid.isdigit() and int(pid) != os.getpid():
                    subprocess.run(['taskkill', '/F', '/PID', pid], capture_output=True)
    except Exception:
        pass

print('=' * 40)
print('  EdgeVision - One-Click Launcher')
print('=' * 40)
print()

print('[1/3] Cleaning up...')
kill_port(8080)
kill_port(3000)
time.sleep(1)

print('[2/3] Starting backend API (port 8080)...')
api_proc = subprocess.Popen(
    [PYTHON, 'main.py'],
    cwd=API_DIR,
    stdout=subprocess.PIPE,
    stderr=subprocess.STDOUT,
)
print(f'  PID: {api_proc.pid}')
time.sleep(3)

print('[3/3] Starting frontend (port 3000)...')
web_proc = subprocess.Popen(
    [PYTHON, '-m', 'http.server', '3000'],
    cwd=WEB_DIR,
    stdout=subprocess.PIPE,
    stderr=subprocess.STDOUT,
)
print(f'  PID: {web_proc.pid}')
time.sleep(1)

print()
print('Verifying...')
ok = True
for name, url in [('Backend API', 'http://localhost:8080/health'), ('Frontend', 'http://localhost:3000/')]:
    try:
        resp = urllib.request.urlopen(url, timeout=5)
        print(f'  {name}: OK')
    except Exception as e:
        print(f'  {name}: FAILED - {e}')
        ok = False

if ok:
    print()
    print('=' * 40)
    print('  Started successfully!')
    print('  Frontend: http://localhost:3000')
    print('  Backend:  http://localhost:8080')
    print('=' * 40)
else:
    print()
    print('WARNING: Some services failed to start.')

print()
print('Press Ctrl+C to stop all services.')

def handler(sig, frame):
    print('\nStopping all services...')
    api_proc.terminate()
    web_proc.terminate()
    api_proc.wait()
    web_proc.wait()
    print('All services stopped.')
    sys.exit(0)

signal.signal(signal.SIGINT, handler)

try:
    while True:
        time.sleep(1)
except KeyboardInterrupt:
    handler(None, None)
