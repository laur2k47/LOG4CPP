#pragma once

#include "Logger.hpp"
#include <string>
#include <memory>
#include <functional>

/**
 * FileRotatingHandler - Automatically rotates log files based on size with customizable formatting
 *
 * Features:
 * - Automatic file rotation when max size reached
 * - Configurable number of backup files to keep
 * - Customizable log formatting via formatter callbacks
 * - Thread-safe file operations
 * - Efficient: only rotates on threshold, not per-message
 * - C++14 compatible (no std::filesystem)
 *
 * Examples:
 *   // Default format (full with timestamp)
 *   FileRotatingHandler handler("app.log", 10*1024*1024);
 *
 *   // Custom format (message only)
 *   auto msgOnly = [](const LogEntry &e) { return e.message; };
 *   FileRotatingHandler handler("app.log", 10*1024*1024, 5, msgOnly);
 */
class FileRotatingHandler
{
public:
    // Formatter callback type: takes LogEntry, returns formatted string
    using Formatter = std::function<std::string(const LogEntry &)>;

    /**
     * Constructor with default formatter
     * @param path          Base log file path (e.g., "app.log")
     * @param maxSize       Max file size in bytes before rotation (e.g., 10*1024*1024 for 10MB)
     * @param backups       Number of backup files to keep (default: 5)
     */
    FileRotatingHandler(const std::string &path, size_t maxSize, int backups = 5);

    /**
     * Constructor with custom formatter
     * @param path          Base log file path
     * @param maxSize       Max file size in bytes before rotation
     * @param backups       Number of backup files to keep
     * @param fmt           Custom formatter function
     */
    FileRotatingHandler(const std::string &path, size_t maxSize, int backups, Formatter fmt);

    ~FileRotatingHandler();

private:
    // Pimpl: pointer to implementation
    class Impl;
    std::unique_ptr<Impl> impl;

    /**
     * Handler function - private, use registerFileRotatingHandler() instead
     */
    void write(const LogEntry &entry);

    // Allow convenience functions to access write()
    friend void registerFileRotatingHandler(const std::string &path, size_t maxSize, int maxBackups);
    friend void registerFileRotatingHandler(
        const std::string &path,
        size_t maxSize,
        int maxBackups,
        FileRotatingHandler::Formatter formatter);
};

// Convenience function for easy registration with default formatter
void registerFileRotatingHandler(const std::string &path, size_t maxSize, int maxBackups = 5);

// Convenience function for registration with custom formatter
void registerFileRotatingHandler(
    const std::string &path,
    size_t maxSize,
    int maxBackups,
    FileRotatingHandler::Formatter formatter);
