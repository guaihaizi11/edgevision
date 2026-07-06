#include \"frame_queue.h\"

namespace edgevision {

FrameQueue::FrameQueue(int max_size) : max_size_(max_size) {}

bool FrameQueue::push(const FramePtr& frame) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (static_cast<int>(queue_.size()) >= max_size_) {
        queue_.pop();
        dropped_++;
    }
    queue_.push(frame);
    cv_.notify_one();
    return true;
}

bool FrameQueue::pop(FramePtr& frame, int timeout_ms) {
    std::unique_lock<std::mutex> lock(mutex_);
    if (!cv_.wait_for(lock, std::chrono::milliseconds(timeout_ms),
                      [this]{ return !queue_.empty(); })) {
        return false;
    }
    frame = queue_.front();
    queue_.pop();
    return true;
}

bool FrameQueue::try_pop(FramePtr& frame) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (queue_.empty()) return false;
    frame = queue_.front();
    queue_.pop();
    return true;
}

int FrameQueue::size() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return static_cast<int>(queue_.size());
}

bool FrameQueue::empty() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return queue_.empty();
}

} // namespace edgevision
