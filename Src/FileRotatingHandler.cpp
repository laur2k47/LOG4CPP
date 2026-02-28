#include "FileRotatingHandler.hpp"
#include <sstream>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <cstdio>
#include <sys/stat.h>
#include <vector>
#include <mutex>

// ========== FileRotatingHandler::Impl Definition ==========

class FileRotatingHandler::Impl
{
public:
    std::string basePath;
    size_t maxFileSize;
    int maxBackups;
    std::ofstream currentFile;
    size_t currentSize;
    FileRotatingHandler::Formatter formatter;
    mutable std::mutex fileMutex;

    Impl(const std::string &path, size_t maxSize, int backups, FileRotatingHandler::Formatter fmt)
        : basePath(path), maxFileSize(maxSize), maxBackups(backups), currentSize(0), formatter(fmt)
    {
        if (!formatter)
        {
            formatter = defaultFormatter;
        }
        openFile();
    }

    static std::string defaultFormatter(const LogEntry &entry)
    {
        std::ostringstream oss;
        oss << "[" << entry.timestamp << "]"
            << "[" << std::left << std::setw(6) << entry.level << "]"
            << "[" << entry.component << "]"
            << "[" << entry.function << ":" << entry.lineNumber << "] "
            << entry.message;
        return oss.str();
    }

    static size_t getFileSize(const std::string &path)
    {
        struct stat statbuf;
        if (stat(path.c_str(), &statbuf) != 0)
        {
            return 0;
        }
        return statbuf.st_size;
    }

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

    void write(const LogEntry &entry)
    {
        std::lock_guard<std::mutex> lock(fileMutex);

        // Format the log line using custom formatter
        std::string logLine = formatter(entry) + "\n";
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
            currentFile.flush();
            currentSize += logSize;
        }
    }

    void setFormatter(FileRotatingHandler::Formatter fmt)
    {
        std::lock_guard<std::mutex> lock(fileMutex);
        if (fmt)
        {
            formatter = fmt;
        }
    }

    std::string getCurrentPath() const
    {
        return basePath;
    }

    size_t getCurrentSize() const
    {
        std::lock_guard<std::mutex> lock(fileMutex);
        return currentSize;
    }
};

// ========== FileRotatingHandler Implementation ==========

FileRotatingHandler::FileRotatingHandler(const std::string &path, size_t maxSize, int backups)
    : impl(std::make_unique<Impl>(path, maxSize, backups, nullptr))
{
}

FileRotatingHandler::FileRotatingHandler(const std::string &path, size_t maxSize, int backups, Formatter fmt)
    : impl(std::make_unique<Impl>(path, maxSize, backups, fmt))
{
}

FileRotatingHandler::~FileRotatingHandler() = default;

// ========== Private Methods ==========

void FileRotatingHandler::write(const LogEntry &entry)
{
    impl->write(entry);
}

// ========== Convenience Functions ==========

void registerFileRotatingHandler(const std::string &path, size_t maxSize, int maxBackups)
{
    static FileRotatingHandler handler(path, maxSize, maxBackups);
    Logger::getInstance()->registerHandler(
        std::bind(&FileRotatingHandler::write, &handler, std::placeholders::_1));
}

void registerFileRotatingHandler(
    const std::string &path,
    size_t maxSize,
    int maxBackups,
    FileRotatingHandler::Formatter formatter)
{
    static FileRotatingHandler handler(path, maxSize, maxBackups, formatter);
    Logger::getInstance()->registerHandler(
        std::bind(&FileRotatingHandler::write, &handler, std::placeholders::_1));
}
