#include "bbt_log.h"

void bbt::logger::init(int argc, char** argv)
{

	loguru::init(argc, argv);
	loguru::add_file("bbt_everything.log", loguru::Append, loguru::Verbosity_MAX);

}
