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
#include "spch-link-hash.h"

#if !defined(OS_WIN)
#   include "spch-stage-yaml.h"
#   include "spch-shell.h"
#endif

#define __ISLOG ((dirs->fp[1]) ? 1 : 0)

static void __stage2_vcs_add(unsigned int cnt, unsigned long hash, char *str, size_t sz, void * data)
{
    int ret;
    paths_t *dirs = (paths_t*)data;
    string_s sdir = { str, 0U };
    (void) hash;
    (void) sz;

    if ((ret = pch_vcs_add(dirs, &sdir)) != 0)
    {
        pch_log_error(dirs, "add VCS directory error: %d -> %u) %s", ret, cnt, sdir.str);
    }
}

int pch_stage1(paths_t *dirs)
{
    int ret;

    if ((ret = pch_vcs_update(dirs, &dirs->setup[FILE_MASTER_REPO])) != 0)
    {
        if ((!dirs->fp[1]) || (dirs->fpos != ftell(dirs->fp[1])))
        {
            pch_log_info(dirs, "update VCS master repo loop?: %d -> %s", ret, dirs->setup[FILE_MASTER_REPO].str);
        }
    }
    if ((ret = pch_vcs_update(dirs, &dirs->setup[FILE_SPLIT_REPO])) != 0)
    {
        pch_log_error(dirs, "update VCS split repo error: %d -> %s", ret, dirs->setup[FILE_SPLIT_REPO].str);
        return 0;
    }
    return 1;
}

int pch_stage2(paths_t *dirs)
{
    int rcode = 0, stage = 0;
    char b[BUFSIZ + 1] = {0};
    hash_t *hd, *hf;
    hd = hash_init();
    hf = hash_init();

    while (fgets(b, BUFSIZ, dirs->fp[0]) != NULL)
    {
        int ret;
        string_s from_s, to_s, to_dir_s;
        string_s __AUTO(__autostring)
        *from_free = &from_s,
         *to_free = &to_s,
          *to_dir_free = &to_dir_s;

        memset(&from_s,   0, sizeof(string_s));
        memset(&to_s,     0, sizeof(string_s));
        memset(&to_dir_s, 0, sizeof(string_s));

        size_t len = strlen(b);
        len = ((len > 0U) ? (len - 1U) : 0U);

        if (!len)
            continue;

        b[len] = '\0';

        /* skip duplicate */
        if (hf->searchs(hf->hash, b, len))
            continue;

        (void) hf->adds(hf->hash, b, len);

        if (
            (!pch_path_format(&from_s, "%s" __PSEPS "%s", dirs->setup[FILE_MASTER_REPO].str, b)) ||
            (!pch_path_destination(dirs, (char*)b, len, &to_s))
        )
        {
            rcode = -1;
            break;
        }

#       if defined(__DEBUG__)
        printf("%d) [%s]: %s -> %s\n", __LINE__, __func__, from_s.str, to_s.str);
#       endif

        if (!__BITTST(dirs->bitopt, OPT_FORCE))
        {
            if (!pch_compare_file(dirs, &from_s, &to_s))
            {
                if (__ISLOG)
                {
                    pch_log_info(dirs, "stage #2 skip: %s -> %s", from_s.str, to_s.str);
                }
                continue;
            }
        }
        if (strchr(b, __PSEPC))
        {
            switch (pch_path_dir(&to_dir_s, &to_s))
            {
            case -1:
            {
                rcode = -2;
                break;
            }
            case  0:
            {
                rcode = 0;
                break;
            }
            default:
            {
                if (!hd->searchs(hd->hash, to_dir_s.str, to_dir_s.sz))
                {
                    (void) hd->addmap(hd->hash, to_dir_s.str, to_dir_s.sz);
                }
                if (!pch_check_dir(&to_dir_s))
                {
                    errno = 0;

                    if (
                        (_mkdir(to_dir_s.str) < 0) &&
                        (errno != EEXIST)
                    )
                    {
                        pch_log_error(dirs, "create directory error: %s", to_dir_s.str);
                        rcode = -3;
                        break;
                    }
                }
            }
            }
        }
        if (rcode < 0)
            break;

#       if defined(__DEBUG__)
        printf("%d) [%s]: %s -> %s -> %s\n",
               __LINE__, __func__, from_s.str, to_s.str,
               ((to_dir_s.str) ? to_dir_s.str : "- none -")
              );
#       endif

        if ((ret = pch_fcopy(&from_s, &to_s)))
        {
            pch_log_error(dirs, "copy file error: %s -> %s", from_s.str, to_s.str);
            continue;
        }
        if (__ISLOG)
        {
            pch_log_info(dirs, "stage #2 %s: %s -> %s",
                         ((__BITTST(dirs->bitopt, OPT_FORCE)) ? "replace" : "update"),
                         from_s.str, to_s.str
                        );
        }
        stage = ((!ret) ? (stage + 1) : stage);
    }

    if ((!rcode) && (stage > 0))
    {
        /* needed to add exist directory before */
        if (pch_vcs_add(dirs, &dirs->setup[FILE_SPLIT_REPO]) != 0)
        {
            pch_log_error(dirs, "add VCS root directory error: -> %s", dirs->setup[FILE_SPLIT_REPO].str);
        }
        /* add all files from directory */
        hd->getmap(hd->hash, (void*)dirs, &__stage2_vcs_add);
    }

    hf->free(hf->hash);
    free(hf);
    hd->free(hd->hash);
    free(hd);

    return ((rcode < 0) ? rcode : stage);
}

int pch_stage3(paths_t *dirs)
{
    int ret = 0;

#   if !defined(OS_WIN)

    if (__BITTST(dirs->bitopt, OPT_YAML))
    {
        do
        {
            int i, idx = -1;
            static string_s binsh[] = {
                { __YAMLSHELL1, __CSZ(__YAMLSHELL1) },
                { __YAMLSHELL2, __CSZ(__YAMLSHELL2) }
                };
            string_s fyaml = { NULL, 0U };
            string_s __AUTO(__autostring) *fy = &fyaml;

            for (i = 0; i < (int)__NELE(binsh); i++)
            {
                if ((binsh[i].sz) && (pch_check_file(&binsh[i])))
                {
                    idx = i;
                    break;
                }
            }
            if (idx == -1)
            {
                pch_log_info(dirs, "examine yaml config shell: [%d] - not found", idx);
                break;
            }
            if (
                (!pch_path_format(&fyaml, "%s" __PSEPS __YAMLNAME, dirs->setup[FILE_SPLIT_REPO].str)) ||
                (!pch_check_file(&fyaml))
            )
            {
                pch_log_error(dirs, "examine yaml config: [%s] - not found",
                              ((fyaml.str) ? fyaml.str : "<repo-split>" __PSEPS __YAMLNAME)
                             );
                break;
            }

            char brev[20] = {0};
            const char *args[] =
            {
                binsh[idx].str,
                "-c",
                yamlscr,
                dirs->setup[FILE_SPLIT_REPO].str,
                NULL,
                NULL,
                NULL
            };

            args[4] = pch_ultostr(brev, dirs->rev, 10);
            args[5] = pch_vcs_type(dirs->bitopt);

            if ((ret = pch_exec(dirs, args)))
            {
                pch_log_error(dirs, "examine yaml config return error: %d", ret);
                break;
            }
            else if (__ISLOG)
            {
                pch_log_info(dirs, "examine yaml config [%s" __PSEPS __YAMLNAME "] - OK",
                             dirs->setup[FILE_SPLIT_REPO].str
                            );
            }
        }
        while (0);
    }
#   endif
    if ((!ret) && (__BITTST(dirs->bitopt, OPT_DEPLOY)))
    {
        const char *scr = NULL;

        if (__ISLOG)
        {
            scr = strrchr(dirs->setup[FILE_DEPLOY].str, __PSEPC);
            pch_log_info(dirs, "deploy script [%s] - start:\n",
                         ((scr) ? (scr + 1) : dirs->setup[FILE_DEPLOY].str)
                        );
        }
        do
        {
            char brev[20] = {0};
            const char *args[] =
            {
                dirs->setup[FILE_DEPLOY].str,
                dirs->setup[FILE_SPLIT_REPO].str,
                NULL,
                NULL,
                NULL
            };

            args[2] = pch_ultostr(brev, dirs->rev, 10);
            args[3] = pch_vcs_type(dirs->bitopt);

            if ((ret = pch_exec(dirs, args)))
            {
                pch_log_error(dirs, "deploy script return error: %d -> %s", ret, dirs->setup[FILE_DEPLOY].str);
                break;
            }
            else if (__ISLOG)
            {
                pch_log_info(dirs, "deploy script [%s] - OK",
                             ((scr) ? (scr + 1) : dirs->setup[FILE_DEPLOY].str)
                            );
            }
        }
        while (0);
    }

    return ret;
}

int pch_stage4(paths_t *dirs)
{
    int ret;

    if ((ret = pch_vcs_add(dirs, &dirs->setup[FILE_SPLIT_REPO])) != 0)
    {
        pch_log_error(dirs, "add VCS root directory error: %d -> %s", ret, dirs->setup[FILE_SPLIT_REPO].str);
    }
    if ((ret = pch_vcs_commit(dirs)) != 0)
    {
        pch_log_error(dirs, "commit VCS error: %d", ret);
        return -1;
    }
    return 1;
}
