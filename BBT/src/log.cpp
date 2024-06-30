#include "log.h"
#include "SDL3/SDL_log.h"

void bbt::logger::log(logLevel logLvL, const std::string& msg)
{

	switch (logLvL)
	{

		case logLevel::trace:
			SDL_LogVerbose(SDL_LOG_CATEGORY_CUSTOM,"%s", msg.c_str());
			break;

		case logLevel::log:
			SDL_LogInfo(SDL_LOG_CATEGORY_CUSTOM, "%s", msg.c_str());
			break;

		case logLevel::warning:
			SDL_LogWarn(SDL_LOG_CATEGORY_CUSTOM, "%s", msg.c_str());
			break;

		case logLevel::error:
			SDL_LogError(SDL_LOG_CATEGORY_CUSTOM, "%s", msg.c_str());
			break;

		default:
			break;
	}

}