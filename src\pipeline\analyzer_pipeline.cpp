#include \"analyzer_pipeline.h\"
#include <iostream>
#include <chrono>

namespace edgevision {

AnalyzerPipeline::AnalyzerPipeline() {}

AnalyzerPipeline::~AnalyzerPipeline() {
    stopAll();
}

bool AnalyzerPipeline::initialize(const std::string& config_dir) {
    std::cout << \"[Pipeline] Initializing with config: \" << config_dir << std::endl;

    // Load configuration
    if (!config_mgr_.load()) {
        std::cerr << \"[Pipeline] Failed to load configuration\" << std::endl;
        return false;
    }

    // Initialize components
    preprocessor_ = std::make_unique<Preprocessor>();
    inference_engine_ = std::make_unique<InferenceEngine>();
    tracker_ = std::make_unique<ByteTracker>();
    behavior_engine_ = std::make_unique<BehaviorEngine>();
    alert_mgr_ = std::make_unique<AlertManager>();

    // Load behavior rules
    behavior_engine_->loadRules(config_dir + \"/behaviors.yaml\");

    // Initialize database
    alert_mgr_->getStats(); // Triggers DB init

    std::cout << \"[Pipeline] Initialization complete\" << std::endl;
    return true;
}

bool AnalyzerPipeline::startAll() {
    running_ = true;
    auto configs = config_mgr_.getChannelConfigs();

    for (const auto& config : configs) {
        if (!config.enabled) continue;
        addChannel(config);
    }

    std::cout << \"[Pipeline] Started \" << channels_.size() << \" channels\" << std::endl;
    return true;
}

void AnalyzerPipeline::stopAll() {
    running_ = false;

    for (auto& pair : channels_) {
        pair.second->stop();
    }
    channels_.clear();
    channel_threads_.clear();

    std::cout << \"[Pipeline] All channels stopped\" << std::endl;
}

bool AnalyzerPipeline::addChannel(const ChannelConfig& config) {
    std::lock_guard<std::mutex> lock(channels_mutex_);

    if (channels_.count(config.channel_id)) {
        std::cerr << \"[Pipeline] Channel already exists: \" << config.channel_id << std::endl;
        return false;
    }

    auto source = std::make_unique<RtspSource>();
    if (!source->start(config.rtsp_url, config.fps)) {
        std::cerr << \"[Pipeline] Failed to start channel: \" << config.channel_id << std::endl;
        return false;
    }

    channels_[config.channel_id] = std::move(source);

    // Start processing thread
    channel_threads_[config.channel_id] = std::thread(
        &AnalyzerPipeline::channelProcessLoop, this, config.channel_id);

    std::cout << \"[Pipeline] Added channel: \" << config.channel_id << std::endl;
    return true;
}

bool AnalyzerPipeline::removeChannel(const std::string& channel_id) {
    std::lock_guard<std::mutex> lock(channels_mutex_);

    auto it = channels_.find(channel_id);
    if (it == channels_.end()) return false;

    it->second->stop();
    channels_.erase(it);

    auto thread_it = channel_threads_.find(channel_id);
    if (thread_it != channel_threads_.end()) {
        if (thread_it->second.joinable()) {
            thread_it->second.join();
        }
        channel_threads_.erase(thread_it);
    }

    std::cout << \"[Pipeline] Removed channel: \" << channel_id << std::endl;
    return true;
}

std::map<std::string, ChannelStatus> AnalyzerPipeline::getChannelStatuses() const {
    std::map<std::string, ChannelStatus> statuses;
    for (const auto& pair : channels_) {
        statuses[pair.first] = pair.second->getStatus();
    }
    return statuses;
}

SystemStats AnalyzerPipeline::getSystemStats() {
    // TODO: Implement system monitoring integration
    return SystemStats();
}

AlertDB::AlertStats AnalyzerPipeline::getAlertStats() {
    return alert_mgr_->getStats();
}

void AnalyzerPipeline::channelProcessLoop(const std::string& channel_id) {
    std::cout << \"[Pipeline] Process loop started for: \" << channel_id << std::endl;

    while (running_) {
        auto it = channels_.find(channel_id);
        if (it == channels_.end()) break;

        FramePtr frame;
        if (!it->second->getFrame(frame, 100)) {
            continue;
        }

        processFrame(channel_id, frame);
    }

    std::cout << \"[Pipeline] Process loop stopped for: \" << channel_id << std::endl;
}

void AnalyzerPipeline::processFrame(const std::string& channel_id, const FramePtr& frame) {
    if (!frame || frame->empty()) return;

    auto start = std::chrono::high_resolution_clock::now();

    // 1. Preprocessing
    cv::Mat processed;
    if (preprocessor_) {
        preprocessor_->preprocess(*frame, processed);
    } else {
        processed = *frame;
    }

    // 2. Inference detection
    std::vector<DetectionResult> detections;
    if (inference_engine_) {
        detections = inference_engine_->infer(processed);
    }

    // 3. Object tracking
    std::vector<TrackedDetection> tracked_dets;
    if (tracker_) {
        tracked_dets = tracker_->update(detections);
    }

    // 4. Behavior analysis
    if (behavior_engine_) {
        auto alerts = behavior_engine_->analyze(channel_id, channel_id, tracked_dets);

        // 5. Handle alerts
        for (const auto& alert : alerts) {
            if (alert_mgr_) {
                alert_mgr_->handleAlert(alert, *frame, tracked_dets);
            }
        }
    }

    auto end = std::chrono::high_resolution_clock::now();
    double elapsed = std::chrono::duration<double, std::milli>(end - start).count();

    // Optional: print processing time
    // std::cout << \"[Pipeline] Frame processed in \" << elapsed << \"ms\" << std::endl;
}

} // namespace edgevision
