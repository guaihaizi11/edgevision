#ifndef EDGESVISION_SYSTEM_MONITOR_H
#define EDGESVISION_SYSTEM_MONITOR_H

#include <string>
#include <thread>
#include <atomic>
#include <functional>
#include <mutex>
#include <chrono>
#include \"../types.h\"

namespace edgevision {

// System monitor for CPU, memory, NPU, and temperature
class SystemMonitor {
public:
    SystemMonitor();
    ~SystemMonitor();

    // Start monitoring
    void start();

    // Stop monitoring
    void stop();

    // Get current system stats
    SystemStats getStats();

    // Set callback for stats updates
    void setStatsCallback(StatusCallback cb) { stats_cb_ = std::move(cb); }

private:
    void monitorThread();
    float getCpuUsage();
    float getMemUsage();
    float getNpuUsage();
    float getNpuTemp();
    float getCpuTemp();
    float getDiskUsage();

    std::atomic<bool> running_{false};
    std::thread monitor_thread_;
    SystemStats current_stats_;
    mutable std::mutex stats_mutex_;

    StatusCallback stats_cb_;
};

} // namespace edgevision

#endif // EDGESVISION_SYSTEM_MONITOR_H
