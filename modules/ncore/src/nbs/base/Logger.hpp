#pragma once

#include <string>
#include <sstream>
#include <memory>
#include "globals.hpp"

namespace spdlog {
    class logger;
}

namespace nbs {

    enum LogLevel {
        LogWarn, LogError, LogDebug, LogInfo, LogTrace
    };

    class Logger
    {
    private:
        inline void addLog(const char* val) {
            ss << val;
        }
	    inline void addLog(std::string_view& val) {
            ss << val.data();
        }
        inline void addLog(const std::string& val) {
            ss << val.c_str();
        }
        inline void addLog(u64 val) {
            ss << val;
        }
        inline void addLog(u32 val) {
            ss << val;
        }

        inline void addLog(u16 val) {
            ss << val;
        }
        inline void addLog(u8 val) {
            ss << val;
        }

        inline void addLog(char val) {
            ss << val;
        }
        inline void addLog(short val) {
            ss << val;
        }
        inline void addLog(long val) {
            ss << val;
        }
        inline void addLog(int val) {
            ss << val;
        }

        inline void addLog(float val) {
            ss << val;
        }
        inline void addLog(double val) {
            ss << val;
        }

        void emitLog(LogLevel level);
        void setPattern(const char* pattern);
        void setLevel(const char* level);

        template <typename... T> 
        inline void log(LogLevel level, const T&... args) {
            int tmp[] = { (addLog(args), 0) ... };
            (void)tmp;
            emitLog(level);
        }

    public:
        Logger(const char* name = "nbs") {
            init(name);
        }

        void init(const char* name = "nbs");

        template <typename... T> 
        inline void info(const T&... args) 
        { log(LogLevel::LogInfo, args...); }

        template <typename... T> 
        inline void warn(const T&... args) 
        { log(LogLevel::LogWarn, args...); }

        template <typename... T> 
        inline void error(const T&... args) 
        { log(LogLevel::LogError, args...); }

        template <typename... T> 
        inline void debug(const T&... args) 
        { log(LogLevel::LogDebug, args...); }

        template <typename... T> 
        inline void trace(const T&... args) 
        { log(LogLevel::LogTrace, args...); }

    private:
        std::stringstream ss;
        std::shared_ptr<spdlog::logger> m_backend;
    };

    class LoggerManager 
    {
    public:
        inline static std::shared_ptr<Logger> getLogger() {
            return s_logger;
        }
        static void createLogger();

    private:
        static std::shared_ptr<Logger> s_logger;
    };
}

#define LogInfo(...)  ::nbs::LoggerManager::getLogger()->info(__VA_ARGS__)
#define LogWarn(...)  ::nbs::LoggerManager::getLogger()->warn(__VA_ARGS__)
#define LogError(...) ::nbs::LoggerManager::getLogger()->error(__VA_ARGS__)
#define LogDebug(...) ::nbs::LoggerManager::getLogger()->debug(__VA_ARGS__)
#define LogTrace(...) ::nbs::LoggerManager::getLogger()->trace(__VA_ARGS__)