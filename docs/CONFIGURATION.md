# Configuration Guide

## Overview

EdgeVision uses YAML configuration files for all system settings. All config files support UTF-8 encoding including Chinese and other Unicode characters.

## Configuration Files

### channels.yaml

Defines camera channels and their RTSP/ONVIF connections.

    channels:
      - id: CAM-001
        name: 工地入口
        rtsp_url: rtsp://admin:pass@192.168.1.100:554/stream1
        fps: 5
        enabled: true
        resolution: 1920x1080

### behaviors.yaml

Defines behavior detection rules with YAML DSL.

    behaviors:
      - name: phone_usage
        description: Detect phone usage by workers
        conditions:
          - person_overlap: true
            min_duration: 3.0
        alert_level: warning
        enabled: true
        priority: 5

      - name: hardhat_absence
        description: Detect missing hardhat
        conditions:
          - person_detected: true
            hardhat_missing: true
            min_duration: 2.0
        alert_level: critical
        enabled: true
        priority: 1

### system.yaml

System-level configuration.

    system:
      logging:
        level: INFO
        file: /var/log/edgevision.log
      monitoring:
        interval_seconds: 5
        report_cpu: true
        report_memory: true
        report_npu: true
        report_temperature: true
      mqtt:
        enabled: false
        broker: localhost
        port: 1883
        topic_prefix: edgevision/alerts
      http_callback:
        enabled: false
        url: http://backend.example.com/api/alerts
        timeout_seconds: 5
