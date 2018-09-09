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
#include <getopt.h>
#include "spch-opt.h"
#include "version.h"

static void __help_prn(const char *exename)
{
    size_t n;
    const char *pname = strrchr(exename, __PSEPC);
    pname = ((!pname) ? exename : (pname + 1));

    fprintf(stdout,
            "\n  Split post commit hook to Version Control System SVN/GIT/HG" \
            "\n  Build: %s(%s) %s" \
            "\n  (c)PS: https://github.com/ClnViewer/Split-post-commit-Hook---SVN-GIT-HG\n" \
            "\n  %s - Options:\n",
            SPCH_FULLVERSION_STRING,
            SPCH_STATUS_SHORT,
            SPCH_UBUNTU_VERSION_STYLE,
            pname
           );
    for (n = 0; n < __NELE(options); n++)
    {
        if (!options[n].name)
            continue;
        fprintf(stdout, "\t -%c, --%s %s\t%s\n",
                options[n].val, options[n].name,
                ((options[n].has_arg == required_argument) ? "'val'" : "     "),
                help[n]
               );
    }
    fprintf(stdout,
            "\n  Example:\n\n" \
            "\t%s \\\n\t  -t svn -m /path/master/repo -s /path/split/repo -l build/FileList.txt\n",
            pname
           );
}

static void __param_err(int c, const char *s, const char *h)
{
    fprintf(stdout, "\n\t-%c or --%s valid parameters required: %s\n\n",
            (char)c, s, h
           );
}

void startedlog(paths_t *dirs, const char *exename)
{
    char b[101] = {0};
    const char *pname = strrchr(exename, __PSEPC);
    pname = ((!pname) ? exename : (pname + 1));

    (void) pch_path_time(b, "%x-%I:%M%p");

    pch_log_info(
        dirs,
        "%s v.%s(%s) %s",
        pname,
        SPCH_FULLVERSION_STRING,
        SPCH_STATUS_SHORT,
        SPCH_UBUNTU_VERSION_STYLE
    );
    pch_log_info(
        dirs,
        "started at %s",
        b
    );
}

void endedlog(paths_t *dirs)
{
    char b[101] = {0};
    (void) pch_path_time(b, "%x-%I:%M%p");

    pch_log_info(
        dirs,
        "ended at %s",
        b
    );
}

const char * pch_option_chkmode(paths_t *dirs)
{
    int chk = (__BITTST(dirs->bitopt, OPT_FCHECK_CTIME)) * 1  // 1,4.6,9
            + (__BITTST(dirs->bitopt, OPT_FCHECK_MTIME)) * 3  // 3,4,8,9
            + (__BITTST(dirs->bitopt, OPT_FCHECK_SIZE))  * 5; // 5,6,8,9

    switch (chk)
    {
    case 1:
        return "create time";
    case 3:
        return "modify time";
    case 4:
        return "create, modify time";
    case 5:
        return "size";
    case 6:
        return "create time and size";
    case 8:
        return "modify time and size";
    case 9:
        return "create, modify time and size";
    default:
        return "none";
    }
}

int pch_option(paths_t *dirs, char *argv[], int argc)
{
    int idx = 0;

    while (1)
    {
        int c;
        if ((c = getopt_long(argc, argv, "m:s:l:o:e:j:u:x:d:t:c:r:g:yfkqih", options, &idx)) == -1)
            break;

        switch (c)
        {
        case 'm':
        {
            if (
                (!optarg) ||
                (!string_append_auto(&dirs->setup[FILE_MASTER_REPO], optarg)) ||
                (!pch_check_dir(&dirs->setup[FILE_MASTER_REPO]))
            )
            {
                __param_err(options[FILE_MASTER_REPO].val, options[FILE_MASTER_REPO].name, help[FILE_MASTER_REPO]);
                return (FILE_MASTER_REPO + 1);
            }
            break;
        }
        case 's':
        {
            if (
                (!optarg) ||
                (!string_append_auto(&dirs->setup[FILE_SPLIT_REPO], optarg)) ||
                (!pch_check_dir(&dirs->setup[FILE_SPLIT_REPO]))
            )
            {
                __param_err(options[FILE_SPLIT_REPO].val, options[FILE_SPLIT_REPO].name, help[FILE_SPLIT_REPO]);
                return (FILE_SPLIT_REPO + 1);
            }
            break;
        }
        case 'l':
        {
            if (
                (!optarg) ||
                (!string_format(&dirs->setup[FILE_FILELIST], "%s" __PSEPS "%s", dirs->setup[FILE_MASTER_REPO].str, optarg)) ||
                (!pch_check_file(&dirs->setup[FILE_FILELIST])) ||
                (!(dirs->fp[PATHS_FILE_LST] = fopen(dirs->setup[FILE_FILELIST].str, "r")))
            )
            {
                __param_err(options[FILE_FILELIST].val, options[FILE_FILELIST].name, help[FILE_FILELIST]);
                return (FILE_FILELIST + 1);
            }
            break;
        }
        case 'o':
        {
            if (
                (!optarg) ||
                (!string_append_auto(&dirs->setup[FILE_ROOTVCS], optarg)) ||
                (!pch_check_dir(&dirs->setup[FILE_ROOTVCS]))
            )
            {
                __param_err(options[FILE_ROOTVCS].val, options[FILE_ROOTVCS].name, help[FILE_ROOTVCS]);
                return (FILE_ROOTVCS + 1);
            }
            break;
        }
        case 'e':
        {
            if (
                (!optarg) ||
                (!string_append_auto(&dirs->setup[FILE_BINDIR], optarg)) ||
                (!pch_check_dir(&dirs->setup[FILE_BINDIR]))
            )
            {
                __param_err(options[FILE_BINDIR].val, options[FILE_BINDIR].name, help[FILE_BINDIR]);
                return (FILE_BINDIR + 1);
            }
            break;
        }
        case 'j':
        {
            if (
                (!optarg) ||
                (!string_append_auto(&dirs->setup[FILE_FLOG], optarg)) ||
                (!(dirs->fp[PATHS_FILE_OUT] = fopen(dirs->setup[FILE_FLOG].str, "a+")))
            )
            {
                __param_err(options[FILE_FLOG].val, options[FILE_FLOG].name, help[FILE_FLOG]);
                return (FILE_FLOG + 1);
            }
            break;
        }
        case 'u':
        {
            if (
                (!optarg) ||
                (!string_append_auto(&dirs->setup[FILE_UUID], optarg))
            )
            {
                __param_err(options[FILE_UUID].val, options[FILE_UUID].name, help[FILE_UUID]);
                return (FILE_UUID + 1);
            }
            dirs->bitopt = __BITSET(dirs->bitopt, OPT_UUID);
            break;
        }
        case 'x':
        {
            char *ren2;

            if (!optarg)
            {
                __param_err(options[FILE_RENAME1].val, options[FILE_RENAME1].name, help[FILE_RENAME1]);
                return (FILE_RENAME1 + 1);
            }
            if ((ren2 = strchr(optarg, '=')))
            {
                char *ren1 = optarg;
                int   sz = (ren2 - optarg);
                ren2++;

                if (
                    (sz > 0) &&
                    (string_append(&dirs->setup[FILE_RENAME1], ren1, (size_t)sz)) &&
                    (string_append_auto(&dirs->setup[FILE_RENAME2], ren2))
                )
                    dirs->bitopt = __BITSET(dirs->bitopt, OPT_RENAME);
            }
            else
            {
                if (string_append_auto(&dirs->setup[FILE_RENAME1], optarg))
                {
                    dirs->bitopt = __BITSET(dirs->bitopt, OPT_PREFIX);
                }
            }
            break;
        }
        case 'd':
        {
            if (
                (!optarg) ||
                (!string_append_auto(&dirs->setup[FILE_DEPLOY], optarg)) ||
                (!pch_check_file(&dirs->setup[FILE_DEPLOY]))
            )
            {
                __param_err(options[FILE_DEPLOY].val, options[FILE_DEPLOY].name, help[FILE_DEPLOY]);
                return (FILE_DEPLOY + 1);
            }
            dirs->bitopt = __BITSET(dirs->bitopt, OPT_DEPLOY);
            break;
        }
        case 't':
        {
            if (!optarg)
                break;
            switch (optarg[0])
            {
            case 's' :
                dirs->bitopt = __BITSET(dirs->bitopt, OPT_VCS_SVN);
                break;
            case 'g' :
                dirs->bitopt = __BITSET(dirs->bitopt, OPT_VCS_GIT);
                break;
            case 'h' :
                dirs->bitopt = __BITSET(dirs->bitopt, OPT_VCS_HG);
                break;
            default:
                __param_err(options[FILE_NONE_IDX].val, options[FILE_NONE_IDX].name, help[FILE_NONE_IDX]);
                return (FILE_NONE_IDX + 1);
            }
            break;
        }
        case 'c':
        {
            size_t i;

            if (!optarg)
            {
                break;
            }
            for (i = 0; i < strlen(optarg); i++)
            {
                switch (optarg[i])
                {
                case 'a' :
                    dirs->bitopt = __BITSET(
                                       __BITSET(
                                           __BITSET(dirs->bitopt,
                                                    OPT_FCHECK_CTIME),
                                           OPT_FCHECK_SIZE),
                                       OPT_FCHECK_MTIME);
                    break;
                case 'd' :
                    dirs->bitopt = __BITSET(
                                       __BITSET(dirs->bitopt,
                                                OPT_FCHECK_SIZE),
                                       OPT_FCHECK_MTIME);
                    break;
                case 'c' :
                    dirs->bitopt = __BITSET(dirs->bitopt, OPT_FCHECK_CTIME);
                    break;
                case 'm' :
                    dirs->bitopt = __BITSET(dirs->bitopt, OPT_FCHECK_MTIME);
                    break;
                case 's' :
                    dirs->bitopt = __BITSET(dirs->bitopt, OPT_FCHECK_SIZE);
                    break;
                default:
                    break;
                }
            }
            break;
        }
        case 'r':
        {
            if (
                (!optarg) ||
                (!(dirs->rev = strtoul(optarg, NULL, 10)))
            )
            {
                __param_err(options[(FILE_NONE_IDX + 1)].val, options[(FILE_NONE_IDX + 1)].name, help[(FILE_NONE_IDX + 1)]);
                return (FILE_NONE_IDX + 2);
            }
            break;
        }
        case 'g':
        {
            if (!optarg)
            {
                dirs->bitopt = __BITSET(dirs->bitopt, OPT_CHLOG_GNU);
                break;
            }
            switch (optarg[0])
            {
            case 'm' :
                dirs->bitopt = __BITSET(dirs->bitopt, OPT_CHLOG_MD);
                break;
            case 'g' :
                dirs->bitopt = __BITSET(dirs->bitopt, OPT_CHLOG_GNU);
                break;
            default:
                break;
            }
            break;
        }
        case 'y':
        {
            dirs->bitopt = __BITSET(dirs->bitopt, OPT_YAML);
            break;
        }
        case 'f':
        {
            dirs->bitopt = __BITSET(dirs->bitopt, OPT_FORCE);
            break;
        }
        case 'k':
        {
            dirs->bitopt = __BITSET(dirs->bitopt, OPT_DEMONIZE);
            break;
        }
        case 'q':
        {
            dirs->bitopt = __BITSET(dirs->bitopt, OPT_QUIET);
            break;
        }
        case 'i':
        {
            fprintf(stdout, "\n%s\n", info);
            exit(0);
        }
        case 'h':
        {
            __help_prn(argv[0]);
            exit(0);
        }
        default:
        {
            fprintf(stdout, "\n\tUnknown option: '%c'\n\n", c);
            return (FILE_NONE_IDX + 3);
        }
        }
    }
    if (optind < argc)
    {
        printf("\n\tBad command line parameters:\n");
        while (optind < argc)
            printf("\t\t%s\n", argv[optind++]);
        printf("\n");
    }
    if (
        (!dirs->setup[FILE_MASTER_REPO].str) ||
        (!dirs->setup[FILE_SPLIT_REPO].str) ||
        (!dirs->setup[FILE_FILELIST].str)
    )
    {
        fprintf(stdout, "\n\tRequired options: '-m ..', '-s ..', '-l ..'\n\n");
        return (FILE_NONE_IDX + 4);
    }

#   if !defined(OS_WIN)
    if (!dirs->setup[FILE_BINDIR].str)
    {
        (void) string_append_auto(&dirs->setup[FILE_BINDIR], "/usr/bin");
    }
#   endif

    if (
        (!dirs->setup[FILE_BINDIR].str) ||
        (!pch_vcs_bincheck(dirs))
    )
    {
        fprintf(stdout, "\n\tRequired options exec VCS bin directory: '-e ..'\n\n");
        return (FILE_NONE_IDX + 5);
    }
    if (
        (!__BITTST(dirs->bitopt, OPT_VCS_SVN)) &&
        (!__BITTST(dirs->bitopt, OPT_VCS_GIT)) &&
        (!__BITTST(dirs->bitopt, OPT_VCS_HG))
    )
    {
        fprintf(stdout, "\n\tRequired options type VCS: '-t svn | git | hg'\n\n");
        return (FILE_NONE_IDX + 6);
    }
    if (
        (!__BITTST(dirs->bitopt, OPT_FCHECK_CTIME)) &&
        (!__BITTST(dirs->bitopt, OPT_FCHECK_MTIME)) &&
        (!__BITTST(dirs->bitopt, OPT_FCHECK_SIZE))
    )
    {
        dirs->bitopt = __BITSET(
                           __BITSET(dirs->bitopt,
                                    OPT_FCHECK_SIZE),
                           OPT_FCHECK_MTIME);
    }

    startedlog(dirs, argv[0]);
    return 0;
}
