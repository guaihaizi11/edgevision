#!/usr/bin/env python3
"""EdgeVision Web Server with API Proxy"""
import http.server
import socketserver
class ThreadedHTTPServer(socketserver.ThreadingMixIn, http.server.HTTPServer):
    daemon_threads = True
import os
import urllib.request

PORT = 3000
WEB_DIR = os.path.dirname(os.path.abspath(__file__))
BACKEND = "http://localhost:8080"

class Handler(http.server.BaseHTTPRequestHandler):
    def do_GET(self):
        if self.path.startswith("/api"):
            url = BACKEND + self.path
            try:
                req = urllib.request.Request(url)
                with urllib.request.urlopen(req, timeout=10) as resp:
                    data = resp.read()
                    self.send_response(resp.status)
                    for k, v in resp.getheaders():
                        if k.lower() not in ("transfer-encoding", "connection"):
                            self.send_header(k, v)
                    self.end_headers()
                    self.wfile.write(data)
            except Exception as e:
                self.send_error(502, str(e))
        else:
            path = self.path
            if path == "/": path = "/index.html"
            full = os.path.join(WEB_DIR, path.lstrip("/"))
            if os.path.exists(full) and os.path.isfile(full):
                self.send_response(200)
                ct = "application/octet-stream"
                if full.endswith(".js"): ct = "application/javascript"
                elif full.endswith(".css"): ct = "text/css"
                elif full.endswith(".html"): ct = "text/html"
                self.send_header("Content-Type", ct)
                self.end_headers()
                with open(full, "rb") as f: self.wfile.write(f.read())
            else:
                self.send_error(404)

    def do_POST(self):
        self._proxy()
    def do_PUT(self):
        self._proxy()
    def do_DELETE(self):
        self._proxy()
    def _proxy(self):
        cl = int(self.headers.get("Content-Length", 0))
        body = self.rfile.read(cl) if cl else None
        url = BACKEND + self.path
        try:
            req = urllib.request.Request(url, data=body, method=self.command)
            req.add_header("Content-Type", "application/json")
            with urllib.request.urlopen(req, timeout=10) as resp:
                data = resp.read()
                self.send_response(resp.status)
                for k, v in resp.getheaders():
                    if k.lower() not in ("transfer-encoding", "connection"):
                        self.send_header(k, v)
                self.end_headers()
                self.wfile.write(data)
        except Exception as e:
            self.send_error(502, str(e))

    def log_message(self, fmt, *args):
        pass

if __name__ == "__main__":
    with ThreadedHTTPServer(("", PORT), Handler) as httpd:
        print("EdgeVision Web -> http://localhost:" + str(PORT))
        httpd.serve_forever()

