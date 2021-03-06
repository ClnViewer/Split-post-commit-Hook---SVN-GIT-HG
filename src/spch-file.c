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
#   include <pwd.h>
#endif

size_t pch_check_(string_s *fpath, type_io_e type)
{
    struct stat _dst1;

    if (stat(fpath->str, &_dst1) < 0)
        return 0U;

    switch (type)
    {
    case TYPE_DIR:
    {
#       if defined(OS_WIN)
        return ((((_dst1.st_mode) & _S_IFDIR) == _S_IFDIR) ? 1U : 0U);
#       else
        return ((S_ISDIR(_dst1.st_mode)) ? 1U : 0U);
#       endif
    }
    case TYPE_FILE:
    {
#       if defined(OS_WIN)
        return ((((_dst1.st_mode) & _S_IFREG) == _S_IFREG) ? (size_t)_dst1.st_size : 0U);
#       else
        return ((S_ISREG(_dst1.st_mode)) ? (size_t)_dst1.st_size : 0U);
#       endif
    }
    case TYPE_UID:
    {
#       if defined(OS_WIN)
        return 1;
#       else
        do
        {
            uid_t uid  = getuid();

            if ((!uid) || (uid != _dst1.st_uid))
            {
                if (setegid(_dst1.st_gid))
                    break;
                if (seteuid(_dst1.st_uid))
                    break;
            }
            return 1;
        }
        while (0);

        return 0;
#       endif
    }
    default:
    {
        return 0U;
    }
    }
}

bool_t pch_compare_file(paths_t *dirs, string_s *fpath1, string_s *fpath2)
{
    bool_t ret = R_FALSE;
    struct stat _dst1, _dst2;

    if (
        (stat(fpath1->str, &_dst1) < 0) ||
#       if defined(BUILD_MSVC)
        (((_dst1.st_mode) & _S_IFREG) == _S_IFREG)
#       else
        (!S_ISREG(_dst1.st_mode))
#       endif
    )
        return R_NEGATIVE;

    if (stat(fpath2->str, &_dst2) < 0)
        return R_TRUE;

#   if defined(BUILD_MSVC)
    if (((_dst2.st_mode) & _S_IFREG) != _S_IFREG)
        return -1;
#   else
    if (!S_ISREG(_dst2.st_mode))
        return R_NEGATIVE;
#   endif

    do
    {
        if (!_dst1.st_size)
            break;

        if (__BITTST(dirs->bitopt, OPT_FCHECK_CTIME))
        {
            if ((ret = ((unsigned long) _dst1.st_ctime > (unsigned long) _dst2.st_ctime)))
                break;
        }
        if (__BITTST(dirs->bitopt, OPT_FCHECK_MTIME))
        {
            if ((ret = ((unsigned long) _dst1.st_mtime > (unsigned long) _dst2.st_mtime)))
                break;
        }
        if (__BITTST(dirs->bitopt, OPT_FCHECK_SIZE))
        {
            ret = (_dst1.st_size != _dst2.st_size);
        }
    }
    while (0);

    return ret;
}

int pch_fcopy(string_s *from_s, string_s *to_s)
{
    int    ret = 0;
    size_t n;
    char   b[BUFSIZ];
    FILE __AUTO(__autofclose) *from_f = NULL, *to_f = NULL;

#   if defined(BUILD_MSVC)
    char   to_b[1024];
    __try
    {
        if ((to_s->sz + 1 + 4) > sizeof(to_b))
            return R_NEGATIVE;

#   else
    char   to_b[(to_s->sz + 1 + 4)];
#   endif

        errno = 0;

        if (
            (__snprintf(to_b, (to_s->sz + 1 + 4), "%s.tmp", to_s->str) <= 0) ||
            (!(from_f = fopen(from_s->str, "rb"))) ||
            (!(to_f = fopen(to_b, "wb")))
        )
            return ((!errno) ? EIO : errno);

        while ((n = fread(b, sizeof(char), sizeof(b), from_f)) > 0)
        {
            if (fwrite(b, sizeof(char), n, to_f) != n)
            {
                (void) remove(to_b);
                return ((!errno) ? EIO : errno);
            }
        }

        fclose (to_f);
        to_f = NULL;

        if (rename(to_b, to_s->str) < 0)
        {
            if (errno == EEXIST)
            {
                errno = 0;
                if (remove(to_s->str) < 0)
                {
                    ret = errno;
                }
                else
                {
                    ret = ((rename(to_b, to_s->str) < 0) ? errno : 0);
                }
            }
            else
            {
                ret = errno;
            }
        }

#   if defined(BUILD_MSVC)
    }
    __finally
    {
        __autofclose(&from_f);
        __autofclose(&to_f);
    }
#   endif

    return ret;
}

bool_t pch_path_dir(string_s *dst, string_s *src)
{
    int  sz;
    char *ctmp;

    if (!(ctmp = strrchr(src->str, __PSEPC)))
    {
        return R_FALSE;
    }

    if (
        ((sz = (int)(ctmp - src->str)) < 0) ||
        (!(dst->str = calloc(1, (size_t)(sz + 1)))) ||
        (!memcpy(dst->str, src->str, (size_t)sz))
    )
        return R_NEGATIVE;

    dst->sz           = (size_t)sz;
    dst->str[dst->sz] = '\0';
    return R_TRUE;
}

bool_t pch_path_time(char b[], const char *fmt)
{
    struct tm *tmi;
    time_t     tmc = time(NULL);

    if (
        ((tmi = localtime(&tmc)) == NULL) ||
        (strftime(b, 100, fmt, tmi) <=0)
    )
    {
        return R_FALSE;
    }

    return R_TRUE;
}

void pch_path_free(paths_t *dirs)
{
    size_t i;
    for (i = 0; i < FILE_NONE_IDX; i++)
    {
        if (dirs->setup[i].str)
        {
            free(dirs->setup[i].str);
            dirs->setup[i].str = NULL;
            dirs->setup[i].sz  = 0U;
        }
    }
    for (i = 0; i < 2; i++)
    {
        if (dirs->bins[i].str)
        {
            free(dirs->bins[i].str);
            dirs->bins[i].str = NULL;
            dirs->bins[i].sz  = 0U;
        }
    }
    for (i = 0; i < __NELE(dirs->fp); i++)
    {
        if (dirs->fp[i])
        {
            fflush(dirs->fp[i]);
            fclose(dirs->fp[i]);
            dirs->fp[i] = NULL;
        }
    }
    dirs->bitopt = dirs->rev = 0UL;
}

bool_t pch_path_setuid(paths_t *dirs, int islog)
{
#   if !defined(OS_WIN)
    do
    {
        if ((__BITTST(dirs->bitopt, OPT_UUID)) && (getuid() == 0))
        {
            struct passwd *p;

            if (!(p = getpwnam(dirs->setup[FILE_UUID].str)))
            {
                pch_log_error(dirs, "get UID from '%s' error: %s", dirs->setup[FILE_UUID].str, strerror(errno));
                break;
            }
            if (!p->pw_uid)
            {
                pch_log_error(dirs, "get UID from '%s' error: is root access login!", dirs->setup[FILE_UUID].str);
                break;
            }
            if (setegid(p->pw_gid))
            {
                pch_log_error(dirs, "stage #0 set GID %d as '%s' error: %s", p->pw_gid, dirs->setup[FILE_UUID].str, strerror(errno));
                break;
            }
            if (seteuid(p->pw_uid))
            {
                pch_log_error(dirs, "stage #0 set UID %d as '%s' error: %s", p->pw_uid, dirs->setup[FILE_UUID].str, strerror(errno));
                break;
            }
            if (islog)
            {
                pch_log_info(dirs, "stage #0 change UID/GID to '%s' : %d/%d", dirs->setup[FILE_UUID].str, p->pw_uid, p->pw_gid);
            }
        }
        else
        {
            if (!pch_check_uid(&dirs->setup[FILE_SPLIT_REPO]))
            {
                pch_log_error(dirs, "stage #0 change UID/GID from directory error: %s", dirs->setup[FILE_SPLIT_REPO].str);
                break;
            }
        }
        return R_TRUE;

    }
    while (0);

    return R_FALSE;
#   else
    (void) dirs;
    (void) islog;
    return R_TRUE;
#   endif
}

bool_t pch_path_destination(paths_t *dirs, char *src, size_t sz, string_s *dst)
{
    int mode = ((__BITTST(dirs->bitopt, OPT_RENAME)) ? 2 :
                ((__BITTST(dirs->bitopt, OPT_PREFIX)) ? 1 :
                 0)
               );

#   if defined(BUILD_MSVC)
    char b[1024];

    if ((sz + 1U) > sizeof(b))
        return R_FALSE;

#   else
    char b[(sz + 1U)];
#   endif

    memcpy((void*)b, (void*)src, sz);
    b[sz] = '\0';

    switch (mode)
    {
    case 1:
    case 2:
    {
        int ismod = 0;
        char *fname = NULL, *fpath  = NULL;

        if ((fname = strrchr(b, __PSEPC)))
        {
            int ssz = (fname - (char*)b);
            fpath = (char*)b;
            fpath[ssz] = '\0';
            fname++;
        }
        else
        {
            fname = (char*)b;
        }
        if (
            (mode == 2) &&
            (memcmp(fname, dirs->setup[FILE_RENAME1].str, dirs->setup[FILE_RENAME1].sz) == 0)
        )
        {
            ismod = 1;
            fname += dirs->setup[FILE_RENAME1].sz;
        }
        if (!string_format(dst, "%s" __PSEPS "%s%s%s%s%s",
                           dirs->setup[FILE_SPLIT_REPO].str,
                           ((fpath) ? fpath : ""),
                           ((fpath) ? __PSEPS : ""),
                           (((mode == 2) && (ismod)) ?
                            dirs->setup[FILE_RENAME2].str : ""),
                           ((mode == 1) ?
                            dirs->setup[FILE_RENAME1].str : ""),
                           ((fname) ? fname : "")
                          )
           )
            return R_FALSE;

        break;
    }
    default:
    {
        if (!string_format(dst, "%s" __PSEPS "%s",
                           dirs->setup[FILE_SPLIT_REPO].str,
                           b
                          )
           )
            return R_FALSE;

        break;
    }
    }

    return R_TRUE;
}

const char * pch_ultostr(char *str, unsigned long val, int base)
{
    unsigned long t = 0UL, b = (unsigned long)base;
    unsigned long tmp = val;
    int count = 0;

    if (!str)
    {
        return NULL;
    }

    if (!tmp)
    {
        count++;
    }

    while(tmp > 0)
    {
        tmp = (tmp / b);
        count++;
    }

    str += count;
    *str = '\0';

    do
    {
        unsigned long res = val - b * (t = val / b);
        if (res < 10U)
        {
            * --str = (char)('0' + res);
        }
        else if ((res >= 10U) && (res < 16U))
        {
            * --str = (char)('A' - 10U + res);
        }
    }
    while ((val = t) != 0);
    return (const char*)str;
}
