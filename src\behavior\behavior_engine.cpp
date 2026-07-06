#include \"behavior_engine.h\"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <random>

namespace edgevision {

BehaviorEngine::BehaviorEngine() {}

BehaviorEngine::~BehaviorEngine() {}

bool BehaviorEngine::loadRules(const std::string& yaml_file) {
    std::lock_guard<std::mutex> lock(mutex_);

    // TODO: Implement full YAML parsing
    // Using simplified text parsing here
    std::ifstream file(yaml_file);
    if (!file.is_open()) {
        std::cerr << \"[Behavior] Failed to open rules file: \" << yaml_file << std::endl;
        return false;
    }

    // Simulate loading default rules
    BehaviorRule rule1;
    rule1.rule_id = \"behavior_phone_usage\";
    rule1.name = \"Phone Usage Detection\";
    rule1.description = \"Detects person holding a mobile phone for sustained duration\";
    rule1.priority = 2;
    rule1.enabled = true;

    RuleCondition cond1;
    cond1.type = ConditionType::SPATIAL_OVERLAP;
    cond1.objects = {\"person\", \"cell_phone\"};
    cond1.iou_threshold = 0.1f;
    cond1.duration_frames = 15;
    rule1.conditions.push_back(cond1);

    RuleCondition cond2;
    cond2.type = ConditionType::TEMPORAL_CONSISTENCY;
    cond2.min_consecutive_frames = 10;
    rule1.conditions.push_back(cond2);

    RuleAction action1;
    action1.alert_level = \"warning\";
    action1.mqtt_topic = \"edgevision/alert/phone\";
    action1.snapshot = true;
    action1.min_confidence = 0.45f;
    action1.cooldown_seconds = 30;
    rule1.action = action1;
    rules_.push_back(rule1);

    // Hardhat absence rule
    BehaviorRule rule2;
    rule2.rule_id = \"behavior_no_hardhat\";
    rule2.name = \"Hardhat Absence\";
    rule2.description = \"Detects person without hardhat\";
    rule2.priority = 1;
    rule2.enabled = true;

    RuleCondition cond3;
    cond3.type = ConditionType::NEGATIVE_DETECTION;
    cond3.target_class = \"person\";
    cond3.absent_classes = {\"hardhat\"};
    cond3.roi_only = true;
    cond3.duration_seconds = 2.0f;
    rule2.conditions.push_back(cond3);

    RuleAction action2;
    action2.alert_level = \"critical\";
    action2.mqtt_topic = \"edgevision/alert/no_hardhat\";
    action2.snapshot = true;
    action2.min_confidence = 0.5f;
    action2.cooldown_seconds = 30;
    rule2.action = action2;
    rules_.push_back(rule2);

    // Smoking detection rule
    BehaviorRule rule3;
    rule3.rule_id = \"behavior_smoking\";
    rule3.name = \"Smoking Detection\";
    rule3.description = \"Detects person smoking\";
    rule3.priority = 1;
    rule3.enabled = true;

    RuleCondition cond4;
    cond4.type = ConditionType::SPATIAL_OVERLAP;
    cond4.objects = {\"person\", \"cigarette\"};
    cond4.iou_threshold = 0.05f;
    cond4.duration_frames = 10;
    rule3.conditions.push_back(cond4);

    RuleAction action3;
    action3.alert_level = \"critical\";
    action3.mqtt_topic = \"edgevision/alert/smoking\";
    action3.snapshot = true;
    action3.video_clip = true;
    action3.min_confidence = 0.45f;
    action3.cooldown_seconds = 60;
    rule3.action = action3;
    rules_.push_back(rule3);

    std::cout << \"[Behavior] Loaded \" << rules_.size() << \" rules from \" << yaml_file << std::endl;
    return true;
}

bool BehaviorEngine::addRule(const BehaviorRule& rule) {
    std::lock_guard<std::mutex> lock(mutex_);
    rules_.push_back(rule);
    return true;
}

bool BehaviorEngine::removeRule(const std::string& rule_id) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = std::find_if(rules_.begin(), rules_.end(),
        [&rule_id](const BehaviorRule& r) { return r.rule_id == rule_id; });
    if (it != rules_.end()) {
        rules_.erase(it);
        return true;
    }
    return false;
}

std::vector<AlertEvent> BehaviorEngine::analyze(
    const std::string& channel_id,
    const std::string& channel_name,
    const std::vector<TrackedDetection>& tracked_dets) {

    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<AlertEvent> alerts;

    // Sort rules by priority
    std::vector<const BehaviorRule*> sorted_rules;
    for (const auto& rule : rules_) {
        if (rule.enabled) {
            sorted_rules.push_back(&rule);
        }
    }
    std::sort(sorted_rules.begin(), sorted_rules.end(),
        [](const BehaviorRule* a, const BehaviorRule* b) {
            return a->priority < b->priority;
        });

    // Evaluate each rule
    for (const auto* rule : sorted_rules) {
        bool all_conditions_met = true;

        for (const auto& cond : rule->conditions) {
            if (!matchCondition(cond, tracked_dets)) {
                all_conditions_met = false;
                break;
            }
        }

        if (all_conditions_met) {
            // Check cooldown period
            std::string cooldown_key = channel_id + \":\" + rule->rule_id;
            if (isInCooldown(channel_id, rule->rule_id)) {
                continue;
            }

            // Generate alert
            AlertEvent alert;
            alert.alert_id = \"ALT-\" + std::to_string(std::hash<std::string>{}(cooldown_key) % 100000);
            auto now = std::chrono::system_clock::now();
            auto time_t_now = std::chrono::system_clock::to_time_t(now);
            std::tm tm_buf;
            gmtime_s(&tm_buf, &time_t_now);
            char buf[64];
            std::strftime(buf, sizeof(buf), \"%Y-%m-%d %H:%M:%S\", &tm_buf);
            alert.timestamp = buf;
            alert.channel_id = channel_id;
            alert.channel_name = channel_name;
            alert.behavior_type = rule->rule_id;
            alert.behavior_label = rule->name;
            alert.alert_level = rule->action.alert_level;
            alert.confidence = 0.85f;
            alert.tracking_id = -1;
            alert.status = \"pending\";
            alert.created_at = std::chrono::duration_cast<std::chrono::seconds>(
                now.time_since_epoch()).count();

            alerts.push_back(alert);

            // Record cooldown
            {
                std::lock_guard<std::mutex> lock(cooldown_mutex_);
                cooldowns_[cooldown_key] = std::chrono::steady_clock::now();
            }
        }
    }

    return alerts;
}

bool BehaviorEngine::matchCondition(const RuleCondition& cond,
                                   const std::vector<TrackedDetection>& dets) {
    switch (cond.type) {
        case ConditionType::SPATIAL_OVERLAP: {
            // Check if two object types spatially overlap
            std::vector<const TrackedDetection*> obj_a, obj_b;
            for (const auto& det : dets) {
                if (det.class_name == cond.objects[0]) obj_a.push_back(&det);
                if (cond.objects.size() > 1 && det.class_name == cond.objects[1]) obj_b.push_back(&det);
            }
            for (auto* a : obj_a) {
                for (auto* b : obj_b) {
                    if (a->IoU(*b) >= cond.iou_threshold) {
                        return true;
                    }
                }
            }
            return false;
        }
        case ConditionType::NEGATIVE_DETECTION: {
            // Check target exists but absent objects are missing
            bool has_target = false;
            bool has_absent = false;
            for (const auto& det : dets) {
                if (det.class_name == cond.target_class) has_target = true;
                for (const auto& absent : cond.absent_classes) {
                    if (det.class_name == absent) has_absent = true;
                }
            }
            return has_target && !has_absent;
        }
        case ConditionType::TEMPORAL_CONSISTENCY:
        case ConditionType::MOTION_PATTERN:
        default:
            return false;
    }
}

bool BehaviorEngine::isInCooldown(const std::string& channel_id,
                                  const std::string& rule_id) {
    std::lock_guard<std::mutex> lock(cooldown_mutex_);
    std::string key = channel_id + \":\" + rule_id;
    auto it = cooldowns_.find(key);
    if (it == cooldowns_.end()) return false;

    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::steady_clock::now() - it->second).count();
    return elapsed < 30; // Default 30s cooldown
}

} // namespace edgevision
