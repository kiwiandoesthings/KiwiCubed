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

void Log(LogLevel level, const std::string& message, const std::source_location &sourceLocation);


#define DEBUG(message) Log(LogLevel::debug, message, std::source_location::current())
#define INFO(message) Log(LogLevel::info, message, std::source_location::current())
#define WARN(message) Log(LogLevel::warn, message, std::source_location::current())
#define ERR(message) Log(LogLevel::error, message, std::source_location::current())
#define CRITICAL(message) Log(LogLevel::critical, message, std::source_location::current())

#define LOG_CHECK(cond, succmsg, errmsg) if (cond) { INFO(succmsg); } else { ERR(errmsg); }
#define LOG_CHECK_RETURN(cond, succmsg, errmsg, retcode) if (cond) { INFO(succmsg); } else { ERR(errmsg); return retcode; }
#define LOG_CHECK_RETURN_VOID(cond, succmsg, errmsg) if (cond) { INFO(succmsg); } else { ERR(errmsg); return; }

#define OVERRIDE_LOG_NAME(replacement) OverrideFunctionLogName(std::source_location::current().function_name(), replacement)


#endif