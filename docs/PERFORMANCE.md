# Performance Optimization Guide

## Overview

This guide covers performance tuning for EdgeVision on Rockchip RK3588 platforms.

## Hardware Requirements

| Component | Minimum | Recommended |
|-----------|---------|-------------|
| CPU | RK3588 4xA78 | RK3588 4xA78 (full) |
| NPU | 6 TOPS | 6 TOPS (active) |
| RAM | 4GB | 8GB |
| Storage | 32GB eMMC | 64GB eMMC |
| Network | Gigabit | Gigabit |

## Optimization Strategies

### 1. NPU Utilization

- Use INT8 quantized models for maximum NPU throughput
- Enable batch inference when processing multiple channels
- Monitor NPU temperature (keep below 80C)
- Use dynamic shape support for varying resolutions

### 2. Memory Management

- Configure frame queue size based on available RAM
- Use memory pools for frequently allocated objects
- Enable swap on devices with less than 4GB RAM
- Monitor memory usage via /sys/class/thermal

### 3. Multi-Stream Concurrency

- Limit concurrent streams to 8 for stable 15fps per channel
- Use hardware decoding (V4L2 m2m) instead of software decoding
- Distribute streams across CPU cores using affinity settings
- Implement adaptive frame dropping during high load

### 4. Network Optimization

- Use UDP instead of TCP for RTSP when packet loss is acceptable
- Configure buffer sizes for optimal throughput
- Use ONVIF for auto-discovery to reduce manual configuration
- Implement connection pooling for MQTT/HTTP callbacks

## Benchmarks

| Metric | Single Stream | 4 Streams | 8 Streams |
|--------|--------------|-----------|-----------|
| FPS | 25+ | 20+ | 15+ |
| Latency | < 100ms | < 150ms | < 200ms |
| CPU Usage | 35% | 65% | 85% |
| NPU Usage | 60% | 80% | 95% |
| Memory | 800MB | 1.2GB | 1.8GB |
| Power | 5W | 7W | 9W |

## Troubleshooting

### High NPU Temperature
- Reduce concurrent stream count
- Enable thermal throttling mitigation
- Improve device cooling

### Low FPS
- Verify hardware decoding is active
- Check NPU utilization
- Reduce resolution if possible

### Memory Leaks
- Monitor RSS over extended periods
- Enable core dumps for crash analysis
- Review frame queue configuration
