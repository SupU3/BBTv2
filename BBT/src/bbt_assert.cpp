#include "bbt_assert.h"
#include "log.h"

void internarAssertImplemmentation(const char* expresion, const char* desc, const char* file, const char* function, int line)
{

    // DO THING...
    LOG_MACRO(bbt::logLevel::error,
        "\n  -+-+-+-+-+-+-+-+-+-+-+-+-+-\n"
        "  [expr]:        {}\n"
        "  [file]:        {}\n"
        "  [function]:    {}\n"
        "  [line]:        {}\n"
        "  [description]: {}\n"
        "  -+-+-+-+-+-+-+-+-+-+-+-+-+-", expresion, file, function, line, desc);

}
