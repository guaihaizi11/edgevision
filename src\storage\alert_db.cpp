#include \"alert_db.h\"
#include <iostream>
#include <sstream>
#include <chrono>
#include <ctime>

namespace edgevision {

AlertDB::AlertDB(const std::string& db_path) : db_path_(db_path) {}

AlertDB::~AlertDB() {
    if (db_) {
        sqlite3_close(db_);
    }
}

bool AlertDB::initialize() {
    // Open or create database
    int rc = sqlite3_open(db_path_.c_str(), &db_);
    if (rc != SQLITE_OK) {
        std::cerr << \"[DB] Cannot open database: \" << sqlite3_errmsg(db_) << std::endl;
        return false;
    }

    // Create tables
    const char* create_tables[] = {
        // Alert events table
        \"CREATE TABLE IF NOT EXISTS alert_events (\"
        \"  alert_id TEXT PRIMARY KEY,\"
        \"  timestamp TEXT NOT NULL,\"
        \"  channel_id TEXT NOT NULL,\"
        \"  channel_name TEXT,\"
        \"  behavior_type TEXT NOT NULL,\"
        \"  behavior_label TEXT,\"
        \"  alert_level TEXT DEFAULT 'warning',\"
        \"  confidence REAL,\"
        \"  bbox_x1 REAL, bbox_y1 REAL, bbox_x2 REAL, bbox_y2 REAL,\"
        \"  tracking_id INTEGER,\"
        \"  snapshot_path TEXT,\"
        \"  video_clip_path TEXT,\"
        \"  device_ip TEXT,\"
        \"  status TEXT DEFAULT 'pending',\"
        \"  created_at INTEGER,\"
        \"  confirmed_at INTEGER\"
        \");\",

        // Indexes
        \"CREATE INDEX IF NOT EXISTS idx_alert_channel ON alert_events(channel_id, created_at DESC);\",
        \"CREATE INDEX IF NOT EXISTS idx_alert_behavior ON alert_events(behavior_type, created_at DESC);\",
        \"CREATE INDEX IF NOT EXISTS idx_alert_status ON alert_events(status, created_at DESC);\",

        // Channel configuration table
        \"CREATE TABLE IF NOT EXISTS channels (\"
        \"  channel_id TEXT PRIMARY KEY,\"
        \"  name TEXT NOT NULL,\"
        \"  rtsp_url TEXT NOT NULL,\"
        \"  username TEXT,\"
        \"  password TEXT,\"
        \"  width INTEGER DEFAULT 0,\"
        \"  height INTEGER DEFAULT 0,\"
        \"  fps INTEGER DEFAULT 5,\"
        \"  enabled INTEGER DEFAULT 1,\"
        \"  rois TEXT,\"
        \"  created_at INTEGER,\"
        \"  updated_at INTEGER\"
        \");\",

        // Model table
        \"CREATE TABLE IF NOT EXISTS models (\"
        \"  model_id TEXT PRIMARY KEY,\"
        \"  name TEXT NOT NULL,\"
        \"  file_path TEXT NOT NULL,\"
        \"  input_width INTEGER DEFAULT 640,\"
        \"  input_height INTEGER DEFAULT 640,\"
        \"  conf_threshold REAL DEFAULT 0.45,\"
        \"  iou_threshold REAL DEFAULT 0.45,\"
        \"  classes TEXT,\"
        \"  quantization TEXT,\"
        \"  active INTEGER DEFAULT 0,\"
        \"  created_at INTEGER\"
        \");\",

        // Behavior rules table
        \"CREATE TABLE IF NOT EXISTS behavior_rules (\"
        \"  rule_id TEXT PRIMARY KEY,\"
        \"  name TEXT NOT NULL,\"
        \"  conditions TEXT,\"
        \"  action TEXT,\"
        \"  enabled INTEGER DEFAULT 1,\"
        \"  priority INTEGER DEFAULT 5,\"
        \"  created_at INTEGER\"
        \");\"
    };

    char* errmsg = nullptr;
    for (const auto& sql : create_tables) {
        rc = sqlite3_exec(db_, sql, nullptr, nullptr, &errmsg);
        if (rc != SQLITE_OK) {
            std::cerr << \"[DB] SQL error: \" << errmsg << std::endl;
            if (errmsg) sqlite3_free(errmsg);
            // Continue with other statements
        }
    }

    std::cout << \"[DB] Database initialized: \" << db_path_ << std::endl;
    return true;
}

bool AlertDB::insertAlert(const AlertEvent& alert) {
    if (!db_) return false;

    std::ostringstream ss;
    ss << \"INSERT OR REPLACE INTO alert_events \"
       << \"(alert_id, timestamp, channel_id, channel_name, behavior_type, \"
       \"behavior_label, alert_level, confidence, tracking_id,\"
       \"snapshot_path, status, created_at) \"
       << \"VALUES ('\"
       << escapeString(alert.alert_id) << \"', '\"
       << escapeString(alert.timestamp) << \"', '\"
       << escapeString(alert.channel_id) << \"', '\"
       << escapeString(alert.channel_name) << \"', '\"
       << escapeString(alert.behavior_type) << \"', '\"
       << escapeString(alert.behavior_label) << \"', '\"
       << escapeString(alert.alert_level) << \"', \"
       << alert.confidence << \", \"
       << alert.tracking_id << \", '\"
       << escapeString(alert.snapshot_path) << \"', '\"
       << escapeString(alert.status) << \"', \"
       << alert.created_at << \");\";

    char* errmsg = nullptr;
    int rc = sqlite3_exec(db_, ss.str().c_str(), nullptr, nullptr, &errmsg);
    if (rc != SQLITE_OK) {
        std::cerr << \"[DB] Insert error: \" << errmsg << std::endl;
        if (errmsg) sqlite3_free(errmsg);
        return false;
    }

    return true;
}

std::vector<AlertEvent> AlertDB::queryAlerts(const std::string& channel_id,
                                             const std::string& behavior_type,
                                             const std::string& status,
                                             int page, int page_size) {
    std::vector<AlertEvent> results;

    if (!db_) return results;

    std::ostringstream ss;
    ss << \"SELECT alert_id, timestamp, channel_id, channel_name, \"
       \"behavior_type, behavior_label, alert_level, confidence, \"
       \"tracking_id, snapshot_path, status, created_at \"
       \"FROM alert_events WHERE 1=1\";

    std::vector<std::string> params;
    if (!channel_id.empty()) {
        ss << \" AND channel_id = '\" << escapeString(channel_id) << \"'\";
    }
    if (!behavior_type.empty()) {
        ss << \" AND behavior_type = '\" << escapeString(behavior_type) << \"'\";
    }
    if (!status.empty()) {
        ss << \" AND status = '\" << escapeString(status) << \"'\";
    }

    int offset = (page - 1) * page_size;
    ss << \" ORDER BY created_at DESC LIMIT \" << page_size << \" OFFSET \" << offset;

    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db_, ss.str().c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            AlertEvent alert;
            const unsigned char* col;

            col = sqlite3_column_text(stmt, 0);
            if (col) alert.alert_id = reinterpret_cast<const char*>(col);
            col = sqlite3_column_text(stmt, 1);
            if (col) alert.timestamp = reinterpret_cast<const char*>(col);
            col = sqlite3_column_text(stmt, 2);
            if (col) alert.channel_id = reinterpret_cast<const char*>(col);
            col = sqlite3_column_text(stmt, 3);
            if (col) alert.channel_name = reinterpret_cast<const char*>(col);
            col = sqlite3_column_text(stmt, 4);
            if (col) alert.behavior_type = reinterpret_cast<const char*>(col);
            col = sqlite3_column_text(stmt, 5);
            if (col) alert.behavior_label = reinterpret_cast<const char*>(col);
            col = sqlite3_column_text(stmt, 6);
            if (col) alert.alert_level = reinterpret_cast<const char*>(col);
            alert.confidence = sqlite3_column_double(stmt, 7);
            alert.tracking_id = sqlite3_column_int(stmt, 8);
            col = sqlite3_column_text(stmt, 9);
            if (col) alert.snapshot_path = reinterpret_cast<const char*>(col);
            col = sqlite3_column_text(stmt, 10);
            if (col) alert.status = reinterpret_cast<const char*>(col);
            alert.created_at = sqlite3_column_int64(stmt, 11);

            results.push_back(alert);
        }
        sqlite3_finalize(stmt);
    }

    return results;
}

bool AlertDB::updateAlertStatus(const std::string& alert_id, const std::string& status) {
    if (!db_) return false;

    std::ostringstream ss;
    ss << \"UPDATE alert_events SET status = '\" << escapeString(status)
       << \"', confirmed_at = \" << std::chrono::duration_cast<std::chrono::seconds>(
           std::chrono::system_clock::now().time_since_epoch()).count()
       << \" WHERE alert_id = '\" << escapeString(alert_id) << \"'\";

    char* errmsg = nullptr;
    int rc = sqlite3_exec(db_, ss.str().c_str(), nullptr, nullptr, &errmsg);
    if (rc != SQLITE_OK) {
        std::cerr << \"[DB] Update error: \" << errmsg << std::endl;
        if (errmsg) sqlite3_free(errmsg);
        return false;
    }

    return true;
}

AlertDB::AlertStats AlertDB::getStats() {
    AlertStats stats;
    if (!db_) return stats;

    sqlite3_stmt* stmt;

    // Total count
    if (sqlite3_prepare_v2(db_, \"SELECT COUNT(*) FROM alert_events\", -1, &stmt, nullptr) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            stats.total = sqlite3_column_int(stmt, 0);
        }
        sqlite3_finalize(stmt);
    }

    // Pending count
    if (sqlite3_prepare_v2(db_, \"SELECT COUNT(*) FROM alert_events WHERE status='pending'\", -1, &stmt, nullptr) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            stats.pending = sqlite3_column_int(stmt, 0);
        }
        sqlite3_finalize(stmt);
    }

    // Confirmed count
    if (sqlite3_prepare_v2(db_, \"SELECT COUNT(*) FROM alert_events WHERE status='confirmed'\", -1, &stmt, nullptr) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            stats.confirmed = sqlite3_column_int(stmt, 0);
        }
        sqlite3_finalize(stmt);
    }

    // Ignored count
    if (sqlite3_prepare_v2(db_, \"SELECT COUNT(*) FROM alert_events WHERE status='ignored'\", -1, &stmt, nullptr) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            stats.ignored = sqlite3_column_int(stmt, 0);
        }
        sqlite3_finalize(stmt);
    }

    return stats;
}

std::string AlertDB::escapeString(const std::string& str) {
    if (!db_) return str;
    char* escaped = sqlite3_mprintf(\"'%w'\", str.c_str());
    std::string result(escaped + 1); // Remove leading quote
    result.pop_back(); // Remove trailing quote
    sqlite3_free(escaped);
    return result;
}

} // namespace edgevision
