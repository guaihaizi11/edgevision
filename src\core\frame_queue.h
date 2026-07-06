#ifndef EDGESVISION_FRAME_QUEUE_H
#define EDGESVISION_FRAME_QUEUE_H

#include <string>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <memory>
#include \"../types.h\"

namespace edgevision {

// Thread-safe frame queue with bounded capacity
class FrameQueue {
public:
    explicit FrameQueue(int max_size = 10);

    // Push a frame (drops oldest if full)
    bool push(const FramePtr& frame);

    // Pop a frame (blocking with timeout)
    bool pop(FramePtr& frame, int timeout_ms = 100);

    // Non-blocking pop
    bool try_pop(FramePtr& frame);

    // Clear all frames
    void clear() {
        std::lock_guard<std::mutex> lock(mutex_);
        while (!queue_.empty()) queue_.pop();
    }

    int size() const;
    bool empty() const;
    int dropped_count() const { return dropped_.load(); }

private:
    int max_size_;
    std::queue<FramePtr> queue_;
    mutable std::mutex mutex_;
    std::condition_variable cv_;
    std::atomic<int> dropped_{0};
};

} // namespace edgevision

#endif // EDGESVISION_FRAME_QUEUE_H
