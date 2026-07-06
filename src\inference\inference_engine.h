#ifndef EDGESVISION_INFERENCE_ENGINE_H
#define EDGESVISION_INFERENCE_ENGINE_H

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <mutex>
#include <chrono>
#include <opencv2/opencv.hpp>
#include \"../types.h\"

namespace edgevision {

// Inference engine supporting RKNN and ONNX backends
struct InferenceStats {
    int total_inferences = 0;
    double total_time_ms = 0.0;
    double avg_time_ms = 0.0;
    double max_time_ms = 0.0;
    double min_time_ms = 1e9;

    void update(double ms) {
        total_inferences++;
        total_time_ms += ms;
        avg_time_ms = total_time_ms / total_inferences;
        max_time_ms = std::max(max_time_ms, ms);
        min_time_ms = std::min(min_time_ms, ms);
    }

    void reset() {
        total_inferences = 0;
        total_time_ms = 0.0;
        avg_time_ms = 0.0;
        max_time_ms = 0.0;
        min_time_ms = 1e9;
    }
};

struct ModelMeta {
    std::string model_id;
    std::string name;
    std::string file_path;
    int input_width = 640;
    int input_height = 640;
    float conf_threshold = 0.45f;
    float iou_threshold = 0.45f;
    std::string classes;
    std::string quantization;
    bool active = false;
};

class InferenceEngine {
public:
    InferenceEngine();
    ~InferenceEngine();

    // Load model from file
    bool loadModel(const std::string& model_path, const ModelMeta& meta);

    // Unload current model
    bool unloadModel();

    // Run inference on a single frame
    std::vector<DetectionResult> infer(const cv::Mat& image);

    // Get inference statistics
    InferenceStats getStats() const { return stats_; }

    // Reset statistics
    void resetStats() { stats_.reset(); }

    // Non-NMS inference (for tracking compatibility)
    std::vector<DetectionResult> inferRaw(const cv::Mat& image);

    // Non-Maximum Suppression
    std::vector<DetectionResult> nms(const std::vector<DetectionResult>& detections);

    // Model cache
    struct ModelEntry {
        ModelMeta meta;
        void* session;  // ONNX Session or RKNN Context
        std::vector<float> input_buffer;
        std::vector<float> output_buffer;
    };

    std::map<std::string, ModelEntry> models_;
    std::string active_model_id_;
    InferenceStats stats_;
    std::mutex mutex_;

    void* onnx_session_ = nullptr;
};

} // namespace edgevision

#endif // EDGESVISION_INFERENCE_ENGINE_H
