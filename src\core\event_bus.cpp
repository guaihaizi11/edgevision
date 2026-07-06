#include \"event_bus.h\"
#include <iostream>

namespace edgevision {

EventBus& EventBus::instance() {
    static EventBus inst;
    return inst;
}

void EventBus::subscribe(const std::string& event_type, FrameCallback callback) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto handler = std::make_shared<TypedHandler<FrameCallback>>();
    handler->callback = callback;
    handlers_[event_type].push_back(handler);
}

void EventBus::subscribe(const std::string& event_type, AlertCallback callback) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto handler = std::make_shared<TypedHandler<AlertCallback>>();
    handler->callback = callback;
    handlers_[event_type].push_back(handler);
}

void EventBus::subscribe(const std::string& event_type, StatusCallback callback) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto handler = std::make_shared<TypedHandler<StatusCallback>>();
    handler->callback = callback;
    handlers_[event_type].push_back(handler);
}

void EventBus::publish_frame(const std::string& channel_id, const FramePtr& frame) {
    std::string event = \"frame:\" + channel_id;
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = handlers_.find(event);
    if (it == handlers_.end()) return;
    for (auto& h : it->second) {
        auto typed = dynamic_cast<TypedHandler<FrameCallback>*>(h.get());
        if (typed) typed->callback(frame);
    }
}

void EventBus::publish_alert(const std::string& channel_id, const AlertEvent& alert) {
    std::string event = \"alert:\" + channel_id;
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = handlers_.find(event);
    if (it == handlers_.end()) return;
    for (auto& h : it->second) {
        auto typed = dynamic_cast<TypedHandler<AlertCallback>*>(h.get());
        if (typed) typed->callback(alert);
    }
}

void EventBus::publish_status(const std::string& channel_id, ChannelStatus status) {
    std::string event = \"status:\" + channel_id;
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = handlers_.find(event);
    if (it == handlers_.end()) return;
    for (auto& h : it->second) {
        auto typed = dynamic_cast<TypedHandler<StatusCallback>*>(h.get());
        if (typed) typed->callback(channel_id, status);
    }
}

void EventBus::unsubscribe(const std::string& event_type) {
    std::lock_guard<std::mutex> lock(mutex_);
    handlers_.erase(event_type);
}

} // namespace edgevision