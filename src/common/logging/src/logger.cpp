#include <algorithm>
#include <cmath>
#include <logging/logger.hpp>
#include <logging/logger_stream.hpp>

#include "string_utils.hpp"

logging::Logger logging::logger;

using namespace utils;

namespace {
std::string spacing(const std::string& string) {
    static const std::string space(" ");
    static uint32_t maxStringLength = 0u;
    uint32_t stringLength           = string.size();
    maxStringLength                 = std::max(maxStringLength, stringLength);
    return space * (maxStringLength - stringLength);
}
}   // namespace

namespace logging {
Logger::Logger()
    : m_stream(std::cout, "\e[0m"), m_silentStream(std::cout, "\e[0m"), m_silent(false) {
    m_silentStream.setstate(std::ios::badbit);
}

LoggerStream& Logger::log() {
    if (m_silent) return m_silentStream;
    return m_stream;
}
LoggerStream& Logger::info(const std::string& category) {
    if (!enabled(category)) {
        m_silent = true;
        return m_silentStream;
    }
    m_silent = false;
    m_stream.kind(LoggerKind::Info);
    m_stream.autoExit(false);
    m_stream.prefixString("\e[1m[" + category + "] \e[32m" + spacing(category));
    return m_stream;
}

LoggerStream& Logger::warning(const std::string& category) {
    static const std::string follow("/!\\ ");
    m_stream.kind(LoggerKind::Unknown);
    m_stream.autoExit(false);
    m_stream.prefixString("\e[1m[" + category + "] \e[33m" + follow
                          + spacing(category + follow));
    return m_stream;
}

LoggerStream& Logger::error(const std::string& category) {
    m_silent = false;
    static const std::string follow("//!\\\\ ");
    m_stream.kind(LoggerKind::Unknown);
    m_stream.autoExit(true);
    m_stream.prefixString("\e[1m[" + category + "] \e[31m" + follow
                          + spacing(category + follow));
    return m_stream;
}

void Logger::enable(const std::string& category) {
    // should be formed something.somethingElse
    m_enabledCategories.insert(category);
}

bool Logger::enabled(const std::string& category) {

    std::string cat = category;
    std::replace(cat.begin(), cat.end(), '.', ' ');

    std::vector<std::string> sv;
    sv.reserve(10);
    std::stringstream ss{ cat };
    std::string tmp;
    while (ss >> tmp) {
        sv.push_back(tmp);
        if (m_enabledCategories.find(utils::stringJoin(sv, "."))
            != m_enabledCategories.end()) {
            return true;
        }
    }
    return false;
}

}   // namespace logging
