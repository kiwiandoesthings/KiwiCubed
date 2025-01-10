#pragma once

#include <iostream>
#include <map>
#include <source_location>
#include <string>
#include <vector>

#ifdef PROFBUILD
#include <tracy/Tracy.hpp>
#endif

enum class LogLevel { info, debug, warn, error, critical, off };

std::string LogLevelToString(LogLevel level);

void OverrideFunctionLogName(const std::string &functionName, const std::string &replacement);

void Log(LogLevel level, const std::string &message, const std::source_location &sourceLocation);

#define DEBUG(message) Log(LogLevel::debug, message, std::source_location::current())
#define INFO(message) Log(LogLevel::info, message, std::source_location::current())
#define WARN(message) Log(LogLevel::warn, message, std::source_location::current())
#define ERR(message) Log(LogLevel::error, message, std::source_location::current())
#define CRITICAL(message) Log(LogLevel::critical, message, std::source_location::current())

#define LOG_CHECK(cond, succmsg, errmsg)                                                                                                   \
    if (cond) {                                                                                                                            \
        INFO(succmsg);                                                                                                                     \
    } else {                                                                                                                               \
        ERR(errmsg);                                                                                                                       \
    }
#define LOG_CHECK_RETURN(cond, succmsg, errmsg, retcode)                                                                                   \
    if (cond) {                                                                                                                            \
        INFO(succmsg);                                                                                                                     \
    } else {                                                                                                                               \
        ERR(errmsg);                                                                                                                       \
        return retcode;                                                                                                                    \
    }
#define LOG_CHECK_RETURN_VOID(cond, succmsg, errmsg)                                                                                       \
    if (cond) {                                                                                                                            \
        INFO(succmsg);                                                                                                                     \
    } else {                                                                                                                               \
        ERR(errmsg);                                                                                                                       \
        return;                                                                                                                            \
    }

#ifdef PROFBUILD
#define PROFILE_SCOPE(name) ZoneScopedN(name)
#define NEW_FRAME() FrameMark
#else
#define PROFILE_SCOPE(name)
#define NEW_FRAME()
#endif

#define OVERRIDE_LOG_NAME(replacement)                                                                                                     \
    PROFILE_SCOPE(#replacement);                                                                                                           \
    OverrideFunctionLogName(std::source_location::current().function_name(), replacement)
