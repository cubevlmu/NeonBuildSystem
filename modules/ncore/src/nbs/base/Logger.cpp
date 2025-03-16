#include "Logger.hpp"
#include "nbs/utils/StringUtils.hpp"
#include "spdlog/common.h"

#include <memory>
#include <sstream>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>

#define FORMAT "%^%v%$"

namespace nbs {
    std::shared_ptr<Logger> LoggerManager::s_logger;

    int _loader = []() {
        LoggerManager::createLogger();
        return 0;
    }();

    void Logger::init(const char* name) {
        FastConcat(fileName, name, ".log");

        std::vector<spdlog::sink_ptr> logSinks;
		logSinks.emplace_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
		logSinks.emplace_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>((const char*)fileName, true));

		logSinks[0]->set_pattern(FORMAT);
		logSinks[1]->set_pattern(FORMAT);

		m_backend = std::make_shared<spdlog::logger>(name, begin(logSinks), end(logSinks));
		spdlog::register_logger(m_backend);
		m_backend->set_level(spdlog::level::debug);
		// m_backend->flush_on(spdlog::level::debug);
    }


    void Logger::emitLog(LogLevel level) {
        auto stdstr = ss.str();
        const char* str = stdstr.c_str();
        
        ss.str("");
        ss.clear();

        switch (level)
        {
        case LogWarn:
            m_backend->warn(str); break;
        case LogError:
            m_backend->error(str); break;
        case LogDebug:
            m_backend->debug(str); break;
        case LogInfo:
            m_backend->info(str); break;
        case LogTrace:
            m_backend->trace(str); break;
        }
    }


    void Logger::setPattern(const char* pattern)
    {
        if (strlen(pattern) == 0) {
            m_backend->set_pattern(FORMAT);
            return;
        }
        m_backend->set_pattern(pattern);
    }

    
    void Logger::setLevel(const char* level)
    {
        if (strlen(level) == 0) {
            m_backend->set_level(spdlog::level::trace);
            return;
        }
        m_backend->set_level(spdlog::level::from_str(level));
    }

    
    void LoggerManager::createLogger() {
        if (s_logger.get() != nullptr) {
            return;
        }
        s_logger = std::make_shared<Logger>();
    }

} // namespace cation::debug