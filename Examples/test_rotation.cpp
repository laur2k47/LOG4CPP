#include "../Includes/Logger.hpp"
#include "../Includes/FileRotatingHandler.hpp"
#include <iostream>
#include <iomanip>

int main()
{
    // Clean up old test logs
    system("rm -f test_rotation.log* 2>/dev/null");

    Logger::initialize("RotationTest", LogLevel::DEBUG1);

    // Register rotating file handler
    // Max 50KB per file, keep 3 backup files
    static FileRotatingHandler rotatingHandler("test_rotation.log", 50 * 1024, 3);
    Logger::getInstance()->registerHandler(
        std::bind(&FileRotatingHandler::write, &rotatingHandler, std::placeholders::_1));

    std::cout << "=== Log Rotation Test ===\n";
    std::cout << "Logging 200 messages (~60KB total) to trigger rotation\n";
    std::cout << "Max file size: 50KB | Max backups: 3\n\n";

    // Log enough messages to trigger multiple rotations
    for (int i = 1; i <= 200; ++i)
    {
        LOG_CPP_INFO(
            "Message ", i, " - ",
            "This is a test message with some padding to increase file size. "
            "Lorem ipsum dolor sit amet consectetur adipiscing elit sed do eiusmod "
            "tempor incididunt ut labore et dolore magna aliqua");

        // Show progress
        if (i % 25 == 0)
        {
            std::cout << "Progress: " << i << "/200 messages logged\n";
            std::cout << "Current file size: " << rotatingHandler.getCurrentSize() << " bytes\n";

            // Show file listing
            std::cout << "Files present:\n";
            system("ls -lh test_rotation.log* 2>/dev/null | awk '{print \"  \" $9 \" (\" $5 \")\"}' || echo '  (none yet)'");
            std::cout << "\n";
        }
    }

    std::cout << "\n=== Rotation Complete ===\n";
    std::cout << "Final file listing:\n";
    system("ls -lh test_rotation.log* 2>/dev/null | awk '{print \"  \" $9 \" (\" $5 \")\"}' || echo '  (none)'");

    std::cout << "\nFile details:\n";
    system("wc -l test_rotation.log* 2>/dev/null | tail -1 | awk '{printf \"  Total lines: %d\\n\", $1}'");
    system("du -ch test_rotation.log* 2>/dev/null | tail -1 | awk '{print \"  Total size: \" $1}'");

    return 0;
}
