#include \"inference_engine.h\"
#include <iostream>
#include <random>
#include <algorithm>

namespace edgevision {

InferenceEngine::InferenceEngine() {}

InferenceEngine::~InferenceEngine() {
    if (onnx_session_) {
        // Clean up ONNX session
        // In production: OrtSession release
        onnx_session_ = nullptr;
    }
}

bool InferenceEngine::loadModel(const std::string& model_path, const ModelMeta& meta) {
    std::lock_guard<std::mutex> lock(mutex_);

    std::cout << \"[Inference] Loading model: \" << model_path
              << \" (\" << meta.name << \")\" << std::endl;

    ModelEntry entry;
    entry.meta = meta;
    entry.session = nullptr;

    // TODO: Implement actual model loading
    // RKNN: rknn_init(&ctx, model_path)
    // ONNX: OrtSessionOptions + OrtEnv

    models_[meta.model_id] = entry;
    active_model_id_ = meta.model_id;

    return true;
}

bool InferenceEngine::unloadModel() {
    std::lock_guard<std::mutex> lock(mutex_);

    if (active_model_id_.empty()) return false;

    std::cout << \"[Inference] Unloading model: \" << active_model_id_ << std::endl;
    models_.erase(active_model_id_);
    active_model_id_.clear();
    return true;
}

std::vector<DetectionResult> InferenceEngine::infer(const cv::Mat& image) {
    auto start = std::chrono::high_resolution_clock::now();

    std::vector<DetectionResult> raw_detections;

    // TODO: Implement actual inference
    // 1. Preprocess image (resize, normalize)
    // 2. Run model inference
    // 3. Post-process (decode boxes, apply NMS)

    // Demo: return empty detections
    // In production: call RKNN/ONNX inference

    auto end = std::chrono::high_resolution_clock::now();
    double elapsed = std::chrono::duration<double, std::milli>(end - start).count();
    stats_.update(elapsed);

    return nms(raw_detections);
}

std::vector<DetectionResult> InferenceEngine::inferRaw(const cv::Mat& image) {
    auto start = std::chrono::high_resolution_clock::now();

    std::vector<DetectionResult> detections;

    // TODO: Same as infer but without NMS

    auto end = std::chrono::high_resolution_clock::now();
    double elapsed = std::chrono::duration<double, std::milli>(end - start).count();
    stats_.update(elapsed);

    return detections;
}

std::vector<DetectionResult> InferenceEngine::nms(const std::vector<DetectionResult>& detections) {
    std::vector<DetectionResult> result;
    std::vector<bool> suppressed(detections.size(), false);

    for (size_t i = 0; i < detections.size(); i++) {
        if (suppressed[i]) continue;
        result.push_back(detections[i]);

        for (size_t j = i + 1; j < detections.size(); j++) {
            if (suppressed[j]) continue;
            if (detections[i].class_id == detections[j].class_id) {
                float iou = detections[i].IoU(detections[j]);
                if (iou > detections[i].iou_threshold) {
                    suppressed[j] = true;
                }
            }
        }
    }

    return result;
}

} // namespace edgevision
