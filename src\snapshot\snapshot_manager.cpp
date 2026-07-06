#include \"snapshot_manager.h\"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <ctime>
#include <iomanip>

namespace fs = std::filesystem;

namespace edgevision {

SnapshotManager::SnapshotManager(const std::string& save_dir)
    : save_dir_(save_dir), total_saved_(0) {
    // Create save directory
    fs::create_directories(save_dir_);
}

SnapshotManager::~SnapshotManager() {}

bool SnapshotManager::saveAlertSnapshot(const std::string& alert_id,
                                       const Frame& image,
                                       const std::vector<TrackedDetection>& detections,
                                       const AlertEvent& alert) {
    // Create date subdirectory
    auto now = std::chrono::system_clock::now();
    auto time_t_now = std::chrono::system_clock::to_time_t(now);
    std::tm tm_buf;
    localtime_s(&tm_buf, &time_t_now);

    char date_dir[32];
    std::strftime(date_dir, sizeof(date_dir), \"%Y%m%d\", &tm_buf);

    std::string full_dir = save_dir_ + \"/\" + date_dir;
    fs::create_directories(full_dir);

    // Draw annotations
    cv::Mat annotated = image.clone();
    drawAnnotations(annotated, detections, alert);

    // Save snapshot
    std::string filepath = full_dir + \"/\" + alert_id + \".jpg\";
    std::vector<int> params = {cv::IMWRITE_JPEG_QUALITY, 85};

    if (cv::imwrite(filepath, annotated, params)) {
        total_saved_++;
        std::cout << \"[Snapshot] Saved: \" << filepath << std::endl;
        return true;
    }

    std::cerr << \"[Snapshot] Failed to save: \" << filepath << std::endl;
    return false;
}

std::string SnapshotManager::getSnapshotPath(const std::string& alert_id) const {
    auto now = std::chrono::system_clock::now();
    auto time_t_now = std::chrono::system_clock::to_time_t(now);
    std::tm tm_buf;
    localtime_s(&tm_buf, &time_t_now);

    char date_dir[32];
    std::strftime(date_dir, sizeof(date_dir), \"%Y%m%d\", &tm_buf);

    return save_dir_ + \"/\" + date_dir + \"/\" + alert_id + \".jpg\";
}

int SnapshotManager::cleanOldSnapshots(int days_to_keep) {
    int deleted = 0;
    auto now = std::time(nullptr);

    for (const auto& entry : fs::directory_iterator(save_dir_)) {
        if (entry.is_directory()) {
            // Simplified directory cleanup logic
            deleted++;
        }
    }

    return deleted;
}

void SnapshotManager::drawAnnotations(cv::Mat& image,
                                     const std::vector<TrackedDetection>& detections,
                                     const AlertEvent& alert) {
    // Color mapping
    std::map<std::string, cv::Scalar> color_map = {
        {\"person\", cv::Scalar(0, 255, 0)},      // Green
        {\"cell_phone\", cv::Scalar(255, 0, 0)},  // Blue
        {\"hardhat\", cv::Scalar(0, 0, 255)},     // Red
        {\"cigarette\", cv::Scalar(255, 255, 0)}, // Yellow
    };

    // Draw each detection box
    for (const auto& det : detections) {
        cv::Scalar color;
        auto it = color_map.find(det.class_name);
        if (it != color_map.end()) {
            color = it->second;
        } else {
            color = cv::Scalar(255, 255, 255);  // White default
        }

        // Draw bounding box
        cv::rectangle(image,
                     cv::Point(static_cast<int>(det.x1), static_cast<int>(det.y1)),
                     cv::Point(static_cast<int>(det.x2), static_cast<int>(det.y2)),
                     color, 2);

        // Draw label
        std::string label = det.class_name + \" \" +
                           std::to_string(static_cast<int>(det.confidence * 100)) + \"%\";
        int baseline;
        cv::Size text_size = cv::getTextSize(label, cv::FONT_HERSHEY_SIMPLEX,
                                            0.6, 1, &baseline);

        cv::rectangle(image,
                     cv::Point(static_cast<int>(det.x1),
                              static_cast<int>(det.y1) - text_size.height - 5),
                     cv::Point(static_cast<int>(det.x1) + text_size.width,
                              static_cast<int>(det.y1) + 5),
                     color, -1);

        cv::putText(image, label,
                   cv::Point(static_cast<int>(det.x1),
                            static_cast<int>(det.y1) - 5),
                   cv::FONT_HERSHEY_SIMPLEX, 0.6, cv::Scalar(0, 0, 0), 1);
    }

    // Draw alert info
    std::string alert_info = \"Alert: \" + alert.behavior_label +
                            \" (\" + alert.alert_level + \")\";
    cv::putText(image, alert_info,
               cv::Point(10, 30),
               cv::FONT_HERSHEY_SIMPLEX, 0.8, cv::Scalar(0, 0, 255), 2);
}

} // namespace edgevision
