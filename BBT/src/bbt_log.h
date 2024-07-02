#ifndef __BBT_LOG_H__
#define __BBT_LOG_H__ 1
#include "loguru.hpp"

/*

        Verbosity_FATAL   = -3,
        Verbosity_ERROR   = -2,
        Verbosity_WARNING = -1,

        // Normal messages. By default written to stderr.
        Verbosity_INFO    =  0,
*/

#ifdef  DEBUG
#define BBT_COMPILETIME_LEVEL    0
#else //DEBUG
#define BBT_COMPILETIME_LEVEL   -1
#endif//DEBUG

// Defines Babytalkcentral log macro.
#define BBT_LOG( _Level, ... )  \
    if constexpr ( static_cast<int>(loguru::Verbosity_##_Level) <= BBT_COMPILETIME_LEVEL) \
    { \
        LOG_F( _Level, __VA_ARGS__ ); \
    }

namespace bbt
{

	class logger
	{

		public:

			static void init(int argc, char** argv);


	};

}

#endif

