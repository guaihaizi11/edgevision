#ifndef EDGESVISION_ANALYZER_PIPELINE_H
#define EDGESVISION_ANALYZER_PIPELINE_H

#include <string>
#include <vector>
#include <memory>
#include <thread>
#include <atomic>
#include <mutex>
#include <map>
#include \"../types.h\"
#include \"../video/rtsp_source.h\"
#include \"../preprocess/preprocessor.h\"
#include \"../inference/inference_engine.h\"
#include \"../tracking/bytetracker.h\"
#include \"../behavior/behavior_engine.h\"
#include \"../alert/alert_manager.h\"
#include \"../config/config_manager.h\"

namespace edgevision {

// Core analysis pipeline orchestrating video ingestion, inference, tracking, and behavior analysis
class AnalyzerPipeline {
public:
    AnalyzerPipeline();
    ~AnalyzerPipeline();

    bool initialize(const std::string& config_dir = \"./config\");
    bool startAll();
    void stopAll();
    bool addChannel(const ChannelConfig& config);
    bool removeChannel(const std::string& channel_id);
    std::map<std::string, ChannelStatus> getChannelStatuses() const;
    SystemStats getSystemStats();
    AlertDB::AlertStats getAlertStats();

private:
    void channelProcessLoop(const std::string& channel_id);
    void processFrame(const std::string& channel_id, const FramePtr& frame);

    ConfigManager config_mgr_;
    std::unique_ptr<Preprocessor> preprocessor_;
    std::unique_ptr<InferenceEngine> inference_engine_;
    std::unique_ptr<ByteTracker> tracker_;
    std::unique_ptr<BehaviorEngine> behavior_engine_;
    std::unique_ptr<AlertManager> alert_mgr_;
    std::map<std::string, std::unique_ptr<RtspSource>> channels_;
    std::map<std::string, std::thread> channel_threads_;
    std::mutex channels_mutex_;
    std::atomic<bool> running_{false};
};

} // namespace edgevision

#endif // EDGESVISION_ANALYZER_PIPELINE_H
