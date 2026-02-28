#include "../Includes/Logger.hpp"
#include "../Includes/FileRotatingHandler.hpp"
#include <iostream>
#include <iomanip>

int main()
{
    // Clean up old test logs
    system("rm -f test_msg_only.log* test_compact.log* test_full.log* test_custom.log* 2>/dev/null");

    Logger::initialize("RotationTest", LogLevel::DEBUG1);

    std::cout << "=== Log Rotation with Custom Formatters ===\n";
    std::cout << "Creating 4 rotating log files with different formats\n";
    std::cout << "Each logging 100 messages, rotating at 30KB\n\n";

    // Formatter 1: Message only
    auto messageOnly = [](const LogEntry &e)
    {
        return e.message;
    };
    registerFileRotatingHandler("test_msg_only.log", 30 * 1024, 2, messageOnly);

    // Formatter 2: Compact [LEVEL] message
    auto compact = [](const LogEntry &e)
    {
        return "[" + e.level + "] " + e.message;
    };
    registerFileRotatingHandler("test_compact.log", 30 * 1024, 2, compact);

    // Formatter 3: Default (full format with timestamp, level, component, etc.)
    registerFileRotatingHandler("test_full.log", 30 * 1024, 2);

    // Formatter 4: Custom - Extract HH:MM:SS from timestamp
    auto timeAndLevel = [](const LogEntry &e)
    {
        // Extract HH:MM:SS from "YYYY-MM-DD HH:MM:SS.microseconds"
        std::string time = e.timestamp.substr(11, 8);
        return "[" + time + "] [" + e.level + "] " + e.message;
    };
    registerFileRotatingHandler("test_custom.log", 30 * 1024, 2, timeAndLevel);

    std::cout << "4 Rotating handlers with different formatters:\n";
    std::cout << "  1. Message only\n";
    std::cout << "  2. Compact: [LEVEL] message\n";
    std::cout << "  3. Full: [timestamp][level][component][function:line] message\n";
    std::cout << "  4. Custom: [HH:MM:SS] [LEVEL] message\n\n";

    // Log messages
    for (int i = 1; i <= 100; ++i)
    {
        LOG_CPP_INFO(
            "Message ", i, " - ",
            "This is a test message with some padding. "
            "Different formatters will show different levels of detail.");

        if (i % 25 == 0)
        {
            std::cout << "Progress: " << i << "/100 messages logged\n";
        }
    }

    std::cout << "\n=== Log Files Created ===\n";
    system("echo '--- test_msg_only.log (first 5 lines) ---' && head -5 test_msg_only.log && echo '...'");
    std::cout << "\n";
    system("echo '--- test_compact.log (first 5 lines) ---' && head -5 test_compact.log && echo '...'");
    std::cout << "\n";
    system("echo '--- test_full.log (first 5 lines) ---' && head -5 test_full.log && echo '...'");
    std::cout << "\n";
    system("echo '--- test_custom.log (first 5 lines) ---' && head -5 test_custom.log && echo '...'");

    std::cout << "\n=== File Sizes (after rotation) ===\n";
    system("ls -lh test_*.log* 2>/dev/null | awk '{print $9 \" (\" $5 \")\"}' | sort");

    std::cout << "\n✓ Test complete! All 4 files created with different formatting:\n";
    std::cout << "  - test_msg_only.log → message only (minimal format)\n";
    std::cout << "  - test_compact.log → [LEVEL] message\n";
    std::cout << "  - test_full.log → full format (timestamp, level, component, etc.)\n";
    std::cout << "  - test_custom.log → custom format with time and level\n";

    return 0;
}
