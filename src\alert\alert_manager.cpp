#include \"alert_manager.h\"
#include <iostream>
#include <sstream>
#include <chrono>
#include <ctime>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace edgevision {

// ==================== MqttPublisher ====================

MqttPublisher::MqttPublisher(const std::string& broker, int port)
    : broker_(broker), port_(port) {}

bool MqttPublisher::connect() {
    // x86 simulated implementation
    std::cout << \"[MQTT] Connecting to \" << broker_ << \":\" << port_ << std::endl;
    connected_ = true;
    return true;
}

void MqttPublisher::publish(const std::string& topic, const std::string& message) {
    if (!connected_) return;
    std::cout << \"[MQTT] Publish to \" << topic << \": \" << message << std::endl;
}

void MqttPublisher::disconnect() {
    connected_ = false;
}

// ==================== HttpCallback ====================

HttpCallback::HttpCallback(const std::string& url) : url_(url) {}

bool HttpCallback::sendAlert(const AlertEvent& alert, const std::string& snapshot_path) {
    if (url_.empty()) return false;

    json j;
    j[\"alert_id\"] = alert.alert_id;
    j[\"timestamp\"] = alert.timestamp;
    j[\"channel_id\"] = alert.channel_id;
    j[\"behavior_type\"] = alert.behavior_type;
    j[\"alert_level\"] = alert.alert_level;
    j[\"confidence\"] = alert.confidence;
    j[\"status\"] = alert.status;

    std::string body = j.dump();
    std::cout << \"[HTTP] POST to \" << url_ << \": \" << body << std::endl;

    // TODO: Use libcurl to send actual HTTP request
    return true;
}

// ==================== AlertManager ====================

AlertManager::AlertManager(const std::string& db_path, const std::string& snapshot_dir)
    : db_(db_path), snapshot_mgr_(snapshot_dir) {
    running_ = true;
    save_thread_ = std::thread(&AlertManager::saveSnapshotThread, this);
}

AlertManager::~AlertManager() {
    running_ = false;
    if (save_thread_.joinable()) {
        save_thread_.join();
    }
}

void AlertManager::handleAlert(const AlertEvent& alert, const Frame& image,
                               const std::vector<TrackedDetection>& detections) {
    // Create pending alert
    PendingAlert pending;
    pending.alert = alert;
    pending.image = std::make_shared<Frame>(image);
    pending.detections = detections;

    {
        std::lock_guard<std::mutex> lock(queue_mutex_);
        pending_queue_.push(std::move(pending));
    }

    // Call callback
    if (alert_callback_) {
        alert_callback_(alert);
    }

    std::cout << \"[Alert] Received: \" << alert.behavior_label
              << \" (ID: \" << alert.alert_id << \")\" << std::endl;
}

void AlertManager::saveSnapshotThread() {
    while (running_) {
        PendingAlert pending;
        {
            std::lock_guard<std::mutex> lock(queue_mutex_);
            if (pending_queue_.empty()) {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                continue;
            }
            pending = std::move(pending_queue_.front());
            pending_queue_.pop();
        }

        // Save snapshot
        std::string snap_path = snapshot_mgr_.getSnapshotPath(pending.alert.alert_id);
        pending.alert.snapshot_path = snap_path;

        snapshot_mgr_.saveAlertSnapshot(pending.alert.alert_id, *pending.image,
                                       pending.detections, pending.alert);

        // Store in database
        db_.insertAlert(pending.alert);

        // MQTT report
        if (mqtt_ && mqtt_->isConnected()) {
            json j;
            j[\"alert_id\"] = pending.alert.alert_id;
            j[\"timestamp\"] = pending.alert.timestamp;
            j[\"channel_id\"] = pending.alert.channel_id;
            j[\"behavior_type\"] = pending.alert.behavior_type;
            j[\"alert_level\"] = pending.alert.alert_level;
            j[\"confidence\"] = pending.alert.confidence;

            mqtt_->publish(pending.alert.alert_level, j.dump());
        }

        // HTTP callback
        if (http_) {
            http_->sendAlert(pending.alert, snap_path);
        }
    }
}

std::vector<AlertEvent> AlertManager::queryAlerts(const std::string& channel_id,
                                                 const std::string& behavior_type,
                                                 const std::string& status,
                                                 int page, int page_size) {
    return db_.queryAlerts(channel_id, behavior_type, status, page, page_size);
}

bool AlertManager::updateAlertStatus(const std::string& alert_id, const std::string& status) {
    return db_.updateAlertStatus(alert_id, status);
}

AlertDB::AlertStats AlertManager::getStats() {
    return db_.getStats();
}

} // namespace edgevision
