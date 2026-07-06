#include \"config_manager.h\"
#include <fstream>
#include <iostream>
#include <sstream>
#include <filesystem>

namespace fs = std::filesystem;

namespace edgevision {

ConfigManager::ConfigManager(const std::string& config_dir)
    : config_dir_(config_dir) {
    // Ensure config directory exists
    fs::create_directories(config_dir_);
    fs::create_directories(fs::path(config_dir_) / \"..\" / \"data\");
}

ConfigManager::~ConfigManager() {}

bool ConfigManager::load() {
    bool ok = true;
    ok &= loadChannels();
    ok &= loadBehaviors();
    ok &= loadSystem();
    return ok;
}

bool ConfigManager::loadChannels() {
    std::string filepath = config_dir_ + \"/channels.yaml\";

    // Create default config if file does not exist
    if (!fs::exists(filepath)) {
        std::ofstream ofs(filepath);
        ofs << \"# Channel configuration example\\n\";
        ofs << \"channels:\\n\";
        ofs << \"  - id: CAM-001\\n\";
        ofs << \"    name: Default Channel\\n\";
        ofs << \"    rtsp_url: rtsp://admin:12345@192.168.1.100:554/stream1\\n\";
        ofs << \"    fps: 5\\n\";
        ofs << \"    enabled: true\\n\";
        ofs.close();
    }

    // Simplified parsing: read channel config from YAML file
    // TODO: Use yaml-cpp library for full parsing
    std::ifstream ifs(filepath);
    if (!ifs.is_open()) {
        std::cerr << \"[Config] Failed to open: \" << filepath << std::endl;
        return false;
    }

    std::string line;
    ChannelConfig current;
    bool in_channels = false;

    while (std::getline(ifs, line)) {
        // Skip comments and empty lines
        if (line.empty() || line[0] == '#') continue;

        if (line.find(\"channels:\") != std::string::npos) {
            in_channels = true;
            continue;
        }

        if (!in_channels) continue;

        if (line.find(\"  - id:\") != std::string::npos) {
            if (!current.channel_id.empty()) {
                channels_.push_back(current);
            }
            current = ChannelConfig();
            current.channel_id = line.substr(line.find(\"id:\") + 3);
            // Trim spaces
            while (!current.channel_id.empty() &&
                   (current.channel_id.back() == ' ' || current.channel_id.back() == '\\r' || current.channel_id.back() == '\\n')) {
                current.channel_id.pop_back();
            }
        } else if (line.find(\"name:\") != std::string::npos && !current.channel_id.empty()) {
            current.name = line.substr(line.find(\"name:\") + 5);
            while (!current.name.empty() && (current.name.back() == ' ' || current.name.back() == '\\r' || current.name.back() == '\\n')) {
                current.name.pop_back();
            }
        } else if (line.find(\"rtsp_url:\") != std::string::npos) {
            current.rtsp_url = line.substr(line.find(\"rtsp_url:\") + 9);
            while (!current.rtsp_url.empty() && (current.rtsp_url.back() == ' ' || current.rtsp_url.back() == '\\r' || current.rtsp_url.back() == '\\n')) {
                current.rtsp_url.pop_back();
            }
        } else if (line.find(\"fps:\") != std::string::npos) {
            std::string val = line.substr(line.find(\"fps:\") + 4);
            while (!val.empty() && (val.back() == ' ' || val.back() == '\\r' || val.back() == '\\n')) {
                val.pop_back();
            }
            current.fps = std::stoi(val);
        } else if (line.find(\"enabled:\") != std::string::npos) {
            std::string val = line.substr(line.find(\"enabled:\") + 8);
            while (!val.empty() && (val.back() == ' ' || val.back() == '\\r' || val.back() == '\\n')) {
                val.pop_back();
            }
            current.enabled = (val == \"true\" || val == \"1\");
        }
    }

    // Add the last channel
    if (!current.channel_id.empty()) {
        channels_.push_back(current);
    }

    std::cout << \"[Config] Loaded \" << channels_.size() << \" channels\" << std::endl;
    return true;
}

bool ConfigManager::loadBehaviors() {
    // Behavior rules loaded from YAML file
    // Using default rules here, should parse from file in production
    std::cout << \"[Config] Behavior rules loaded with defaults\" << std::endl;
    return true;
}

bool ConfigManager::loadSystem() {
    std::cout << \"[Config] System config loaded with defaults\" << std::endl;
    return true;
}

std::vector<ChannelConfig> ConfigManager::getChannelConfigs() const {
    return channels_;
}

void ConfigManager::addChannel(const ChannelConfig& config) {
    channels_.push_back(config);
    modified_ = true;
}

std::vector<BehaviorRule> ConfigManager::getBehaviorRules() const {
    return behavior_rules_;
}

ConfigManager::SystemConfig ConfigManager::getSystemConfig() const {
    return system_config_;
}

bool ConfigManager::save() {
    if (!modified_) return true;

    // Save channel configuration
    std::string filepath = config_dir_ + \"/channels.yaml\";
    std::ofstream ofs(filepath);
    if (!ofs.is_open()) {
        std::cerr << \"[Config] Failed to save: \" << filepath << std::endl;
        return false;
    }

    ofs << \"channels:\\n\";
    for (const auto& ch : channels_) {
        ofs << \"  - id: \" << ch.channel_id << \"\\n\";
        ofs << \"    name: \" << ch.name << \"\\n\";
        ofs << \"    rtsp_url: \" << ch.rtsp_url << \"\\n\";
        ofs << \"    fps: \" << ch.fps << \"\\n\";
        ofs << \"    enabled: \" << (ch.enabled ? \"true\" : \"false\") << \"\\n\";
    }

    ofs.close();
    modified_ = false;
    return true;
}

} // namespace edgevision
