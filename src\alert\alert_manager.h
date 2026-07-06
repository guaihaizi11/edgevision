#ifndef EDGESVISION_ALERT_MANAGER_H
#define EDGESVISION_ALERT_MANAGER_H

#include <string>
#include <vector>
#include <memory>
#include <queue>
#include <thread>
#include <mutex>
#include <atomic>
#include <functional>
#include \"../types.h\"
#include \"../storage/alert_db.h\"
#include \"../snapshot/snapshot_manager.h\"

namespace edgevision {

// MQTT publisher (x86 simulated implementation)
class MqttPublisher {
public:
    MqttPublisher(const std::string& broker = \"127.0.0.1\", int port = 1883);
    bool connect();
    void publish(const std::string& topic, const std::string& message);
    void disconnect();
    bool isConnected() const { return connected_; }
private:
    std::string broker_;
    int port_;
    bool connected_ = false;
};

// HTTP callback handler
class HttpCallback {
public:
    HttpCallback(const std::string& url = \"\");
    bool sendAlert(const AlertEvent& alert, const std::string& snapshot_path);
    std::string getUrl() const { return url_; }
    void setUrl(const std::string& url) { url_ = url; }
private:
    std::string url_;
};

// Manages alert events, snapshot saving, and reporting
class AlertManager {
public:
    AlertManager(const std::string& db_path = \"./data/edgevision.db\",
                const std::string& snapshot_dir = \"./data/alerts\");
    ~AlertManager();

    // Handle alert event
    void handleAlert(const AlertEvent& alert, const Frame& image,
                    const std::vector<TrackedDetection>& detections);

    // Set MQTT publisher
    void setMqttPublisher(std::shared_ptr<MqttPublisher> mqtt) { mqtt_ = mqtt; }

    // Set HTTP callback
    void setHttpCallback(std::shared_ptr<HttpCallback> http) { http_ = http; }

    // Query alerts
    std::vector<AlertEvent> queryAlerts(const std::string& channel_id,
                                       const std::string& behavior_type,
                                       const std::string& status,
                                       int page, int page_size);

    // Update alert status
    bool updateAlertStatus(const std::string& alert_id, const std::string& status);

    // Get statistics
    AlertDB::AlertStats getStats();

    // Set alert callback
    void setAlertCallback(AlertCallback callback) { alert_callback_ = std::move(callback); }

private:
    void saveSnapshotThread();

    AlertDB db_;
    SnapshotManager snapshot_mgr_;
    std::shared_ptr<MqttPublisher> mqtt_;
    std::shared_ptr<HttpCallback> http_;

    struct PendingAlert {
        AlertEvent alert;
        FramePtr image;
        std::vector<TrackedDetection> detections;
    };

    std::queue<PendingAlert> pending_queue_;
    std::mutex queue_mutex_;
    std::thread save_thread_;
    std::atomic<bool> running_{false};

    AlertCallback alert_callback_;
};

} // namespace edgevision

#endif // EDGESVISION_ALERT_MANAGER_H
