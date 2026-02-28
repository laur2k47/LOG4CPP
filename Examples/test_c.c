#include <Logger_C.h>
#include <stdio.h>

// Custom C handler that logs only level and message
void my_custom_handler(const char *timestamp, const char *level, const char *component,
                       const char *function, int line, const char *message)
{
    fprintf(stderr, "[C_HANDLER] [%s] %s\n", level, message);
}

int main()
{
    printf("=== C Application Test ===\n\n");

    // Initialize logger
    logger_initialize("TestC", LOG_DEBUG1);

    // Register custom handler
    logger_register_handler(my_custom_handler);

    printf("Testing C Logger:\n\n");

    LOG_TRACE("This is a TRACE message");
    LOG_DEBUG1("This is a DEBUG1 message");
    LOG_DEBUG2("This is a DEBUG2 message");
    LOG_INFO("This is an INFO message with value: %d", 42);
    LOG_WARN("This is a WARN message");
    LOG_ERROR("This is an ERROR message");

    printf("\n=== Test Complete ===\n");

    return 0;
}
