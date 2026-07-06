#include \"system_monitor.h\"
#include <iostream>
#include <random>

#ifdef _WIN32
#include <windows.h>
#include <psapi.h>
#else
#include <sys/sysinfo.h>
#include <fstream>
#endif

namespace edgevision {

SystemMonitor::SystemMonitor() {}

SystemMonitor::~SystemMonitor() {
    stop();
}

void SystemMonitor::start() {
    running_ = true;
    monitor_thread_ = std::thread(&SystemMonitor::monitorThread, this);
    std::cout << \"[Monitor] System monitor started\" << std::endl;
}

void SystemMonitor::stop() {
    running_ = false;
    if (monitor_thread_.joinable()) {
        monitor_thread_.join();
    }
    std::cout << \"[Monitor] System monitor stopped\" << std::endl;
}

SystemStats SystemMonitor::getStats() {
    std::lock_guard<std::mutex> lock(stats_mutex_);
    return current_stats_;
}

void SystemMonitor::monitorThread() {
    while (running_) {
        current_stats_.cpu_usage_percent = getCpuUsage();
        current_stats_.mem_usage_percent = getMemUsage();
        current_stats_.mem_used_mb = current_stats_.mem_usage_percent * current_stats_.mem_total_mb / 100.0f;
        current_stats_.npu_usage_percent = getNpuUsage();
        current_stats_.npu_temp_celsius = getNpuTemp();
        current_stats_.cpu_temp_celsius = getCpuTemp();
        current_stats_.disk_usage_percent = getDiskUsage();
        current_stats_.timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::steady_clock::now().time_since_epoch()).count();

        if (stats_cb_) {
            stats_cb_(\"monitor\", ChannelStatus::RUNNING);
        }

        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

float SystemMonitor::getCpuUsage() {
#ifdef _WIN32
    // Windows CPU usage
    static FILETIME prev_idle = {0}, prev_kernel = {0}, prev_user = {0};
    FILETIME idle, kernel, user;
    GetSystemTimes(&idle, &kernel, &user);

    ULARGE_INTEGER idle_u, kernel_u, user_u;
    memcpy(&idle_u.QuadPart, &idle, sizeof(ULARGE_INTEGER));
    memcpy(&kernel_u.QuadPart, &kernel, sizeof(ULARGE_INTEGER));
    memcpy(&user_u.QuadPart, &user, sizeof(ULARGE_INTEGER));

    if (prev_idle.QuadPart == 0) {
        prev_idle = idle_u;
        prev_kernel = kernel_u;
        prev_user = user_u;
        return 0.0f;
    }

    ULONGLONG idle_d = idle_u.QuadPart - prev_idle.QuadPart;
    ULONGLONG kernel_d = kernel_u.QuadPart - prev_kernel.QuadPart;
    ULONGLONG user_d = user_u.QuadPart - prev_user.QuadPart;
    ULONGLONG total_d = kernel_d + user_d - idle_d;
    ULONGLONG idle_total = idle_d;

    prev_idle = idle_u;
    prev_kernel = kernel_u;
    prev_user = user_u;

    if (total_d == 0) return 0.0f;
    return (1.0f - static_cast<float>(idle_d) / total_d) * 100.0f;
#else
    // Linux: read /proc/stat
    std::ifstream stat_file(\"/proc/stat\");
    std::string line;
    if (std::getline(stat_file, line)) {
        // Simplified: return simulated value
        return 45.0f + (std::rand() % 20);
    }
    return 0.0f;
#endif
}

float SystemMonitor::getMemUsage() {
#ifdef _WIN32
    MEMORYSTATUSEX memInfo;
    memInfo.dwLength = sizeof(memInfo);
    GlobalMemoryStatusEx(&memInfo);
    current_stats_.mem_total_mb = memInfo.ullTotalPhys / (1024 * 1024);
    return static_cast<float>(memInfo.dwMemoryLoad);
#else
    struct sysinfo info;
    sysinfo(&info);
    current_stats_.mem_total_mb = info.totalram / (1024 * 1024);
    return static_cast<float>(info.totalram - info.freeram) / info.totalram * 100.0f;
#endif
}

float SystemMonitor::getNpuUsage() {
    // TODO: Implement real NPU usage on RK3588
    // Read from /sys/class/thermal or RKNN SDK
    return 0.0f; // Simulated for x86
}

float SystemMonitor::getNpuTemp() {
    // TODO: Read NPU temperature from RK3588 thermal zone
    return 42.0f; // Simulated
}

float SystemMonitor::getCpuTemp() {
#ifdef _WIN32
    return 38.0f; // Simulated on Windows
#else
    std::ifstream temp_file(\"/sys/class/thermal/thermal_zone0/temp\");
    float temp = 0.0f;
    if (temp_file >> temp) {
        return temp / 1000.0f;
    }
    return 38.0f; // Simulated
#endif
}

float SystemMonitor::getDiskUsage() {
    // TODO: Implement disk usage monitoring
    return 35.0f; // Simulated
}

} // namespace edgevision
