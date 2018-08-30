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

    if ((ret = pch_option(&dirs, argv, argc)) != 0)
    {
        return ret;
    }
    /* stage #0 */
    /* set UID/GID for Linux */
#   if !defined(OS_WIN)
    /* fork and wait,
       non-loop SVC call for 'update' command,
       fixed error: repo is locked
       (Linux) */
    if (__BITTST(dirs.bitopt, OPT_DEMONIZE))
    {
        if (__ISLOG)
        {
            fflush(dirs.fp[1]);
        }
        switch(fork())
        {
        case 0:
        {
            int cfd;
            (void) signal(SIGHUP, SIG_IGN);

            if ((cfd = open("/dev/null", O_RDWR, 0)) >= 0)
            {
                dup2(cfd, STDIN_FILENO);
                dup2(cfd, STDOUT_FILENO);
                dup2(cfd, STDERR_FILENO);

                if (cfd > 2)
                {
                    close(cfd);
                }
            }
            if ((getppid() != 1) && (setsid() < 0))
            {
                if (__ISLOG)
                {
                    pch_log_error(&dirs, "stage #%d non-loop set SID error:", 0);
                    exit(125);
                }
            }
            (void) sleep(12);
            break;
        }
        case -1:
        {
            if (__ISLOG)
            {
                pch_log_error(&dirs, "stage #%d non-loop fork error:", 0);
            }
            exit(127);
        }
        default:
        {
            (void) signal(SIGCHLD, SIG_IGN);
            if (__ISLOG)
            {
                pch_log_info(&dirs, "stage #0 non-loop mode: Parent %d successful exit", getpid());
                fflush(dirs.fp[1]);
            }
            ds = NULL;
            exit(0);
        }
        }
    }
#   endif

    do
    {
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
