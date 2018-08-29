#include "spch.h"

void pch_log_(int type, paths_t *dirs, const char *fmt, ...)
{
    FILE *lout;

    lout = ((dirs->fp[1]) ? dirs->fp[1] :
            ((__BITTST(dirs->bitopt, OPT_QUIET)) ? NULL :
             ((type) ? stdout : stderr)
            )
           );

    if (lout)
    {
        va_list va;
        va_start(va, fmt);
        vfprintf(lout, fmt, va);
        va_end(va);
    }
}
