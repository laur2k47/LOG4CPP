#include "Logger_C.h"
#include "Logger.hpp"
#include <cstdarg>
#include <cstdio>
#include <cstring>

// Convert C log level to C++ log level
static LogLevel convertLogLevel(CLogLevel level)
{
    switch (level)
    {
    case LOG_TRACE:
        return LogLevel::TRACE;
    case LOG_DEBUG3:
        return LogLevel::DEBUG3;
    case LOG_DEBUG2:
        return LogLevel::DEBUG2;
    case LOG_DEBUG1:
        return LogLevel::DEBUG1;
    case LOG_INFO:
        return LogLevel::INFO;
    case LOG_WARN:
        return LogLevel::WARN;
    case LOG_ERROR:
        return LogLevel::ERROR;
    default:
        return LogLevel::INFO;
    }
}

// Convert C++ log level to C log level
static CLogLevel convertLogLevelToCpp(LogLevel level)
{
    switch (level)
    {
    case LogLevel::TRACE:
        return LOG_TRACE;
    case LogLevel::DEBUG3:
        return LOG_DEBUG3;
    case LogLevel::DEBUG2:
        return LOG_DEBUG2;
    case LogLevel::DEBUG1:
        return LOG_DEBUG1;
    case LogLevel::INFO:
        return LOG_INFO;
    case LogLevel::WARN:
        return LOG_WARN;
    case LogLevel::ERROR:
        return LOG_ERROR;
    default:
        return LOG_INFO;
    }
}

// Static storage for C handler callback
static CLogHandler g_c_handler = nullptr;

// Format message from variadic arguments (similar to printf)
static std::string formatMessage(const char *format, va_list args)
{
    char buffer[4096];
    vsnprintf(buffer, sizeof(buffer), format, args);
    return std::string(buffer);
}

// Internal logging functions with function and line number
void logger_trace_impl(CLogger logger, const char *function, int line, const char *format, ...)
{
    if (!logger || !format)
        return;

    Logger *log = static_cast<Logger *>(logger);
    va_list args;
    va_start(args, format);
    std::string message = formatMessage(format, args);
    va_end(args);
    log->trace(function, line, message);
}

void logger_debug3_impl(CLogger logger, const char *function, int line, const char *format, ...)
{
    if (!logger || !format)
        return;

    Logger *log = static_cast<Logger *>(logger);
    va_list args;
    va_start(args, format);
    std::string message = formatMessage(format, args);
    va_end(args);
    log->debug3(function, line, message);
}

void logger_debug2_impl(CLogger logger, const char *function, int line, const char *format, ...)
{
    if (!logger || !format)
        return;

    Logger *log = static_cast<Logger *>(logger);
    va_list args;
    va_start(args, format);
    std::string message = formatMessage(format, args);
    va_end(args);
    log->debug2(function, line, message);
}

void logger_debug1_impl(CLogger logger, const char *function, int line, const char *format, ...)
{
    if (!logger || !format)
        return;

    Logger *log = static_cast<Logger *>(logger);
    va_list args;
    va_start(args, format);
    std::string message = formatMessage(format, args);
    va_end(args);
    log->debug1(function, line, message);
}

void logger_info_impl(CLogger logger, const char *function, int line, const char *format, ...)
{
    if (!logger || !format)
        return;

    Logger *log = static_cast<Logger *>(logger);
    va_list args;
    va_start(args, format);
    std::string message = formatMessage(format, args);
    va_end(args);
    log->info(function, line, message);
}

void logger_warn_impl(CLogger logger, const char *function, int line, const char *format, ...)
{
    if (!logger || !format)
        return;

    Logger *log = static_cast<Logger *>(logger);
    va_list args;
    va_start(args, format);
    std::string message = formatMessage(format, args);
    va_end(args);
    log->warn(function, line, message);
}

void logger_error_impl(CLogger logger, const char *function, int line, const char *format, ...)
{
    if (!logger || !format)
        return;

    Logger *log = static_cast<Logger *>(logger);
    va_list args;
    va_start(args, format);
    std::string message = formatMessage(format, args);
    va_end(args);
    log->error(function, line, message);
}

// Legacy functions (kept for backward compatibility)

// Initialize the logger
void logger_initialize(const char *component_name, CLogLevel level)
{
    Logger::initialize(component_name, convertLogLevel(level));
}

// Get the logger instance
CLogger logger_get_instance(void)
{
    return static_cast<CLogger>(Logger::getInstance());
}

// Set the log level
void logger_set_level(CLogger logger, CLogLevel level)
{
    if (logger)
    {
        static_cast<Logger *>(logger)->setLogLevel(convertLogLevel(level));
    }
}

// Get the current log level
CLogLevel logger_get_level(CLogger logger)
{
    if (logger)
    {
        return convertLogLevelToCpp(static_cast<Logger *>(logger)->getLogLevel());
    }
    return LOG_INFO;
}

// Register a custom C-compatible handler
void logger_register_handler(CLogHandler handler)
{
    if (!handler)
        return;

    // Store the C handler
    g_c_handler = handler;

    // Register a C++ wrapper handler that calls the C handler
    Logger::getInstance()->registerHandler([](const LogEntry &entry)
                                           {
        if (g_c_handler)
        {
            g_c_handler(entry.timestamp.c_str(), entry.level.c_str(), entry.component.c_str(),
                       entry.function.c_str(), entry.lineNumber, entry.message.c_str());
        } });
}

// Logging functions
void logger_trace(CLogger logger, const char *format, ...)
{
    if (!logger || !format)
        return;

    Logger *log = static_cast<Logger *>(logger);
    va_list args;
    va_start(args, format);
    std::string message = formatMessage(format, args);
    va_end(args);
    log->trace(__FUNCTION__, 0, message);
}

void logger_debug3(CLogger logger, const char *format, ...)
{
    if (!logger || !format)
        return;

    Logger *log = static_cast<Logger *>(logger);
    va_list args;
    va_start(args, format);
    std::string message = formatMessage(format, args);
    va_end(args);
    log->debug3(__FUNCTION__, 0, message);
}

void logger_debug2(CLogger logger, const char *format, ...)
{
    if (!logger || !format)
        return;

    Logger *log = static_cast<Logger *>(logger);
    va_list args;
    va_start(args, format);
    std::string message = formatMessage(format, args);
    va_end(args);
    log->debug2(__FUNCTION__, 0, message);
}

void logger_debug1(CLogger logger, const char *format, ...)
{
    if (!logger || !format)
        return;

    Logger *log = static_cast<Logger *>(logger);
    va_list args;
    va_start(args, format);
    std::string message = formatMessage(format, args);
    va_end(args);
    log->debug1(__FUNCTION__, 0, message);
}

void logger_info(CLogger logger, const char *format, ...)
{
    if (!logger || !format)
        return;

    Logger *log = static_cast<Logger *>(logger);
    va_list args;
    va_start(args, format);
    std::string message = formatMessage(format, args);
    va_end(args);
    log->info(__FUNCTION__, 0, message);
}

void logger_warn(CLogger logger, const char *format, ...)
{
    if (!logger || !format)
        return;

    Logger *log = static_cast<Logger *>(logger);
    va_list args;
    va_start(args, format);
    std::string message = formatMessage(format, args);
    va_end(args);
    log->warn(__FUNCTION__, 0, message);
}

void logger_error(CLogger logger, const char *format, ...)
{
    if (!logger || !format)
        return;

    Logger *log = static_cast<Logger *>(logger);
    va_list args;
    va_start(args, format);
    std::string message = formatMessage(format, args);
    va_end(args);
    log->error(__FUNCTION__, 0, message);
}
