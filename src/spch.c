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

#if !defined(OS_WIN)
#   include <signal.h>
#   include <sys/types.h>
#   include <sys/wait.h>
#   include <unistd.h>
#   include <fcntl.h>
#endif

int main(int argc, char *argv[])
{
    int ret = 0;
    bool_t bret;
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

        //pch_stage2(&dirs);
        //return 0;

        /* stage #0 */
        if (__BITTST(dirs.bitopt, OPT_DEMONIZE))
        {
            if (__ISLOGS)
            {
                fflush(dirs.fp[1]);
            }
            /* fork and wait,
               non-loop SVC call for 'update' command,
               fixed error: repo is locked
               (Linux/Windows) */
            switch(pch_fork(argc, argv))
            {
            case R_NEGATIVE:
            {
                if (__ISLOGS)
                {
                    pch_log_error(&dirs, "stage #%d non-loop fork error!", 0);
                    fflush(dirs.fp[1]);
                }
                exit(127);
            }
            case R_FALSE:
            {
                if (__ISLOGS)
                {
                    pch_log_info(&dirs, "stage #0 non-loop mode: Parent %d successful exit", __PID);
                    fflush(dirs.fp[1]);
                }
#           if !defined(OS_WIN)
                ds = NULL;
#           endif
                exit(0);
            }
            case R_TRUE:
            {
                if (__ISLOGS)
                {
                    pch_log_info(&dirs, "stage #0 non-loop mode: Daemon %d run successful", __PID);
                    fflush(dirs.fp[1]);
                }
                break;
            }
            default:
            {
                if (__ISLOGS)
                {
                    pch_log_info(&dirs, "stage #%d unknown select error, exit", 0);
                    endedlog(&dirs);
                    fflush(dirs.fp[1]);
                    exit(0);
                }
                break;
            }
            }
        }
        do
        {
            /* stage #1 */
            /* set UID/GID for Linux */
            if (pch_path_setuid(&dirs, __ISLOGS) != R_TRUE)
            {
                pch_log_error(&dirs, "stage #%d error set UID/GID, exit", 1);
                ret = 125;
                break;
            }
            if (__ISLOGS)
            {
                pch_log_info(&dirs, "stage #1 check files mode: %s", pch_option_chkmode(&dirs));
            }
            /* stage #1 VCS update */
            bret = pch_stage1(&dirs);
            if (bret != R_TRUE)
            {
                pch_log_error(&dirs, "stage #%d VCS update incomplete, return 0", 1);
                ret = 121;
                break;
            }
            else
            {
                if (__ISLOGS)
                {
                    pch_log_info(&dirs, "stage #%d VCS update master/slave success", 1);
                }
            }
            /* stage #2 */
            bret = pch_stage2(&dirs);
            if (bret == R_TRUE)
            {
                if (__ISLOGS)
                {
                    pch_log_info(&dirs, "stage #2 changed repo objects: %s", dirs.setup[FILE_SPLIT_REPO].str);
                }
            }
            else if (bret == R_FALSE)
            {
                if (__ISLOGS)
                {
                    pch_log_info(&dirs, "stage #2 not change repo: %s", dirs.setup[FILE_SPLIT_REPO].str);
                }
                ret = 0;
                break;
            }
            else if (bret == R_NEGATIVE)
            {
                pch_log_error(&dirs, "stage #%d incomplete, return error", 2);
                ret = 120;
                break;
            }
            /* stage #3 check/deploy */
            bret = pch_stage3(&dirs);
            if (bret != R_TRUE)
            {
                pch_log_error(&dirs, "stage #3 check/deploy error: %s", dirs.setup[FILE_SPLIT_REPO].str);
                break;
            }
            else
            {
                if (__ISLOGS)
                {
                    pch_log_info(&dirs, "stage #3 check/deploy success: %s", dirs.setup[FILE_SPLIT_REPO].str);
                }
            }
            /* stage #4 */
            bret = pch_stage4(&dirs);
            if (bret == R_TRUE)
            {
                if (__ISLOGS)
                {
                    pch_log_info(&dirs, "stage #4 send commit success: %s", dirs.setup[FILE_SPLIT_REPO].str);
                }
                ret = 0;
                break;
            }
            else
            {
                pch_log_error(&dirs, "stage #%d commit incomplete, return error", 4);
                ret = 119;
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
