# Security Policy

## Supported Versions

| Version | Supported |
|---------|-----------|
| 1.2.x   | Yes       |
| 1.1.x   | Yes       |
| 1.0.x   | Yes       |
| < 1.0   | No        |

## Reporting a Vulnerability

We take security seriously. If you discover a security vulnerability:

1. **DO NOT** open a public issue
2. Use GitHub Security Advisories or contact maintainers directly
3. Include a detailed description and reproduction steps
4. Allow time for the fix before public disclosure

## Security Considerations

- RTSP credentials should be stored securely in config files
- API endpoints should be behind authentication in production
- Configuration files may contain sensitive data (passwords, URLs)
- Use HTTPS for all remote connections
- Validate all input data in C++ and Python layers
- Regularly update RKNN Toolkit2 and GStreamer dependencies
