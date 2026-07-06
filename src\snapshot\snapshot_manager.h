#ifndef EDGESVISION_SNAPSHOT_MANAGER_H
#define EDGESVISION_SNAPSHOT_MANAGER_H

#include <string>
#include <opencv2/opencv.hpp>
#include \"../types.h\"

namespace edgevision {

// Manages alert snapshot saving and annotation drawing
class SnapshotManager {
public:
    SnapshotManager(const std::string& save_dir = \"./data/alerts\");
    ~SnapshotManager();

    // Save an alert snapshot with annotations
    bool saveAlertSnapshot(const std::string& alert_id,
                          const Frame& image,
                          const std::vector<TrackedDetection>& detections,
                          const AlertEvent& alert);

    // Get snapshot file path
    std::string getSnapshotPath(const std::string& alert_id) const;

    // Clean old snapshots
    int cleanOldSnapshots(int days_to_keep = 7);

    // Get total saved count
    int getTotalSaved() const { return total_saved_; }

private:
    void drawAnnotations(cv::Mat& image,
                        const std::vector<TrackedDetection>& detections,
                        const AlertEvent& alert);

    std::string save_dir_;
    int total_saved_;
};

} // namespace edgevision

#endif // EDGESVISION_SNAPSHOT_MANAGER_H
