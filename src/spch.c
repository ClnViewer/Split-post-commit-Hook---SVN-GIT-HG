#include "spch.h"
#define __ISLOG ((dirs.fp[1]) ? 1 : 0)

int main(int argc, char *argv[])
{
    int ret;
    paths_t dirs;
    paths_t __AUTO(__autopathst) *ds = &dirs;
    memset(&dirs, 0, sizeof(dirs));

    do
    {
        if ((ret = pch_option(&dirs, argv, argc)) != 0)
        {
            return ret;
        }
        /* stage #0 */
        /* set UID/GID for Linux */
#       if !defined(OS_WIN)
        if (!pch_path_setuid(&dirs, __ISLOG))
        {
            ret = 125;
            break;
        }
#       endif
        if (__ISLOG)
        {
            pch_log_info(&dirs, "stage #0 check files mode: %s", pch_option_chkmode(&dirs));
        }
        /* stage #1 */
        if (!pch_stage1(&dirs))
        {
            pch_log_error(&dirs, "stage #1 incomplete, return: %d", 0);
            ret = 121;
            break;
        }
        /* stage #2 */
        ret = pch_stage2(&dirs);
        if (ret > 0)
        {
            if (__ISLOG)
            {
                pch_log_info(&dirs, "stage #2 changed repo objects: %d -> %s", ret, dirs.setup[FILE_SPLIT_REPO].str);
            }
        }
        else if (ret < 0)
        {
            pch_log_error(&dirs, "stage #2 incomplete, return: %d", ret);
            ret = 120;
            break;
        }
        else if (!ret)
        {
            if (__ISLOG)
            {
                pch_log_info(&dirs, "stage #2 not change repo: %s", dirs.setup[FILE_SPLIT_REPO].str);
            }
            ret = 0;
            break;
        }
        /* stage #3 */
        ret = pch_stage3(&dirs, ret);
        if (ret > 0)
        {
            if (__ISLOG)
            {
                pch_log_info(&dirs, "stage #3 changed commit success: %s", dirs.setup[FILE_SPLIT_REPO].str);
            }
            ret = 0;
            break;
        }
        else if (ret < 0)
        {
            pch_log_error(&dirs, "stage #3 incomplete, return error: %d", ret);
            ret = 119;
            break;
        }
        else if (!ret)
        {
            if (__ISLOG)
            {
                pch_log_info(&dirs, "stage #3 not commit repo: %s", dirs.setup[FILE_SPLIT_REPO].str);
            }
            ret = 0;
            break;
        }
    }
    while (0);

    endedlog(&dirs);

    return ret;
}
