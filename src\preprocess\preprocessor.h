#ifndef EDGESVISION_PREPROCESSOR_H
#define EDGESVISION_PREPROCESSOR_H

#include <string>
#include <vector>
#include <memory>
#include <opencv2/opencv.hpp>
#include \"../types.h\"

namespace edgevision {

struct PreprocessConfig {
    int input_width = 640;
    int input_height = 640;
    std::string color_space = \"BGR\";  // BGR, RGB, NV12
    float mean[3] = {0.0f, 0.0f, 0.0f};
    float std[3] = {1.0f, 1.0f, 1.0f};
    bool normalize = true;
    std::vector<cv::Rect> rois;

    PreprocessConfig() {
        mean[0] = mean[1] = mean[2] = 0.0f;
        std[0] = std[1] = std[2] = 1.0f;
    }
};

class Preprocessor {
public:
    Preprocessor();
    explicit Preprocessor(const PreprocessConfig& config);

    // Set configuration
    void setConfig(const PreprocessConfig& config);
    const PreprocessConfig& getConfig() const { return config_; }

    // Preprocess image
    // Input: raw frame, Output: model input data
    bool preprocess(const Frame& input, cv::Mat& output);

    // Batch preprocessing
    bool preprocessBatch(const std::vector<FramePtr>& frames,
                        std::vector<cv::Mat>& outputs);

    // Performance statistics
    float getAvgTimeMs() const { return avg_time_ms_; }
    int getTotalProcessed() const { return total_processed_; }

private:
    void resizeAndInterpolate(const cv::Mat& src, cv::Mat& dst);
    void colorConvert(const cv::Mat& src, cv::Mat& dst);
    void normalize(const cv::Mat& src, cv::Mat& dst);
    void cropROI(const cv::Mat& src, cv::Mat& dst);

    PreprocessConfig config_;
    float avg_time_ms_ = 0.0f;
    int total_processed_ = 0;
    double total_time_ = 0.0;
};

} // namespace edgevision

#endif // EDGESVISION_PREPROCESSOR_H
