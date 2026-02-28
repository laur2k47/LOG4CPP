#include "Logger.hpp"

// Static instance definition
Logger *Logger::instance = nullptr;

// Static mutex definition
std::mutex Logger::instanceMutex;
