#include "bbt_assert.h"
#include "bbt_log.h"

void internarAssertImplemmentation(const char* expresion, const char* desc, const char* file, const char* function, int line)
{

    // DO THING...
    BBT_LOG(ERROR,
        "\n  -+-+-+-+-+-+-+-+-+-+-+-+-+-\n"
        "  [expr]:        %s\n"
        "  [file]:        %s\n"
        "  [function]:    %s\n"
        "  [line]:        %d\n"
        "  [description]: %s\n"
        "  -+-+-+-+-+-+-+-+-+-+-+-+-+-", expresion, file, function, line, desc);
}
