#include \"rtsp_source.h\"
#include <iostream>

namespace edgevision {

RtspSource::RtspSource()
    : target_fps_(5), running_(false), status_(ChannelStatus::STOPPED),
      frame_count_(0), drop_count_(0), reconnect_attempts_(0) {
    capture_handle_ = nullptr;
}

RtspSource::~RtspSource() {
    stop();
}

bool RtspSource::start(const std::string& url, int target_fps) {
    url_ = url;
    target_fps_ = target_fps;
    reconnect_attempts_ = 0;

    std::cout << \"[RtspSource] Starting channel: \" << url << std::endl;

    // VideoCapture (x86 implementation)
    // On RK3588, this is replaced with GStreamer pipeline
    cv::VideoCapture* cap = new cv::VideoCapture();
    if (!cap->open(url, cv::CAP_FFMPEG)) {
        delete cap;
        capture_handle_ = nullptr;
        status_ = ChannelStatus::ERROR;
        if (status_cb_) status_cb_(\"\", ChannelStatus::ERROR);
        return false;
    }
    capture_handle_ = cap;

    running_ = true;
    status_ = ChannelStatus::RUNNING;
    reconnect_attempts_ = 0;

    if (status_cb_) status_cb_(\"\", ChannelStatus::RUNNING);

    capture_thread_ = std::thread(&RtspSource::captureThread, this);
    heartbeat_thread_ = std::thread(&RtspSource::heartbeatThread, this);

    return true;
}

void RtspSource::stop() {
    running_ = false;
    if (capture_thread_.joinable()) capture_thread_.join();
    if (heartbeat_thread_.joinable()) heartbeat_thread_.join();

    if (capture_handle_) {
        cv::VideoCapture* cap = static_cast<cv::VideoCapture*>(capture_handle_);
        cap->release();
        delete cap;
        capture_handle_ = nullptr;
    }
    status_ = ChannelStatus::STOPPED;
    frame_queue_.clear();
}

bool RtspSource::getFrame(FramePtr& frame, int timeout_ms) {
    return frame_queue_.pop(frame, timeout_ms);
}

bool RtspSource::tryGetFrame(FramePtr& frame) {
    return frame_queue_.try_pop(frame);
}

int RtspSource::getFPS() const {
    if (!capture_handle_) return 0;
    cv::VideoCapture* cap = static_cast<cv::VideoCapture*>(capture_handle_);
    return static_cast<int>(cap->get(cv::CAP_PROP_FPS));
}

void RtspSource::captureThread() {
    if (!capture_handle_) return;
    cv::VideoCapture* cap = static_cast<cv::VideoCapture*>(capture_handle_);

    int frame_interval_ms = 1000 / target_fps_;
    auto last_frame_time = std::chrono::steady_clock::now();

    while (running_) {
        cv::Mat mat;
        if (!cap->read(mat) || mat.empty()) {
            drop_count_++;
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            continue;
        }

        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - last_frame_time).count();

        if (elapsed >= frame_interval_ms) {
            auto frame_ptr = std::make_shared<Frame>(mat);
            frame_queue_.push(frame_ptr);
            frame_count_++;
            last_frame_time = now;
        }
    }
}

void RtspSource::heartbeatThread() {
    while (running_) {
        std::this_thread::sleep_for(std::chrono::seconds(10));
        if (!running_) break;

        // Check if frames have arrived in the last 30 seconds
        // Simplified: check if frame_count_ is increasing
        // Production should use more precise timestamps
    }
}

void RtspSource::reconnect() {
    if (!running_) return;

    int delay = std::min(5 * (1 << reconnect_attempts_), 60);
    std::this_thread::sleep_for(std::chrono::seconds(delay));
    reconnect_attempts_++;

    status_ = ChannelStatus::RECONNECTING;
    if (status_cb_) status_cb_(\"\", ChannelStatus::RECONNECTING);

    if (start(url_, target_fps_)) {
        reconnect_attempts_ = 0;
    }
}

} // namespace edgevision
