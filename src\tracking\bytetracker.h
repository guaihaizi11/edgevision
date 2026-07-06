#ifndef EDGESVISION_BYTETRACKER_H
#define EDGESVISION_BYTETRACKER_H

#include <vector>
#include <map>
#include <memory>
#include <mutex>
#include <opencv2/opencv.hpp>
#include \"../types.h\"

namespace edgevision {

// ByteTrack-style multi-object tracker
class ByteTracker {
public:
    ByteTracker(float score_threshold = 0.5f,
               float obj_threshold = 0.35f,
               float tracking_threshold = 0.25f,
               int max_age = 10,
               int min_hits = 3,
               float iou_threshold = 0.3f);

    // Track detection results for a single frame
    std::vector<TrackedDetection> update(const std::vector<DetectionResult>& detections);

    // Get number of active trackers
    int getActiveCount() const;

    // Reset all trackers
    void reset();

private:
    struct Track {
        int track_id;
        std::vector<DetectionResult> history;
        int age = 1;
        int time_since_update = 0;
        int hits = 0;
        int hit_streak = 0;
        bool lost = false;
        cv::Point2f velocity;

        Track() : track_id(-1) {}
    };

    std::vector<Track> tracks_;
    int next_track_id_;

    float score_threshold_;
    float obj_threshold_;
    float tracking_threshold_;
    int max_age_;
    int min_hits_;
    float iou_threshold_;

    std::mutex mutex_;
};

} // namespace edgevision

#endif // EDGESVISION_BYTETRACKER_H
