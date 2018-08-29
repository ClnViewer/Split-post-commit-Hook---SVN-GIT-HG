#include "spch.h"
#include <getopt.h>
#include "version.h"

static char *help[] =
{
    "master repository, from copy",
    "split repository, to copy",
    "file list name, search in master repository",
    "root directory to SVN/GIT/HG repository",
    "binary execute VCS directory",
    "journal log path include file name",
    "run as 'user' privilege (Linux only)",
    "rename files, prefix example 'new' or 'old=new'",
    "run deploy script before commit",
    "VCS type: [svn|git|hq]",
    "file check: [mtime|ctime|size|all]",
    "current VCS revision",
    "force overwrite all destination files",
    "quiet mode, no print message to console",
    "this help page",
    NULL
};

static struct option options[] =
{
    { "master",   required_argument,  NULL, 'm' },
    { "split",    required_argument,  NULL, 's' },
    { "list",     required_argument,  NULL, 'l' },
    { "rootsvn",  required_argument,  NULL, 'o' },
    { "execdir",  required_argument,  NULL, 'e' },
    { "log",      required_argument,  NULL, 'j' },
    { "suid",     required_argument,  NULL, 'u' },
    { "rename",   required_argument,  NULL, 'x' },
    { "deploy",   required_argument,  NULL, 'd' },
    { "vcs",      required_argument,  NULL, 't' },
    { "check",    required_argument,  NULL, 'c' },
    { "revision", required_argument,  NULL, 'r' },
    { "force",    no_argument,        NULL, 'f' },
    { "quiet",    no_argument,        NULL, 'q' },
    { "help",     no_argument,        NULL, 'h' },
    { 0, 0, 0, 0 }
};

static void __help_prn(const char *exename)
{
    size_t n;
    const char *pname = strrchr(exename, __PSEPC);
    pname = ((!pname) ? exename : (pname + 1));

    fprintf(stdout,
            "\n  Split post commit hook to Version Control System SVN/GIT/HG" \
            "\n  Build: %s(%s) %s" \
            "\n  (c)PS: https://github.com/ClnViewer/spch\n" \
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
    return ((__BITTST(dirs->bitopt, OPT_FCHECK_CTIME)) ?
            "create time" :
            ((__BITTST(dirs->bitopt, OPT_FCHECK_MTIME)) ?
             "modify time" :
             ((__BITTST(dirs->bitopt, OPT_FCHECK_SIZE)) ?
              "size" :
              ((__BITTST(dirs->bitopt, OPT_FCHECK_ALL)) ?
               "all - create, modify time and size" : "-none-"
              )
             )
            )
           );
}


int pch_option(paths_t *dirs, char *argv[], int argc)
{
    int idx = 0;

    while (1)
    {
        int c;
        if ((c = getopt_long(argc, argv, "m:s:l:o:e:j:u:x:d:t:c:r:fqh", options, &idx)) == -1)
            break;

        switch (c)
        {
        case 'm':
        {
            if (
                (!optarg) ||
                (!pch_path_dump(&dirs->setup[FILE_MASTER_REPO], optarg)) ||
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
                (!pch_path_dump(&dirs->setup[FILE_SPLIT_REPO], optarg)) ||
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
                (!pch_path_format(&dirs->setup[FILE_FILELIST], "%s" __PSEPS "%s", dirs->setup[FILE_MASTER_REPO].str, optarg)) ||
                (!pch_check_file(&dirs->setup[FILE_FILELIST])) ||
                (!(dirs->fp[0] = fopen(dirs->setup[FILE_FILELIST].str, "r")))
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
                (!pch_path_dump(&dirs->setup[FILE_ROOTVCS], optarg)) ||
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
                (!pch_path_dump(&dirs->setup[FILE_BINDIR], optarg)) ||
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
                (!pch_path_dump(&dirs->setup[FILE_FLOG], optarg)) ||
                (!(dirs->fp[1] = fopen(dirs->setup[FILE_FLOG].str, "a+")))
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
                (!pch_path_dump(&dirs->setup[FILE_UUID], optarg))
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
                ren1[sz] = '\0';

                if (
                    (pch_path_dump(&dirs->setup[FILE_RENAME1], ren1)) &&
                    (pch_path_dump(&dirs->setup[FILE_RENAME2], ren2))
                )
                    dirs->bitopt = __BITSET(dirs->bitopt, OPT_RENAME);
            }
            else
            {
                if (pch_path_dump(&dirs->setup[FILE_RENAME1], optarg))
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
                (!pch_path_dump(&dirs->setup[FILE_DEPLOY], optarg)) ||
                (!pch_check_dir(&dirs->setup[FILE_DEPLOY]))
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
                dirs->bitopt = __BITSET(dirs->bitopt, OPT_VCS_HQ);
                break;
            default:
                __param_err(options[FILE_NONE_IDX].val, options[FILE_NONE_IDX].name, help[FILE_NONE_IDX]);
                return (FILE_NONE_IDX + 1);
            }
            break;
        }
        case 'c':
        {
            if (!optarg)
                break;
            switch (optarg[0])
            {
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
                dirs->bitopt = __BITSET(dirs->bitopt, OPT_FCHECK_ALL);
                break;
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
        case 'f':
        {
            dirs->bitopt = __BITSET(dirs->bitopt, OPT_FORCE);
            break;
        }
        case 'q':
        {
            dirs->bitopt = __BITSET(dirs->bitopt, OPT_QUIET);
            break;
        }
        case 'h':
        {
            __help_prn(argv[0]);
            return 127;
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
        (void) pch_path_dump(&dirs->setup[FILE_BINDIR], "/usr/bin");
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
        (!__BITTST(dirs->bitopt, OPT_VCS_HQ))
    )
    {
        fprintf(stdout, "\n\tRequired options type VCS: '-t svn | git | hq'\n\n");
        return (FILE_NONE_IDX + 6);
    }
    if (
        (!__BITTST(dirs->bitopt, OPT_FCHECK_CTIME)) &&
        (!__BITTST(dirs->bitopt, OPT_FCHECK_MTIME)) &&
        (!__BITTST(dirs->bitopt, OPT_FCHECK_SIZE)) &&
        (!__BITTST(dirs->bitopt, OPT_FCHECK_ALL))
    )
    {
        dirs->bitopt = __BITSET(dirs->bitopt, OPT_FCHECK_ALL);
    }

    startedlog(dirs, argv[0]);
    return 0;
}
