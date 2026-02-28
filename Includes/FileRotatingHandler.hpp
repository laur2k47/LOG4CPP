#pragma once

#include "Logger.hpp"
#include <fstream>
#include <string>
#include <mutex>
#include <cstdio>
#include <sys/stat.h>

/**
 * FileRotatingHandler - Automatically rotates log files based on size
 *
 * Features:
 * - Automatic file rotation when max size reached
 * - Configurable number of backup files to keep
 * - Thread-safe file operations
 * - Efficient: only rotates on threshold, not per-message
 * - C++14 compatible (no std::filesystem)
 *
 * Example:
 *   FileRotatingHandler handler("app.log", 10*1024*1024);  // 10MB max
 *   Logger::getInstance()->registerHandler(handler);
 */
class FileRotatingHandler
{
private:
    std::string basePath;         // Base log file path: "app.log"
    size_t maxFileSize;           // Max size before rotation (bytes)
    int maxBackups;               // Number of backup files to keep (e.g., app.log.1, .2, .3)
    std::ofstream currentFile;    // Opened file handle
    size_t currentSize;           // Current file size in bytes
    mutable std::mutex fileMutex; // Thread-safe file access

public:
    /**
     * Constructor
     * @param path          Base log file path (e.g., "app.log")
     * @param maxSize       Max file size in bytes before rotation (e.g., 10*1024*1024 for 10MB)
     * @param backups       Number of backup files to keep (default: 5)
     */
    FileRotatingHandler(const std::string &path, size_t maxSize, int backups = 5)
        : basePath(path), maxFileSize(maxSize), maxBackups(backups), currentSize(0)
    {
        openFile();
    }

    ~FileRotatingHandler()
    {
        std::lock_guard<std::mutex> lock(fileMutex);
        if (currentFile.is_open())
        {
            currentFile.close();
        }
    }

    /**
     * Handler function (compatible with Logger::registerHandler)
     * Call as: Logger::getInstance()->registerHandler(
     *     std::bind(&FileRotatingHandler::write, &handler, std::placeholders::_1)
     * );
     */
    void write(const LogEntry &entry)
    {
        std::lock_guard<std::mutex> lock(fileMutex);

        // Format the complete log line
        std::ostringstream oss;
        oss << "[" << entry.timestamp << "]"
            << "[" << std::left << std::setw(6) << entry.level << "]"
            << "[" << entry.component << "]"
            << "[" << entry.function << ":" << entry.lineNumber << "] "
            << entry.message;

        std::string logLine = oss.str() + "\n";
        size_t logSize = logLine.length();

        // Check if rotation needed
        if (currentSize + logSize > maxFileSize)
        {
            rotate();
        }

        // Write to file
        if (currentFile.is_open())
        {
            currentFile << logLine;
            currentFile.flush(); // Flush after each write for durability
            currentSize += logSize;
        }
    }

    /**
     * Get current log file path
     */
    std::string getCurrentPath() const
    {
        return basePath;
    }

    /**
     * Get current file size in bytes
     */
    size_t getCurrentSize() const
    {
        std::lock_guard<std::mutex> lock(fileMutex);
        return currentSize;
    }

private:
    /**
     * Get file size using stat (C++14 compatible)
     */
    static size_t getFileSize(const std::string &path)
    {
        struct stat statbuf;
        if (stat(path.c_str(), &statbuf) != 0)
        {
            return 0;
        }
        return statbuf.st_size;
    }

    /**
     * Check if file exists using stat (C++14 compatible)
     */
    static bool fileExists(const std::string &path)
    {
        struct stat statbuf;
        return stat(path.c_str(), &statbuf) == 0;
    }

    void openFile()
    {
        std::lock_guard<std::mutex> lock(fileMutex);
        if (currentFile.is_open())
        {
            currentFile.close();
        }

        // Check if file already exists to get its size
        currentSize = 0;
        if (fileExists(basePath))
        {
            currentSize = getFileSize(basePath);
        }

        currentFile.open(basePath, std::ios::app);
    }

    void rotate()
    {
        if (currentFile.is_open())
        {
            currentFile.close();
        }

        try
        {
            // Delete oldest backup if we exceed maxBackups
            std::string oldestPath = basePath + "." + std::to_string(maxBackups);
            if (fileExists(oldestPath))
            {
                std::remove(oldestPath.c_str());
            }

            // Shift files down: app.log.3 -> app.log.4, app.log.2 -> app.log.3, etc.
            for (int i = maxBackups - 1; i >= 1; --i)
            {
                std::string oldPath = basePath + "." + std::to_string(i);
                std::string newPath = basePath + "." + std::to_string(i + 1);
                if (fileExists(oldPath))
                {
                    std::rename(oldPath.c_str(), newPath.c_str());
                }
            }

            // Rename current to app.log.1
            std::string backupPath = basePath + ".1";
            if (fileExists(basePath))
            {
                std::rename(basePath.c_str(), backupPath.c_str());
            }
        }
        catch (const std::exception &e)
        {
            std::cerr << "Error rotating log files: " << e.what() << "\n";
        }

        // Open new file
        currentSize = 0;
        currentFile.open(basePath, std::ios::app);
    }
};

// Convenience function for easy registration
inline void registerFileRotatingHandler(const std::string &path, size_t maxSize, int maxBackups = 5)
{
    static FileRotatingHandler handler(path, maxSize, maxBackups);
    Logger::getInstance()->registerHandler(
        std::bind(&FileRotatingHandler::write, &handler, std::placeholders::_1));
}
