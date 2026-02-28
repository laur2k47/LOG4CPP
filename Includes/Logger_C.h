#ifndef LOGGER_C_H
#define LOGGER_C_H

#ifdef __cplusplus
extern "C"
{
#endif

    typedef enum
    {
        LOG_TRACE,
        LOG_DEBUG3,
        LOG_DEBUG2,
        LOG_DEBUG1,
        LOG_INFO,
        LOG_WARN,
        LOG_ERROR
    } CLogLevel;

    typedef struct
    {
        const char *timestamp;
        const char *level;
        const char *component;
        const char *function;
        int lineNumber;
        const char *message;
    } CLogEntry;

    // Opaque pointer to Logger instance
    typedef void *CLogger;

    // Initialize the logger with component name and log level
    void logger_initialize(const char *component_name, CLogLevel level);

    // Get the logger instance
    CLogger logger_get_instance(void);

    // Set the log level
    void logger_set_level(CLogger logger, CLogLevel level);

    // Get the current log level
    CLogLevel logger_get_level(CLogger logger);

    // Handler callback type for C code
    // Receives simplified log entry data as strings
    typedef void (*CLogHandler)(const char *timestamp, const char *level, const char *component,
                                const char *function, int line, const char *message);

    // Register a custom C-compatible handler
    void logger_register_handler(CLogHandler handler);

    // Logging functions - internal versions with function and line number
    void logger_trace_impl(CLogger logger, const char *function, int line, const char *format, ...);
    void logger_debug3_impl(CLogger logger, const char *function, int line, const char *format, ...);
    void logger_debug2_impl(CLogger logger, const char *function, int line, const char *format, ...);
    void logger_debug1_impl(CLogger logger, const char *function, int line, const char *format, ...);
    void logger_info_impl(CLogger logger, const char *function, int line, const char *format, ...);
    void logger_warn_impl(CLogger logger, const char *function, int line, const char *format, ...);
    void logger_error_impl(CLogger logger, const char *function, int line, const char *format, ...);

    // Convenience macros - automatically include function name and line number
    // (uses singleton instance, no need to pass logger)
#define LOG_TRACE(fmt, ...) logger_trace_impl(logger_get_instance(), __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__)
#define LOG_DEBUG3(fmt, ...) logger_debug3_impl(logger_get_instance(), __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__)
#define LOG_DEBUG2(fmt, ...) logger_debug2_impl(logger_get_instance(), __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__)
#define LOG_DEBUG1(fmt, ...) logger_debug1_impl(logger_get_instance(), __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__)
#define LOG_INFO(fmt, ...) logger_info_impl(logger_get_instance(), __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__)
#define LOG_WARN(fmt, ...) logger_warn_impl(logger_get_instance(), __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__)
#define LOG_ERROR(fmt, ...) logger_error_impl(logger_get_instance(), __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__)

#ifdef __cplusplus
}
#endif

#endif // LOGGER_C_H
