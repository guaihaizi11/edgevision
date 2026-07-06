#ifndef EDGESVISION_BEHAVIOR_ENGINE_H
#define EDGESVISION_BEHAVIOR_ENGINE_H

#include <string>
#include <vector>
#include <map>
#include <mutex>
#include <chrono>
#include \"../types.h\"

namespace edgevision {

enum class ConditionType {
    SPATIAL_OVERLAP,
    TEMPORAL_CONSISTENCY,
    NEGATIVE_DETECTION,
    MOTION_PATTERN
};

struct RuleCondition {
    ConditionType type;
    std::vector<std::string> objects;
    std::string target_class;
    std::vector<std::string> absent_classes;
    float iou_threshold = 0.3f;
    int duration_frames = 10;
    int min_consecutive_frames = 5;
    float duration_seconds = 2.0f;
    bool roi_only = false;
    std::string region;
    float min_movement_score = 0.5f;
};

struct RuleAction {
    std::string alert_level;
    std::string mqtt_topic;
    bool snapshot = false;
    bool video_clip = false;
    float min_confidence = 0.5f;
    int cooldown_seconds = 30;
};

struct BehaviorRule {
    std::string rule_id;
    std::string name;
    std::string description;
    int priority;
    bool enabled;
    std::vector<RuleCondition> conditions;
    RuleAction action;
};

// Behavior analysis engine - evaluates detection results against configurable rules
class BehaviorEngine {
public:
    BehaviorEngine();
    ~BehaviorEngine();

    // Load behavior rules from YAML file
    bool loadRules(const std::string& yaml_file);

    // Add/remove rules dynamically
    bool addRule(const BehaviorRule& rule);
    bool removeRule(const std::string& rule_id);

    // Analyze tracked detections against rules
    std::vector<AlertEvent> analyze(
        const std::string& channel_id,
        const std::string& channel_name,
        const std::vector<TrackedDetection>& tracked_dets);

private:
    bool matchCondition(const RuleCondition& cond,
                       const std::vector<TrackedDetection>& dets);
    bool isInCooldown(const std::string& channel_id, const std::string& rule_id);

    std::vector<BehaviorRule> rules_;
    std::mutex mutex_;

    // Cooldown tracking: channel_id:rule_id -> last_alert_time
    std::map<std::string, std::chrono::steady_clock::time_point> cooldowns_;
    mutable std::mutex cooldown_mutex_;
};

} // namespace edgevision

#endif // EDGESVISION_BEHAVIOR_ENGINE_H
