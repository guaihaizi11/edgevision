#include \"preprocessor.h\"
#include <chrono>
#include <algorithm>
#include <iostream>

namespace edgevision {

Preprocessor::Preprocessor() {}

Preprocessor::Preprocessor(const PreprocessConfig& config)
    : config_(config) {}

void Preprocessor::setConfig(const PreprocessConfig& config) {
    config_ = config;
}

bool Preprocessor::preprocess(const Frame& input, cv::Mat& output) {
    auto start = std::chrono::high_resolution_clock::now();

    cv::Mat work = input.clone();

    // 1. ROI cropping
    if (!config_.rois.empty()) {
        cropROI(work, work);
    }

    // 2. Color space conversion
    colorConvert(work, work);

    // 3. Resize to model input dimensions
    resizeAndInterpolate(work, work);

    // 4. Normalize
    if (config_.normalize) {
        normalize(work, output);
    } else {
        output = work;
    }

    auto end = std::chrono::high_resolution_clock::now();
    double elapsed = std::chrono::duration<double, std::milli>(end - start).count();
    total_time_ += elapsed;
    total_processed_++;
    avg_time_ms_ = static_cast<float>(total_time_ / total_processed_);

    return true;
}

bool Preprocessor::preprocessBatch(const std::vector<FramePtr>& frames,
                                   std::vector<cv::Mat>& outputs) {
    outputs.resize(frames.size());
    for (size_t i = 0; i < frames.size(); i++) {
        if (!preprocess(*frames[i], outputs[i])) {
            return false;
        }
    }
    return true;
}

void Preprocessor::resizeAndInterpolate(const cv::Mat& src, cv::Mat& dst) {
    if (src.size() == cv::Size(config_.input_width, config_.input_height)) {
        dst = src;
        return;
    }
    cv::resize(src, dst, cv::Size(config_.input_width, config_.input_height),
               0, 0, cv::INTER_LINEAR);
}

void Preprocessor::colorConvert(const cv::Mat& src, cv::Mat& dst) {
    if (config_.color_space == \"BGR\") {
        dst = src;
    } else if (config_.color_space == \"RGB\") {
        cv::cvtColor(src, dst, cv::COLOR_BGR2RGB);
    } else if (config_.color_space == \"NV12\") {
        cv::cvtColor(src, dst, cv::COLOR_BGR2YUV_I420);
    }
}

void Preprocessor::normalize(const cv::Mat& src, cv::Mat& dst) {
    dst = src.clone();
    dst.convertTo(dst, CV_32FC3);

    // Normalize: (pixel - mean) / std
    cv::Scalar m(config_.mean[0], config_.mean[1], config_.mean[2]);
    cv::Scalar s(config_.std[0], config_.std[1], config_.std[2]);
    dst = (dst - m) / s;
}

void Preprocessor::cropROI(const cv::Mat& src, cv::Mat& dst) {
    if (config_.rois.empty()) {
        dst = src;
        return;
    }
    // Use first ROI
    const auto& roi = config_.rois[0];
    dst = src(roi);
}

} // namespace edgevision
