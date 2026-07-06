#ifndef EDGESVISION_ALERT_DB_H
#define EDGESVISION_ALERT_DB_H

#include <string>
#include <vector>
#include <sqlite3.h>
#include \"../types.h\"

namespace edgevision {

// SQLite-based alert and data persistence
class AlertDB {
public:
    struct AlertStats {
        int total = 0;
        int pending = 0;
        int confirmed = 0;
        int ignored = 0;
    };

    explicit AlertDB(const std::string& db_path = \"./data/edgevision.db\");
    ~AlertDB();

    // Initialize database schema
    bool initialize();

    // Insert an alert event
    bool insertAlert(const AlertEvent& alert);

    // Query alerts with filters
    std::vector<AlertEvent> queryAlerts(const std::string& channel_id,
                                        const std::string& behavior_type,
                                        const std::string& status,
                                        int page, int page_size);

    // Update alert status
    bool updateAlertStatus(const std::string& alert_id, const std::string& status);

    // Get alert statistics
    AlertStats getStats();

private:
    std::string escapeString(const std::string& str);

    std::string db_path_;
    sqlite3* db_ = nullptr;
};

} // namespace edgevision

#endif // EDGESVISION_ALERT_DB_H
