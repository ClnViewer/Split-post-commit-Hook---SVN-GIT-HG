#include "spch.h"

#if defined(OS_WIN)
#   include <windows.h>
#else
#   include <signal.h>
#   include <sys/types.h>
#   include <sys/wait.h>
#   include <unistd.h>
#endif

#define __ISLOG ((dirs->fp[1]) ? 1 : 0)

#if defined(OS_WIN)
static void __get_error(char b[], size_t bsz, unsigned long err)
{
    FormatMessage(
        FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        err,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        b,
        bsz,
        NULL
    );
}
#endif

int pch_exec(paths_t *dirs, const char *const opt[])
{
#   if defined(OS_WIN)

    int ret;
    unsigned long ecode = 0UL;
    size_t sz = 0;
    char __AUTO(__autofree) *cmd = NULL;
    char *p;

    for (; *opt; opt++)
    {
        size_t ssz;

        if (
            (!opt) ||
            ((ssz = (strlen(*opt) + 1U)) == 1U)
        )
            break;

        if (!(p = realloc(cmd, (size_t)(sz + ssz + 1U))))
        {
            if (cmd)
                free(cmd);
            return -1;
        }

        cmd = p;
        memcpy((void*)(p + sz), *opt, ssz);
        sz += (ssz - 1);
        p[sz] = 0x20;
        sz += 1;
    }

    sz = ((sz) ? (sz - 1) : 0);

    if ((!cmd) || (!sz))
    {
        errno = EINVAL;
        return -1;
    }

    cmd[sz] = 0x0;

    if (__ISLOG)
    {
        dirs->fpos = ftell(dirs->fp[1]);
    }

    do
    {
        STARTUPINFO si;
        PROCESS_INFORMATION pi;

        memset(&si, 0, sizeof(si));
        memset(&pi, 0, sizeof(pi));
        si.cb = sizeof(si);

        if (!CreateProcess(
                    NULL,
                    cmd,
                    NULL,
                    NULL,
                    FALSE,
                    CREATE_NEW_PROCESS_GROUP,
                    NULL,
                    NULL,
                    &si,
                    &pi
                )
           )
        {
            char errstr[256] = {0};
            unsigned long errco = GetLastError();
            __get_error(errstr, sizeof(errstr), errco);
            pch_log_error(dirs, "create exec fatal: %s", cmd);
            return -1;
        }

        WaitForSingleObject(pi.hProcess, INFINITE);
        if (!GetExitCodeProcess(pi.hProcess, &ecode))
        {
            char errstr[256] = {0};
            unsigned long errco = GetLastError();
            __get_error(errstr, sizeof(errstr), errco);
            pch_log_error(dirs, "create exec fatal: %s", cmd);
            return -1;
        }
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    }
    while (0);

    ret = ((!ecode) ? 0 : -((int)ecode));
    return ret;

#   elif defined(__GNUC__)

    int cstatus;
    pid_t pid;

    (void) dirs;
    errno = 0;

    switch((pid = fork()))
    {
    case -1:
    {
        pch_log_error(dirs, "exec '%s -> %s' error fork: [%d] - %s",
                      opt[0], opt[1], errno, strerror(errno)
                     );
        return -1;
    }
    case 0:
    {
        if (__ISLOG)
        {
            (void) fflush(dirs->fp[1]);
            dirs->fpos = ftell(dirs->fp[1]);
            (void) dup2(fileno(dirs->fp[1]), 1);
            (void) dup2(fileno(dirs->fp[1]), 2);
        }
        (void) alarm(120U);
        (void) execv(opt[0], (char * const*)opt);
        exit((errno == ENOEXEC) ? 126 : 127);
    }
    default:
    {
        pid_t wpid;

        while(1)
        {
            if ((wpid = waitpid(pid, &cstatus, WNOHANG)))
                break;

            if ((WIFSIGNALED(cstatus)) && (WTERMSIG(cstatus) == SIGALRM))
                break;
        }
        if (WIFEXITED(cstatus))
        {
            return WEXITSTATUS(cstatus);
        }
        else if (WIFSIGNALED(cstatus))
        {
            pch_log_error(dirs, "exec '%s -> %s' exit at signal: [%d] - %s",
                          opt[0], opt[1], WTERMSIG(cstatus), strsignal(WTERMSIG(cstatus))
                         );
            if (WTERMSIG(cstatus) == SIGALRM)
            {
                (void) kill(pid, SIGKILL);
                (void) wait(NULL);
            }
            return WTERMSIG(cstatus);
        }
        else if (WIFSTOPPED(cstatus))
        {
            pch_log_error(dirs, "exec '%s -> %s' exit at stoped: [%d] - %s",
                          opt[0], opt[1], WSTOPSIG(cstatus), strsignal(WSTOPSIG(cstatus))
                         );
            return -2;
        }
#       if defined(WIFCONTINUED)
        else if (WIFCONTINUED(cstatus))
        {
            pch_log_error(dirs, "exec '%s -> %s' exit at continued",
                          opt[0], opt[1]
                         );
            return -3;
        }
#       endif
        else if (WCOREDUMP(cstatus))
        {
            pch_log_error(dirs, "exec '%s -> %s' exit at core dumped",
                          opt[0], opt[1]
                         );
            return -4;
        }
        break;
    }
    }
    return 0;

#   else
    return -1;

#   endif

}
