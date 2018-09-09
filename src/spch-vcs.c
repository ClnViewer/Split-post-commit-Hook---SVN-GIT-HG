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
#include "spch-vcs.h"
#include "version.h"

/* Version Control System */

static int __select_vcs_type(unsigned long bitopt)
{
    return (
               ((__BITTST(bitopt, OPT_VCS_HG)) ? OPT_VCS_HG :
                ((__BITTST(bitopt, OPT_VCS_GIT)) ? OPT_VCS_GIT : OPT_VCS_SVN
                )));
}
static const char * __select_vcs_bin(paths_t *dirs, vcs_bin_e type)
{
    if (dirs->bins[type].str)
        return (const char*)dirs->bins[type].str;

    if (!string_format(
                &dirs->bins[type],
                "%s" __PSEPS "%s",
                dirs->setup[FILE_BINDIR].str,
                vcs_bin[__select_vcs_type(dirs->bitopt)][type]
            )
       )
    {
        return NULL;
    }
    return (const char*)dirs->bins[type].str;
}

const char * pch_vcs_type(unsigned long bitopt)
{
    return (
               ((__BITTST(bitopt, OPT_VCS_HG)) ? vcs_bin[2][0] :
                ((__BITTST(bitopt, OPT_VCS_GIT)) ? vcs_bin[1][0] : vcs_bin[0][0]
                )));
}

int pch_vcs_bincheck(paths_t *dirs)
{
    string_s chk;
    if (
        (!(chk.str = (char*) __select_vcs_bin(dirs, VCS_BIN_ACT))) ||
        (!pch_check_file(&chk))
    )
        return 0;

    if (
        (!(chk.str = (char*) __select_vcs_bin(dirs, VCS_BIN_ADMIN))) ||
        (!pch_check_file(&chk))
    )
        return 0;

    return 1;
}

int pch_vcs_update(paths_t *dirs, string_s *repo)
{
    const char** args;

    if (_chdir(repo->str) < 0)
    {
        return -1;
    }
    args = (const char**)__vcs[__select_vcs_type(dirs->bitopt)].update;
    if (!(args[0] = __select_vcs_bin(dirs, VCS_BIN_ACT)))
        return -1;

    return pch_exec(dirs, (const char**)__vcs[__select_vcs_type(dirs->bitopt)].update, NULL);
}

int pch_vcs_commit(paths_t *dirs)
{
    int ret = -1;
    const char** args;
    string_s barg3 = { NULL, 0U };
    string_s __AUTO(__autostring) *arg3 = &barg3;

#   if defined(BUILD_MSVC)
    __try
    {
#   endif

        if (_chdir(dirs->setup[FILE_SPLIT_REPO].str) < 0)
        {
            return ret;
        }
        do
        {
            args = (const char**)__vcs[__select_vcs_type(dirs->bitopt)].commit;
            if (!(args[0] = __select_vcs_bin(dirs, VCS_BIN_ACT)))
                break;

            if (dirs->rev)
            {
                if (!string_format(
                            arg3,
                            "-m\"split auto commit rev.%lu (spch v.%s)\"",
                            dirs->rev,
                            SPCH_FULLVERSION_STRING
                        )
                   )
                {
                    break;
                }
            }
            else
            {
                if (!string_format(
                            arg3,
                            "-m\"split auto commit (spch v.%s)\"",
                            SPCH_FULLVERSION_STRING
                        )
                   )
                {
                    break;
                }
            }
            args[3] = arg3->str;
            ret = pch_exec(dirs, args, NULL);

            if (ret)
                break;

            args = (const char**)__vcs[__select_vcs_type(dirs->bitopt)].push;

            if ((!args[1]) || (!(args[0] = __select_vcs_bin(dirs, VCS_BIN_ACT))))
                break;

            ret = pch_exec(dirs, args, NULL);
        }
        while (0);

#   if defined(BUILD_MSVC)
    }
    __finally
    {
        if (arg3->str)
        {
            __autostring(&arg3);
        }
    }
#   endif

    return ret;
}

int pch_vcs_add(paths_t *dirs, string_s *dir)
{
    const char** args;

    if (_chdir(dir->str) < 0)
    {
        return -1;
    }
    args = (const char**)__vcs[__select_vcs_type(dirs->bitopt)].add;
    if (!(args[0] = __select_vcs_bin(dirs, VCS_BIN_ACT)))
        return -1;

    return pch_exec(dirs, args, NULL);
}

int pch_vcs_create(paths_t *dirs)
{
    int ret = -1;
    string_s barg2 = { NULL, 0U };
    string_s __AUTO(__autostring) *arg2 = &barg2;

#   if defined(BUILD_MSVC)
    __try
    {
#   endif

        if (
            (!dirs->setup[FILE_ROOTVCS].str) ||
            (_chdir(dirs->setup[FILE_ROOTVCS].str) < 0)
        )
        {
            return -1;
        }
        do
        {
            const char *mhome;
            const char** args = (const char**)__vcs[__select_vcs_type(dirs->bitopt)].admin;
            if (!(args[0] = __select_vcs_bin(dirs, VCS_BIN_ADMIN)))
                break;

            mhome = strrchr(dirs->setup[FILE_MASTER_REPO].str, __PSEPC);
            mhome = ((!mhome) ? dirs->setup[FILE_MASTER_REPO].str : (mhome + 1));

            if (!string_format(
                        arg2,
                        "%s" __PSEPS "%s",
                        dirs->setup[FILE_ROOTVCS].str,
                        mhome
                    )
               )
            {
                break;
            }
            args[2] = arg2->str;
            ret = pch_exec(dirs, args, NULL);
        }
        while (0);

#   if defined(BUILD_MSVC)
    }
    __finally
    {
        if (arg2->str)
        {
            __autostring(&arg2);
        }
    }
#   endif

    return ret;
}

int pch_vcs_log(paths_t *dirs)
{
    int err, ret = -1;

    if (
        (!dirs->setup[FILE_MASTER_REPO].str) ||
        (_chdir(dirs->setup[FILE_MASTER_REPO].str) < 0)
    )
    {
        return -1;
    }
    do
    {
        char brev[20] = {0};
        const char** args = (const char**)__vcs[__select_vcs_type(dirs->bitopt)].log;
        if (!(args[0] = __select_vcs_bin(dirs, VCS_BIN_ACT)))
            break;

        if (__select_vcs_type(dirs->bitopt) == OPT_VCS_SVN)
        {
            if (dirs->rev)
            {
                args[3] = pch_ultostr(brev, dirs->rev, 10);
            }
            else
            {
                args[2] = args[4];
                args[3] = NULL;
            }
        }
        if (!(dirs->fp[PATHS_FILE_TMP] = tmpfile()))
        {
            break;
        }
        if ((ret = pch_exec(dirs, args, dirs->fp[PATHS_FILE_TMP])))
        {
            err = errno;
            if (dirs->fp[PATHS_FILE_TMP])
                fclose(dirs->fp[PATHS_FILE_TMP]);

            dirs->fp[PATHS_FILE_TMP] = NULL;
            errno = err;
        }
        else if (dirs->fp[PATHS_FILE_TMP])
        {
            err = errno;
            {
                (void) fflush(dirs->fp[PATHS_FILE_TMP]);
                (void) fseek(dirs->fp[PATHS_FILE_TMP], 0, SEEK_SET);
            }
            errno = err;
        }
    }
    while (0);

    return ret;
}

bool_t pch_vcs_changelog(paths_t *dirs)
{
    bool_t ret = R_NEGATIVE;

    string_s lf = { NULL, 0U };
    string_s __AUTO(__autostring) *lout = &lf;
    FILE __AUTO(__autofclose) *fchglog = NULL;
    FILE *ftmplog = NULL;

#       if defined(BUILD_MSVC)
    __try
    {
#       endif

        do
        {
            if (pch_vcs_log(dirs))
            {
                break;
            }
            if (!(ftmplog = tmpfile()))
            {
                break;
            }
            if (spch_xmllog(dirs, dirs->fp[PATHS_FILE_TMP], ftmplog) != R_TRUE)
            {
                (void) fclose(ftmplog);
                break;
            }

            (void) fclose(dirs->fp[PATHS_FILE_TMP]);
            dirs->fp[PATHS_FILE_TMP] = NULL;

            if (
                (fseek(ftmplog, 0, SEEK_END)) ||
                (!ftell(ftmplog))
            )
            {
                (void) fclose(ftmplog);
                ret = R_FALSE;
                break;
            }
            if (
                (!string_format(
                     lout,
                     "%s" __PSEPS __CHNGLOG "%s",
                     dirs->setup[FILE_SPLIT_REPO].str,
                     ((__BITTST(dirs->bitopt, OPT_CHLOG_MD)) ? "md" : "txt")
                 )
                ) ||
                (!(fchglog = fopen(lout->str, "a+"))) ||
                (fseek(fchglog, 0, SEEK_SET)) ||
                (fseek(ftmplog, 0, SEEK_SET))
            )
                break;

            {
                size_t sz;
                char b[BUFSIZ];
                (void) fflush(ftmplog);

                while ((sz = fread(b, 1U, (size_t)BUFSIZ, ftmplog)) != 0U)
                {
                    (void) fwrite(b, 1U, sz, fchglog);
                }

                (void) fclose(ftmplog);
                (void) fclose(fchglog);
                fchglog = NULL;
            }

            ret = R_TRUE;
        }
        while (0);

#       if defined(BUILD_MSVC)
    }
    __finally
    {
        if (lout->str)
        {
            __autostring(&lout);
        }
        if (fchglog)
        {
            (void) fclose(fchglog);
        }
    }
#       endif

    return ret;
}
