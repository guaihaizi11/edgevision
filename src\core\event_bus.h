#ifndef EDGESVISION_EVENT_BUS_H
#define EDGESVISION_EVENT_BUS_H

#include <string>
#include <functional>
#include <map>
#include <mutex>
#include <vector>
#include <memory>
#include \"../types.h\"

namespace edgevision {

class EventBus {
public:
    static EventBus& instance();

    // Subscribe to events
    void subscribe(const std::string& event_type, FrameCallback callback);
    void subscribe(const std::string& event_type, AlertCallback callback);
    void subscribe(const std::string& event_type, StatusCallback callback);

    // Publish events
    void publish_frame(const std::string& channel_id, const FramePtr& frame);
    void publish_alert(const std::string& channel_id, const AlertEvent& alert);
    void publish_status(const std::string& channel_id, ChannelStatus status);

    // Unsubscribe
    void unsubscribe(const std::string& event_type);

private:
    EventBus() = default;
    ~EventBus() = default;
    EventBus(const EventBus&) = delete;
    EventBus& operator=(const EventBus&) = delete;

    struct Handler {
        virtual ~Handler() = default;
    };

    template<typename T>
    struct TypedHandler : Handler {
        T callback;
    };

    std::map<std::string, std::vector<std::shared_ptr<Handler>>> handlers_;
    std::mutex mutex_;
};

} // namespace edgevision

#endif // EDGESVISION_EVENT_BUS_H
