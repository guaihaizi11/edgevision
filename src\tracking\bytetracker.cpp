#include \"bytetracker.h\"
#include <algorithm>
#include <iostream>

namespace edgevision {

ByteTracker::ByteTracker(float score_threshold, float obj_threshold,
                        float tracking_threshold, int max_age,
                        int min_hits, float iou_threshold)
    : score_threshold_(score_threshold),
      obj_threshold_(obj_threshold),
      tracking_threshold_(tracking_threshold),
      max_age_(max_age),
      min_hits_(min_hits),
      iou_threshold_(iou_threshold),
      next_track_id_(1) {}

std::vector<TrackedDetection> ByteTracker::update(
    const std::vector<DetectionResult>& detections) {
    std::lock_guard<std::mutex> lock(mutex_);

    std::vector<TrackedDetection> result;

    // Filter low-confidence detections
    std::vector<DetectionResult> kept_dets;
    for (const auto& det : detections) {
        if (det.confidence >= score_threshold_) {
            kept_dets.push_back(det);
        }
    }

    // Update existing trackers
    for (auto& track : tracks_) {
        track.time_since_update++;
        track.hit_streak--;

        if (track.hit_streak <= 0) {
            track.lost = true;
        }
    }

    // Remove expired trackers
    tracks_.erase(
        std::remove_if(tracks_.begin(), tracks_.end(),
            [this](const Track& t) {
                return t.age > max_age_ || t.lost;
            }),
        tracks_.end());

    // Match each detection to a tracker (simplified IoU matching)
    std::vector<bool> det_matched(kept_dets.size(), false);
    std::vector<bool> track_matched(tracks_.size(), false);

    for (size_t i = 0; i < kept_dets.size(); i++) {
        for (size_t j = 0; j < tracks_.size(); j++) {
            if (det_matched[i] || track_matched[j]) continue;

            float iou = kept_dets[i].IoU({
                .x1 = tracks_[j].history.back().x1,
                .y1 = tracks_[j].history.back().y1,
                .x2 = tracks_[j].history.back().x2,
                .y2 = tracks_[j].history.back().y2,
            });

            if (iou > iou_threshold_) {
                // Match successful
                auto& track = tracks_[j];
                track.time_since_update = 0;
                track.hits++;
                track.hit_streak = min_hits_;
                track.lost = false;
                track.history.push_back(kept_dets[i]);
                det_matched[i] = true;
                track_matched[j] = true;

                // Create tracked detection result
                TrackedDetection td;
                td = kept_dets[i];
                td.track_id = track.track_id;
                td.age = track.age;
                td.hits = track.hits;
                td.lost = false;
                result.push_back(td);
                break;
            }
        }
    }

    // Create new trackers for unmatched detections
    for (size_t i = 0; i < kept_dets.size(); i++) {
        if (!det_matched[i]) {
            Track new_track;
            new_track.track_id = next_track_id_++;
            new_track.age = 1;
            new_track.hits = 1;
            new_track.hit_streak = min_hits_;
            new_track.history.push_back(kept_dets[i]);

            TrackedDetection td;
            td = kept_dets[i];
            td.track_id = new_track.track_id;
            td.age = 1;
            td.hits = 1;
            td.lost = false;
            result.push_back(td);

            tracks_.push_back(new_track);
        }
    }

    return result;
}

int ByteTracker::getActiveCount() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return static_cast<int>(tracks_.size());
}

void ByteTracker::reset() {
    std::lock_guard<std::mutex> lock(mutex_);
    tracks_.clear();
    next_track_id_ = 1;
}

} // namespace edgevision
