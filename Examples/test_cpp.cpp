#include "Logger.hpp"
#include <stdio.h>
#include <iostream>

// Custom handler that logs to stderr with simplified format
void customHandler(const LogEntry &entry)
{
    std::cerr << "[CUSTOM HANDLER] [" << entry.level << "] " << entry.message << std::endl;
}

int main()
{
    printf("=== C++ Application Test ===\n\n");

    // Initialize logger
    Logger::initialize("TestCPP", LogLevel::DEBUG1);

    // Register custom handler (in addition to default console handler)
    Logger::getInstance()->registerHandler(customHandler);

    printf("Testing C++ Logger:\n\n");

    LOG_CPP_TRACE("This is a TRACE message");
    LOG_CPP_DEBUG1("This is a DEBUG1 message");
    LOG_CPP_DEBUG2("This is a DEBUG2 message");
    LOG_CPP_INFO("This is an INFO message with value: ", 42);
    LOG_CPP_WARN("This is a WARN message");
    LOG_CPP_ERROR("This is an ERROR message");

    printf("\n=== Test Complete ===\n");

    return 0;
}
