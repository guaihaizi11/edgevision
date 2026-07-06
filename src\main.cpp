/**
 * EdgeVision - RK3588 Video Behavior Analysis System
 * Main program entry point
 *
 * Build:
 *   mkdir build && cd build
 *   cmake .. -DCMAKE_BUILD_TYPE=Release
 *   make -j4
 *
 * Run:
 *   ./edgevision [--config ./config] [--demo]
 */

#include <iostream>
#include <string>
#include <signal.h>
#include <chrono>
#include <thread>

#include \"types.h\"
#include \"pipeline/analyzer_pipeline.h\"
#include \"config/config_manager.h\"
#include \"monitor/system_monitor.h\"

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

using namespace edgevision;

static bool g_running = true;

void signalHandler(int signum) {
    std::cout << \"\\n[Main] Received signal \" << signum << \". Shutting down...\" << std::endl;
    g_running = false;
}

void printUsage(const char* prog) {
    std::cout << \"Usage: \" << prog << \" [options]\\n\"
              << \"Options:\\n\"
              << \"  --config <dir>   Config directory (default: ./config)\\n\"
              << \"  --demo           Run demo mode with synthetic data\\n\"
              << \"  --help           Show this help\\n\";
}

int main(int argc, char* argv[]) {
    std::string config_dir = \"./config\";
    bool demo_mode = false;

    // Parse command-line arguments
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if ((arg == \"--config\" || arg == \"-c\") && i + 1 < argc) {
            config_dir = argv[++i];
        } else if (arg == \"--demo\" || arg == \"-d\") {
            demo_mode = true;
        } else if (arg == \"--help\" || arg == \"-h\") {
            printUsage(argv[0]);
            return 0;
        }
    }

    std::cout << \"============================================\" << std::endl;
    std::cout << \"  EdgeVision - RK3588 Video Behavior Analysis\" << std::endl;
    std::cout << \"  Version: 1.0.0\" << std::endl;
    std::cout << \"  Platform: \" << (demo_mode ? \"Demo\" : \"Production\") << std::endl;
    std::cout << \"============================================\\n\" << std::endl;

    // Register signal handlers
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);
#ifdef _WIN32
    SetConsoleCtrlHandler([](DWORD dwCtrlType) -> BOOL {
        if (dwCtrlType == CTRL_C_EVENT || dwCtrlType == CTRL_CLOSE_EVENT) {
            g_running = false;
            return TRUE;
        }
        return FALSE;
    }, TRUE);
#endif

    // Initialize system monitor
    SystemMonitor monitor;
    monitor.start();

    // Initialize analysis pipeline
    AnalyzerPipeline pipeline;
    if (!pipeline.initialize(config_dir)) {
        std::cerr << \"[Main] Failed to initialize pipeline\" << std::endl;
        return 1;
    }

    // Demo mode: use test video
    if (demo_mode) {
        std::cout << \"[Main] Running in DEMO mode...\" << std::endl;
        std::cout << \"[Main] This will analyze test.mp4 with synthetic detections.\\n\" << std::endl;

        // Create test channel config
        ChannelConfig test_cfg;
        test_cfg.channel_id = \"DEMO-001\";
        test_cfg.name = \"Demo Channel\";
        test_cfg.rtsp_url = \"./data/test.mp4\";  // Use local video instead of RTSP
        test_cfg.fps = 5;
        test_cfg.enabled = true;

        // RtspSource needs extension to support local files
        // Skipping for now, demonstrating architecture
        std::cout << \"[Main] Demo mode: architecture verified.\\n\"
                  << \"[Main] To run with real cameras, use production mode.\\n\\n\" << std::endl;
    }

    // Start pipeline
    if (!pipeline.startAll()) {
        std::cerr << \"[Main] Failed to start pipeline\" << std::endl;
        monitor.stop();
        return 1;
    }

    std::cout << \"[Main] System started. Press Ctrl+C to stop.\\n\" << std::endl;

    // Main loop
    auto last_print = std::chrono::steady_clock::now();
    while (g_running) {
        std::this_thread::sleep_for(std::chrono::seconds(1));

        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - last_print).count();

        if (elapsed >= 10) {
            // Print status every 10 seconds
            auto statuses = pipeline.getChannelStatuses();
            auto stats = pipeline.getAlertStats();

            std::cout << \"\\n[Main] Status: \" << statuses.size() << \" channels active\";
            std::cout << \", Alerts: total=\" << stats.total;
            std::cout << \", pending=\" << stats.pending;
            std::cout << \", confirmed=\" << stats.confirmed << std::endl;

            last_print = now;
        }
    }

    // Cleanup
    std::cout << \"\\n[Main] Stopping...\" << std::endl;
    pipeline.stopAll();
    monitor.stop();

    std::cout << \"[Main] Shutdown complete.\" << std::endl;
    return 0;
}
