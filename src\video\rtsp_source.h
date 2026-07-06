#ifndef EDGESVISION_RTSP_SOURCE_H
#define EDGESVISION_RTSP_SOURCE_H

#include <string>
#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <chrono>
#include <functional>
#include \"../types.h\"
#include \"../core/frame_queue.h\"

namespace edgevision {

// RTSP source - uses OpenCV VideoCapture for x86
// On RK3588, replace with GStreamer + V4L2 m2m
class RtspSource {
public:
    RtspSource();
    ~RtspSource();

    // Initialize and start
    bool start(const std::string& url, int target_fps = 5);

    // Stop
    void stop();

    // Get frame (blocking)
    bool getFrame(FramePtr& frame, int timeout_ms = 100);

    // Non-blocking get
    bool tryGetFrame(FramePtr& frame);

    // Status queries
    bool isRunning() const { return running_.load(); }
    int getFPS() const;
    int getFrameCount() const { return frame_count_.load(); }
    int getDropCount() const { return drop_count_.load(); }
    ChannelStatus getStatus() const { return status_.load(); }

    // Callback setup
    void setStatusCallback(StatusCallback cb) { status_cb_ = std::move(cb); }

private:
    void captureThread();
    void heartbeatThread();
    void reconnect();

    std::string url_;
    std::string username_;
    std::string password_;
    int target_fps_;

    // Video capture handle (OpenCV on x86, GStreamer on RK3588)
    void* capture_handle_;

    FrameQueue frame_queue_;
    std::atomic<bool> running_;
    std::atomic<ChannelStatus> status_;
    std::atomic<int> frame_count_;
    std::atomic<int> drop_count_;
    int reconnect_attempts_;

    std::thread capture_thread_;
    std::thread heartbeat_thread_;

    StatusCallback status_cb_;

    mutable std::mutex status_mutex_;
};

} // namespace edgevision

#endif // EDGESVISION_RTSP_SOURCE_H
