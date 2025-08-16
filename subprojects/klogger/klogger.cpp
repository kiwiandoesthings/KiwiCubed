#include <klogger.hpp>



static std::map<std::string, std::string> logColors = {
	{"DEBUG", "\033[1;34m"},
	{"INFO", "\033[1;32m"},
	{"WARN", "\033[1;33m"},
	{"ERROR", "\033[1;31m"},
	{"CRITICAL", "\033[1;41m"},
	{"RESET", "\033[0m"},
	{"SRCLOC", "\033[1;30m"},
	{"FUNCTION", "\033[1;35m"}
};


#define COLORED_STR(color, str) logColors[color] + str + logColors["RESET"]

LogLevel logLevel = LogLevel::debug;

std::map<std::string, std::string> functionHeaderReplacements;

std::string headerStructure = "[{level} | {function}]";

std::string LogLevelToString(LogLevel level) {
	switch (level) {
		case LogLevel::debug: return "DEBUG";
		case LogLevel::info: return "INFO";
		case LogLevel::warn: return "WARN";
		case LogLevel::error: return "ERROR";
		case LogLevel::critical: return "CRITICAL";
		case LogLevel::off: return "OFF";
		default: return "UNKNOWN";
	}
}

void OverrideFunctionLogName(const std::string& functionName, const std::string& replacement) {
	functionHeaderReplacements[functionName] = replacement;
}

void Log(LogLevel level, const std::string& message, const std::source_location &srclc) {
	if (level >= logLevel) {

		std::string header = headerStructure;
		#define REPLACE(f, r) header.replace(header.find(f), sizeof(f) - 1, r)

		std::string loglvlstr = LogLevelToString(level);
		REPLACE("{level}", COLORED_STR(loglvlstr, loglvlstr));

		std::string fname;

		if (functionHeaderReplacements.find(srclc.function_name()) != functionHeaderReplacements.end()) {
			fname = COLORED_STR("FUNCTION", functionHeaderReplacements[srclc.function_name()]);
		} else {
			fname = COLORED_STR("FUNCTION", srclc.function_name());
		}
		
		REPLACE("{function}", fname);

		std::string stupidStringThatIHate = "void __cdecl ";
		size_t stupid = header.find(stupidStringThatIHate);
    
    	if (stupid != std::string::npos) {
        	header.erase(stupid, stupidStringThatIHate.length());
    	}

		#undef REPLACE
		
		std::cout << header << " " << message << std::endl;
	}
}