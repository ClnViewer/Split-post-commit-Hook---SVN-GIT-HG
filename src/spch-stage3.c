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


bool_t pch_stage3(paths_t *dirs)
{
    int ret = 0;

#   if !defined(OS_WIN)
    if (__BITTST(dirs->bitopt, OPT_YAML))
    {
        do
        {
            int i, idx = -1;
            static string_s binsh[] =
            {
                { __YAMLSHELL1, __CSZ(__YAMLSHELL1) },
                { __YAMLSHELL2, __CSZ(__YAMLSHELL2) }
            };
            string_s byaml = { NULL, 0U };
            string_s __AUTO(__autostring) *fyaml = &byaml;

#           if defined(BUILD_MSVC)
            __try
            {
#           endif

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
                    (!string_format(fyaml, "%s" __PSEPS __YAMLNAME, dirs->setup[FILE_SPLIT_REPO].str)) ||
                    (!pch_check_file(fyaml))
                )
                {
                    pch_log_error(dirs, "examine yaml config: [%s] - not found",
                                  ((fyaml->str) ? fyaml->str : "<repo-split>" __PSEPS __YAMLNAME)
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

                if ((ret = pch_exec(dirs, args, NULL)))
                {
                    pch_log_error(dirs, "examine yaml config return error: %d", ret);
                    break;
                }
                else if (__ISLOGP)
                {
                    pch_log_info(dirs, "examine yaml config [%s" __PSEPS __YAMLNAME "] - OK",
                                 dirs->setup[FILE_SPLIT_REPO].str
                                );
                }
#           if defined(BUILD_MSVC)
            }
            __finally
            {
                if (fyaml->str)
                {
                    string_free(fyaml);
                }
            }
#           endif
        }
        while (0);
    }
#   endif
    if ((!ret) && (__BITTST(dirs->bitopt, OPT_DEPLOY)))
    {
        const char *scr = NULL;

        if (__ISLOGP)
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

            if ((ret = pch_exec(dirs, args, NULL)))
            {
                pch_log_error(dirs, "deploy script return error: %d -> %s", ret, dirs->setup[FILE_DEPLOY].str);
                break;
            }
            else if (__ISLOGP)
            {
                pch_log_info(dirs, "deploy script [%s] - OK",
                             ((scr) ? (scr + 1) : dirs->setup[FILE_DEPLOY].str)
                            );
            }
        }
        while (0);
    }
    if ((!ret) && ((__BITTST(dirs->bitopt, OPT_CHLOG_MD)) || (__BITTST(dirs->bitopt, OPT_CHLOG_GNU))))
    {
        switch (pch_vcs_xmllog(dirs))
        {
        case R_NEGATIVE:
            {
                pch_log_error(dirs, "split repo ChangeLog update %s", "error");
                break;
            }
        case R_FALSE:
            {
                pch_log_info(dirs, "split repo ChangeLog %s update", "not");
                break;
            }
        case R_TRUE:
            {
                pch_log_info(dirs, "split repo ChangeLog update %s", "successfully");
                break;
            }
        default:
            {
                break;
            }
        }
    }
    return ((!ret) ? R_TRUE : R_FALSE);
}
