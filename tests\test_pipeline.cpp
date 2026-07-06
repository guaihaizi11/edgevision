/**
 * Pipeline Test Program
 * Verify module collaboration
 */

#include <iostream>
#include <opencv2/opencv.hpp>
#include \"../src/types.h\"
#include \"../src/preprocess/preprocessor.h\"
#include \"../src/inference/inference_engine.h\"
#include \"../src/tracking/bytetracker.h\"
#include \"../src/behavior/behavior_engine.h\"
#include \"../src/config/config_manager.h\"

using namespace edgevision;

void test_preprocessor() {
    std::cout << \"\\n=== Test: Preprocessor ===\" << std::endl;

    PreprocessConfig config;
    config.input_width = 640;
    config.input_height = 640;
    config.color_space = \"BGR\";
    config.normalize = true;

    Preprocessor proc(config);

    // Create test image
    cv::Mat test_img(1080, 1920, CV_8UC3, cv::Scalar(128, 128, 128));

    cv::Mat output;
    bool ok = proc.preprocess(test_img, output);

    std::cout << \"  Preprocess: \" << (ok ? \"PASS\" : \"FAIL\") << std::endl;
    std::cout << \"  Output size: \" << output.cols << \"x\" << output.rows << std::endl;
    std::cout << \"  Avg time: \" << proc.getAvgTimeMs() << \"ms\" << std::endl;
}

void test_tracker() {
    std::cout << \"\\n=== Test: ByteTracker ===\" << std::endl;

    ByteTracker tracker(0.5f, 0.35f, 0.25f, 10, 3, 0.3f);

    // Mock detection results
    std::vector<DetectionResult> dets = {
        {0, \"person\", 0.92f, 100, 200, 300, 400},
        {1, \"cell_phone\", 0.85f, 150, 250, 180, 280},
    };

    auto tracked = tracker.update(dets);
    std::cout << \"  Detections: \" << dets.size() << std::endl;
    std::cout << \"  Tracked: \" << tracked.size() << std::endl;

    for (const auto& td : tracked) {
        std::cout << \"    TrackID=\" << td.track_id
                  << \" Class=\" << td.class_name
                  << \" Conf=\" << td.confidence
                  << std::endl;
    }
}

void test_behavior_engine() {
    std::cout << \"\\n=== Test: BehaviorEngine ===\" << std::endl;

    BehaviorEngine engine;
    engine.loadRules(\"./config/behaviors.yaml\");

    auto rules = engine.getRules();
    std::cout << \"  Loaded rules: \" << rules.size() << std::endl;

    for (const auto& rule : rules) {
        std::cout << \"    - \" << rule.name
                  << \" (Priority: \" << rule.priority << \")\"
                  << std::endl;
    }
}

void test_config_manager() {
    std::cout << \"\\n=== Test: ConfigManager ===\" << std::endl;

    ConfigManager config(\"./config\");
    config.load();

    auto channels = config.getChannelConfigs();
    std::cout << \"  Channels loaded: \" << channels.size() << std::endl;

    for (const auto& ch : channels) {
        std::cout << \"    - \" << ch.name
                  << \" (\" << ch.channel_id << \")\"
                  << \" Enabled: \" << (ch.enabled ? \"yes\" : \"no\")
                  << std::endl;
    }
}

int main() {
    std::cout << \"============================================\" << std::endl;
    std::cout << \"  EdgeVision Unit Tests\" << std::endl;
    std::cout << \"============================================\" << std::endl;

    try {
        test_preprocessor();
        test_tracker();
        test_behavior_engine();
        test_config_manager();

        std::cout << \"\\n============================================\" << std::endl;
        std::cout << \"  All tests completed!\" << std::endl;
        std::cout << \"============================================\" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << \"\\nTest FAILED: \" << e.what() << std::endl;
        return 1;
    }

    return 0;
}