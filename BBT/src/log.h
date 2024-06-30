#ifndef __BBT_LOG_H__
#define __BBT_LOG_H__ 1
#include <string>
#include <format>

#ifdef DEBUG
#define BBT_COMPILE_TIME_LEVEL ::bbt::logLevel::trace
#else
#define BBT_COMPILE_TIME_LEVEL ::bbt::logLevel::log
#endif

#define LOG_MACRO(_level, ...) \
if constexpr (_level >= BBT_COMPILE_TIME_LEVEL)\
	::bbt::logger::log(_level, std::format(__VA_ARGS__))

namespace bbt
{

	enum class logLevel
	{

		trace,
		log,
		warning,
		error

	};

	class logger
	{

		public:

			static void log(logLevel logLvL, const std::string& msg);


	};

}

#endif

