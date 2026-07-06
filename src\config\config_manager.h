#ifndef EDGESVISION_CONFIG_MANAGER_H
#define EDGESVISION_CONFIG_MANAGER_H

#include <string>
#include <vector>
#include <memory>
#include \"../types.h\"

namespace edgevision {

// Manages channel, behavior, and system configuration
class ConfigManager {
public:
    explicit ConfigManager(const std::string& config_dir = \"./config\");
    ~ConfigManager();

    // Load all configurations
    bool load();

    // Get channel configurations
    std::vector<ChannelConfig> getChannelConfigs() const;

    // Add a new channel
    void addChannel(const ChannelConfig& config);

    // Get behavior rules
    std::vector<BehaviorRule> getBehaviorRules() const;

    // Get system configuration
    struct SystemConfig {
        int max_concurrent_channels = 8;
        int fps_sampling = 5;
        int alert_cooldown_seconds = 30;
        int snapshot_quality = 85;
        std::string storage_path = \"./data/alerts\";
        std::string db_path = \"./data/edgevision.db\";
        std::string log_level = \"info\";
        std::string log_path = \"./logs\";
    };
    SystemConfig getSystemConfig() const;

    // Save configurations back to files
    bool save();

private:
    bool loadChannels();
    bool loadBehaviors();
    bool loadSystem();

    std::string config_dir_;
    std::vector<ChannelConfig> channels_;
    std::vector<BehaviorRule> behavior_rules_;
    SystemConfig system_config_;
    bool modified_ = false;
};

} // namespace edgevision

#endif // EDGESVISION_CONFIG_MANAGER_H
