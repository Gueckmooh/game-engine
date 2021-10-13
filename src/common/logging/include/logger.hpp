#pragma once

#include <iostream>
#include <logging/logger_stream.hpp>
#include <set>
#include <string>

namespace logging {
/**
 * Logger with different messages levels.
 */
class Logger {
  public:
    Logger();

    /**
     * Log with the same type and category as the last one.
     */
    LoggerStream& log();

    /**
     * Log an output with information level.
     */
    LoggerStream& info(const std::string& category);

    /**
     * Log an output with warning level.
     */
    LoggerStream& warning(const std::string& category);

    /**
     * Log an output with highest severity level.
     */
    LoggerStream& error(const std::string& category);

    /**
     * Enables a category
     */
    void enable(const std::string& category);
    bool enabled(const std::string& category);

  private:
    LoggerStream m_stream;
    LoggerStream m_silentStream;
    std::set<std::string> m_enabledCategories;
    bool m_silent;
};

/**
 * Default logger.
 */
extern Logger logger;
}   // namespace logging
