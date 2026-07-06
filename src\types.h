#ifndef EDGESVISION_TYPES_H
#define EDGESVISION_TYPES_H

#include <string>
#include <vector>
#include <memory>
#include <cstdint>
#include <chrono>
#include <functional>
#include <opencv2/opencv.hpp>

namespace edgevision {

// ==================== Base Types ====================

using Frame = cv::Mat;
using FramePtr = std::shared_ptr<Frame>;

enum class AlertLevel {
    INFO,
    WARNING,
    CRITICAL
};

enum class ChannelStatus {
    STOPPED,
    RUNNING,
    RECONNECTING,
    ERROR
};

enum class AlertStatus {
    PENDING,
    CONFIRMED,
    IGNORED
};

// ==================== Detection Target ====================

struct DetectionResult {
    int class_id;
    std::string class_name;
    float confidence;
    float x1, y1, x2, y2;  // Bounding box coordinates

    // Calculate center point
    float cx() const { return (x1 + x2) / 2.0f; }
    float cy() const { return (y1 + y2) / 2.0f; }

    // Calculate width and height
    float width() const { return x2 - x1; }
    float height() const { return y2 - y1; }

    // Calculate IoU
    float IoU(const DetectionResult& other) const {
        float inter_x1 = std::max(x1, other.x1);
        float inter_y1 = std::max(y1, other.y1);
        float inter_x2 = std::min(x2, other.x2);
        float inter_y2 = std::min(y2, other.y2);
        float inter_w = std::max(0.0f, inter_x2 - inter_x1);
        float inter_h = std::max(0.0f, inter_y2 - inter_y1);
        float inter_area = inter_w * inter_h;
        float area1 = width() * height();
        float area2 = other.width() * other.height();
        return inter_area / (area1 + area2 - inter_area);
    }
};

// ==================== Tracking Target ====================

struct TrackedDetection : public DetectionResult {
    int track_id;       // Track ID
    int age;            // Track age (consecutive frames)
    int hits;           // Hit count
    bool lost;          // Whether track is lost
    float vx, vy;       // Motion velocity
    std::vector<float> score_history;  // Historical confidence scores

    TrackedDetection() : track_id(-1), age(0), hits(0), lost(false), vx(0), vy(0) {}
};

// ==================== Channel Configuration ====================

struct ChannelConfig {
    std::string channel_id;
    std::string name;
    std::string rtsp_url;
    std::string username;
    std::string password;
    int width = 0;              // 0 = auto-adapt
    int height = 0;
    int fps = 5;                // Analysis frame rate
    bool enabled = true;
    std::vector<std::pair<float,float,float,float>> rois;  // ROI regions

    ChannelConfig() : channel_id(""), name(""), rtsp_url(""), fps(5) {}
};

// ==================== Alert Event ====================

struct AlertEvent {
    std::string alert_id;
    std::string timestamp;
    std::string channel_id;
    std::string channel_name;
    std::string behavior_type;
    std::string behavior_label;
    std::string alert_level;
    float confidence;
    float bbox[4];              // [x1, y1, x2, y2]
    int tracking_id;
    std::string snapshot_path;
    std::string video_clip_path;
    std::string device_ip;
    std::string status;         // pending/confirmed/ignored
    int64_t created_at;
    int64_t confirmed_at;

    AlertEvent() : confidence(0), tracking_id(-1), status("pending"),
                   created_at(0), confirmed_at(0) {
        bbox[0] = bbox[1] = bbox[2] = bbox[3] = 0;
    }
};

// ==================== System Stats ====================

struct SystemStats {
    float cpu_usage_percent;
    float mem_usage_percent;
    float mem_used_mb;
    float mem_total_mb;
    float npu_usage_percent;
    float npu_temp_celsius;
    float cpu_temp_celsius;
    float disk_usage_percent;
    float disk_free_gb;
    int64_t timestamp;

    SystemStats() : cpu_usage_percent(0), mem_usage_percent(0),
                    mem_used_mb(0), mem_total_mb(0), npu_usage_percent(0),
                    npu_temp_celsius(0), cpu_temp_celsius(0),
                    disk_usage_percent(0), disk_free_gb(0), timestamp(0) {}
};

// ==================== Callback Types ====================

using FrameCallback = std::function<void(const FramePtr&)>;
using AlertCallback = std::function<void(const AlertEvent&)>;
using StatusCallback = std::function<void(const std::string&, ChannelStatus)>;

} // namespace edgevision

#endif // EDGESVISION_TYPES_H
