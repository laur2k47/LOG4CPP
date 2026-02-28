#include "../Includes/Logger.hpp"
#include <iostream>
#include <fstream>

// Handler 1: File output
void fileHandler(const LogEntry &entry)
{
    std::ofstream file("/tmp/log_output.txt", std::ios::app);
    file << "[FILE] " << entry.level << " - " << entry.message << "\n";
    file.close();
}

// Handler 2: Stderr output
void stderrHandler(const LogEntry &entry)
{
    std::cerr << "[STDERR] " << entry.level << ": " << entry.message << "\n";
}

// Handler 3: JSON-like format
void jsonHandler(const LogEntry &entry)
{
    std::cerr << "{\"level\":\"" << entry.level << "\",\"msg\":\"" << entry.message << "\"}\n";
}

int main()
{
    // Clear previous test log
    if (system("rm -f /tmp/log_output.txt") != 0)
    {
        std::cerr << "Warning: failed to clear log file\n";
    }

    Logger::initialize("MultiHandler", LogLevel::DEBUG1);
    Logger *logger = Logger::getInstance();

    // Register multiple handlers (plus default console handler already registered)
    logger->registerHandler(fileHandler);
    logger->registerHandler(stderrHandler);
    logger->registerHandler(jsonHandler);

    std::cout << "=== Logging with 4 handlers (console + file + stderr + json) ===\n";
    LOG_CPP_DEBUG1("Test message 1");
    LOG_CPP_INFO("Test message 2");
    LOG_CPP_WARN("Test message 3");

    std::cout << "\n=== File output (from handler #1) ===\n";
    if (system("cat /tmp/log_output.txt") != 0)
    {
        std::cerr << "Warning: failed to read log file\n";
    }

    return 0;
}
