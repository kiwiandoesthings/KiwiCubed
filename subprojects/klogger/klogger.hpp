#pragma once

#ifndef KLOGGER_HPP
#define KLOGGER_HPP

#include <string>
#include <iostream>
#include <source_location>
#include <map>

enum class LogLevel { 
	debug,
	info,
	warn,
	error,
	critical,
	off
};


std::string LogLevelToString(LogLevel level);

void OverrideFunctionLogName(const std::string& functionName, const std::string& replacement);

void Log(LogLevel level, const std::string& message, const bool debugMode, const std::source_location &sourceLocation);


#define DEBUG(message, debugMode) Log(LogLevel::debug, message, debugMode, std::source_location::current())
#define INFO(message) Log(LogLevel::info, message, false, std::source_location::current())
#define WARN(message) Log(LogLevel::warn, message, false, std::source_location::current())
#define ERR(message) Log(LogLevel::error, message, false, std::source_location::current())
#define CRITICAL(message) Log(LogLevel::critical, message, false, std::source_location::current())

#define LOG_CHECK(cond, succmsg, errmsg) if (cond) { INFO(succmsg); } else { ERR(errmsg); }
#define LOG_CHECK_BAD(cond, errmsg) if (cond) {} else { ERR(errmsg); }
#define LOG_CHECK_RETURN(cond, succmsg, errmsg, retcode) if (cond) { INFO(succmsg); } else { ERR(errmsg); return retcode; }
#define LOG_CHECK_RETURN_BAD(cond, errmsg, retcode) if (cond) {} else { ERR(errmsg); return retcode; }
#define LOG_CHECK_RETURN_VOID(cond, succmsg, errmsg) if (cond) { INFO(succmsg); } else { ERR(errmsg); return; }
#define LOG_CHECK_RETURN_VOID_BAD(cond, succmsg, errmsg) if (cond) {} else { ERR(errmsg); return; }

#define LOG_CHECK_CRITICAL(cond, succmsg, errmsg) if (cond) { INFO(succmsg); } else { CRITICAL(errmsg); }
#define LOG_CHECK_BAD_CRITICAL(cond, errmsg) if (cond) {} else { CRITICAL(errmsg); }
#define LOG_CHECK_RETURN_CRITICAL(cond, succmsg, errmsg, retcode) if (cond) { INFO(succmsg); } else { CRITICAL(errmsg); return retcode; }
#define LOG_CHECK_RETURN_BAD_CRITICAL(cond, errmsg, retcode) if (cond) {} else { CRITICAL(errmsg); return retcode; }
#define LOG_CHECK_RETURN_VOID_CRITICAL(cond, succmsg, errmsg) if (cond) { INFO(succmsg); } else { CRITICAL(errmsg); return; }
#define LOG_CHECK_RETURN_VOID_BAD_CRITICAL(cond, succmsg, errmsg) if (cond) {} else { CRITICAL(errmsg); return; }

#define OVERRIDE_LOG_NAME(replacement) OverrideFunctionLogName(std::source_location::current().function_name(), replacement)


#endif