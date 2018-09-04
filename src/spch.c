/*
    MIT License

    Copyright (c) 2018 PS
    GitHub: https://github.com/ClnViewer/Split-post-commit-Hook---SVN-GIT-HG

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sub license, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.
 */

#include "spch.h"
#define __ISLOG ((dirs.fp[1]) ? 1 : 0)

#if !defined(OS_WIN)
#   include <signal.h>
#   include <sys/types.h>
#   include <sys/wait.h>
#   include <unistd.h>
#   include <fcntl.h>
#endif

int main(int argc, char *argv[])
{
    int ret;
    paths_t dirs;
    paths_t __AUTO(__autopathst) *ds = &dirs;
    memset(&dirs, 0, sizeof(dirs));

#   if defined(BUILD_MSVC)
    __try
    {
#   endif

        if ((ret = pch_option(&dirs, argv, argc)) != 0)
        {
            return ret;
        }
        /* stage #0 */
        if (__BITTST(dirs.bitopt, OPT_DEMONIZE))
        {
            if (__ISLOG)
            {
                fflush(dirs.fp[1]);
            }
            /* fork and wait,
               non-loop SVC call for 'update' command,
               fixed error: repo is locked
               (Linux/Windows) */
            switch(pch_fork(argc, argv))
            {
            case -1:
            {
                if (__ISLOG)
                {
                    pch_log_error(&dirs, "stage #%d non-loop fork error!", 0);
                    fflush(dirs.fp[1]);
                }
                exit(127);
            }
            case 0:
            {
                if (__ISLOG)
                {
                    pch_log_info(&dirs, "stage #0 non-loop mode: Parent %d successful exit", __PID);
                    fflush(dirs.fp[1]);
                }
#           if !defined(OS_WIN)
                ds = NULL;
#           endif
                exit(0);
            }
            default:
            {
                if (__ISLOG)
                {
                    pch_log_info(&dirs, "stage #0 non-loop mode: Daemon %d run successful", __PID);
                    fflush(dirs.fp[1]);
                }
                break;
            }
            }
        }
        do
        {
            /* set UID/GID for Linux */
            if (!pch_path_setuid(&dirs, __ISLOG))
            {
                ret = 125;
                break;
            }
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
            /* stage #3 check/deploy */
            ret = pch_stage3(&dirs);
            if (ret != 0)
            {
                if (__ISLOG)
                {
                    pch_log_info(&dirs, "stage #3 changed commit success: %s", dirs.setup[FILE_SPLIT_REPO].str);
                }
                break;
            }
            /* stage #4 */
            ret = pch_stage4(&dirs);
            if (ret > 0)
            {
                if (__ISLOG)
                {
                    pch_log_info(&dirs, "stage #4 changed commit success: %s", dirs.setup[FILE_SPLIT_REPO].str);
                }
                ret = 0;
                break;
            }
            else if (ret < 0)
            {
                pch_log_error(&dirs, "stage #4 incomplete, return error: %d", ret);
                ret = 119;
                break;
            }
            else if (!ret)
            {
                if (__ISLOG)
                {
                    pch_log_info(&dirs, "stage #4 not commit repo: %s", dirs.setup[FILE_SPLIT_REPO].str);
                }
                ret = 0;
                break;
            }
        }
        while (0);

        endedlog(&dirs);

#   if defined(BUILD_MSVC)
    }
    __finally
    {
        __autopathst(&ds);
    }
#   endif

    return ret;
}
