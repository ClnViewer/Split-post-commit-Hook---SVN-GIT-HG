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
#include "libs/include/xmlp.h"
#include "version.h"

typedef struct
{
    setup_options_e id;
    paths_t *data;
} optxml_a;

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

static setup_options_e __pch_option_parse(paths_t *dirs, int c, char *optarg)
{
    switch (c)
    {
    case 'm':
    {
        if (dirs->setup[FILE_MASTER_REPO].str)
            break;

        if (
            (!optarg) ||
            (!string_append_auto(&dirs->setup[FILE_MASTER_REPO], optarg)) ||
            (!pch_check_dir(&dirs->setup[FILE_MASTER_REPO]))
        )
            return ENUM_SETUP_master;
        break;
    }
    case 's':
    {
        if (dirs->setup[FILE_SPLIT_REPO].str)
            break;

        if (
            (!optarg) ||
            (!string_append_auto(&dirs->setup[FILE_SPLIT_REPO], optarg)) ||
            (!pch_check_dir(&dirs->setup[FILE_SPLIT_REPO]))
        )
            return ENUM_SETUP_split;
        break;
    }
    case 'l':
    {
        if (dirs->setup[FILE_FILELIST].str)
            break;

        if (
            (!optarg) ||
            (!string_format(&dirs->setup[FILE_FILELIST], "%s" __PSEPS "%s", dirs->setup[FILE_MASTER_REPO].str, optarg)) ||
            (!pch_check_file(&dirs->setup[FILE_FILELIST])) ||
            (!(dirs->fp[PATHS_FILE_LST] = fopen(dirs->setup[FILE_FILELIST].str, "r")))
        )
            return ENUM_SETUP_list;
        do
        {
            char cbuf[25] = {0}, *b = (char*)&cbuf;

            if (fread((void*)b, 1U, xmltag.sz, dirs->fp[PATHS_FILE_LST]) > 0)
            {
                (void) fseek(dirs->fp[PATHS_FILE_LST], 0L, SEEK_SET);

                /**< UTF8 BOM skipped: EF BB BF */
                if (
                    ((unsigned char)b[0] == 0xef) &&
                    ((unsigned char)b[1] == 0xbb) &&
                    ((unsigned char)b[2] == 0xbf)
                )
                {
                    b += 3;
                }
                if (memcmp((void*)b, (void*)xmltag.str, xmltag.sz) == 0)
                {
                    dirs->bitopt = __BITSET(dirs->bitopt, OPT_INPUT_XML);
                    break;
                }
            }
            dirs->bitopt = __BITSET(dirs->bitopt, OPT_INPUT_TXT);
        }
        while(0);
        break;
    }
    case 'o':
    {
        if (dirs->setup[FILE_ROOTVCS].str)
            break;

        if (
            (!optarg) ||
            (!string_append_auto(&dirs->setup[FILE_ROOTVCS], optarg)) ||
            (!pch_check_dir(&dirs->setup[FILE_ROOTVCS]))
        )
            return ENUM_SETUP_rootsvn;
        break;
    }
    case 'e':
    {
        if (dirs->setup[FILE_BINDIR].str)
            break;

        if (
            (!optarg) ||
            (!string_append_auto(&dirs->setup[FILE_BINDIR], optarg)) ||
            (!pch_check_dir(&dirs->setup[FILE_BINDIR]))
        )
            return ENUM_SETUP_execdir;
        break;
    }
    case 'j':
    {
        if (dirs->setup[FILE_FLOG].str)
            break;

        if (
            (!optarg) ||
            (!string_append_auto(&dirs->setup[FILE_FLOG], optarg)) ||
            (!(dirs->fp[PATHS_FILE_OUT] = fopen(dirs->setup[FILE_FLOG].str, "a+")))
        )
            return ENUM_SETUP_log;
        break;
    }
    case 'u':
    {
        if (dirs->setup[FILE_UUID].str)
            break;

        if (
            (!optarg) ||
            (!string_append_auto(&dirs->setup[FILE_UUID], optarg))
        )
            return ENUM_SETUP_uid;
        dirs->bitopt = __BITSET(dirs->bitopt, OPT_UUID);
        break;
    }
    case 'd':
    {
        if (dirs->setup[FILE_DEPLOY].str)
            break;

        if (
            (!optarg) ||
            (!string_append_auto(&dirs->setup[FILE_DEPLOY], optarg)) ||
            (!pch_check_file(&dirs->setup[FILE_DEPLOY]))
        )
            return ENUM_SETUP_deploy;
        dirs->bitopt = __BITSET(dirs->bitopt, OPT_DEPLOY);
        break;
    }
    case 'b':
    {
        if (dirs->setup[FILE_BACKUP].str)
            break;

        if (
            (!optarg) ||
            (!string_append_auto(&dirs->setup[FILE_BACKUP], optarg)) ||
            (!pch_check_dir(&dirs->setup[FILE_BACKUP]))
        )
            return ENUM_SETUP_backup;
        dirs->bitopt = __BITSET(dirs->bitopt, OPT_BACKUP);
        break;
    }
    case 'x':
    {
        char *ren2;

        if (dirs->setup[FILE_RENAME1].str)
            break;

        if (!optarg)
            return ENUM_SETUP_rename;

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
            return ENUM_SETUP_vcs;
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
        if (dirs->rev)
            break;

        if (
            (!optarg) ||
            (!(dirs->rev = strtoul(optarg, NULL, 10)))
        )
            return ENUM_SETUP_revision;
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
        return ENUM_SETUP_info;
    }
    case 'h':
    {
        return ENUM_SETUP_help;
    }
    default:
    {
        fprintf(stdout, "\n\tUnknown option: '%c'\n\n", c);
        return (ENUM_SETUP_RETURN_OK + 2);
    }
    }
    return ENUM_SETUP_RETURN_OK;
}

static int __cb_opentag(void *v, int depth, char *name)
{
    unsigned i;
    optxml_a *data = (optxml_a*)v;
    data->id = ENUM_SETUP_RETURN_OK;

    (void) depth;

    /* skip help and info  non XML configuration*/
    for (i = 0U; i < (__NELE(options) - 1); i++)
    {
        switch ((setup_options_e)i)
        {
        case ENUM_SETUP_help:
        case ENUM_SETUP_info:
        case ENUM_SETUP_list:
        case ENUM_SETUP_master:
            continue;
        default:
        {
            break;
        }
        }
        if (!strncmp(name, options[i].name, strlen(options[i].name)))
        {
            data->id = (setup_options_e)i;
            break;
        }
    }
    if (data->id == ENUM_SETUP_RETURN_OK)
    {
        if (!strncmp(name, "name", 4U))
        {
            data->id = ENUM_SETUP_EXT_NAME;
        }
        else if (!strncmp(name, "date", 4U))
        {
            data->id = ENUM_SETUP_EXT_DATE;
        }
    }
    return 0;
}

static int __cb_text(void *v, int depth, char *text)
{
    optxml_a *data = (optxml_a*)v;
    size_t sz = strlen(text);
    (void) depth;

    do
    {
        if (!sz)
            break;

        switch (data->id)
        {
        case ENUM_SETUP_RETURN_OK:
            break;
        case ENUM_SETUP_EXT_NAME:
        {
            string_free(&data->data->setup[FILE_MASTER_NAME]);
            if (!string_append(&data->data->setup[FILE_MASTER_NAME], text, sz))
                return -1;
            break;
        }
        case ENUM_SETUP_EXT_DATE:
        {
            /* TODO (clanc#1#): add to dirs_t structure */
            char b[100] = {0};
            struct tm *tms;
            time_t t = (time_t)strtoul(text, NULL, 10);

            if ((unsigned long)t == 0UL)
                break;

            tms = localtime(&t);
            if (!tms)
                break;

            (void) strftime(b, 100, "%d.%m.%Y %H:%M", tms);
            pch_log_info(
                data->data,
                "found XML configuration date: %s",
                b
            );
            break;
        }
        case ENUM_SETUP_yaml:
        case ENUM_SETUP_force:
        case ENUM_SETUP_nonloop:
        case ENUM_SETUP_quiet:
        {
            if ((sz == 3U) && (!strncasecmp(text, "yes", 3U)))
            {
                (void) __pch_option_parse(data->data, options[(int)data->id].val, NULL);
            }
            break;
        }
        default:
        {
            (void) __pch_option_parse(data->data, options[(int)data->id].val, text);
            break;
        }
        }
    }
    while (0);

    data->id = ENUM_SETUP_RETURN_OK;
    return 0;
}

static bool_t __opt_xmlsetup(paths_t *dirs)
{
    int ret;
    optxml_a oxml;
    memset(&oxml, 0, sizeof(optxml_a));
    oxml.id = ENUM_SETUP_RETURN_OK;
    oxml.data = dirs;

    ret = xmlpf(dirs->fp[PATHS_FILE_LST],
                &oxml,
                __cb_opentag,
                NULL,
                NULL,
                NULL,
                __cb_text
               );

    (void) fseek(dirs->fp[PATHS_FILE_LST], 0L, SEEK_SET);

    return ((ret == -1) ? R_NEGATIVE :
            ((!ret) ? R_TRUE : R_FALSE)
           );
}

int pch_option(paths_t *dirs, char *argv[], int argc)
{
    int idx = 0;

    while (1)
    {
        int c;
        setup_options_e ret;
        if ((c = getopt_long(argc, argv, "m:s:l:o:e:j:u:x:d:b:t:c:r:g:yfkqih", options, &idx)) == -1)
            break;

        switch ((ret = __pch_option_parse(dirs, c, optarg)))
        {
        case ENUM_SETUP_RETURN_OK:
        {
            break;
        }
        case ENUM_SETUP_info:
        {
            fprintf(stdout, "\n%s\n", info);
            exit (0);
        }
        case ENUM_SETUP_help:
        {
            __help_prn(argv[0]);
            exit (0);
        }
        default:
        {
            if (ret < ENUM_SETUP_RETURN_OK)
            {
                __param_err(options[(unsigned)ret].val, options[(unsigned)ret].name, help[(unsigned)ret]);
            }
            return (int) ret;
        }
        }
    }

    if (optind < argc)
    {
        fprintf(stdout, "\n\tBad command line parameters:\n");
        while (optind < argc)
            fprintf(stdout, "\t\t%s\n", argv[optind++]);
        fprintf(stdout, "\n");
    }
    if ((__BITTST(dirs->bitopt, OPT_INPUT_XML)) && (dirs->fp[PATHS_FILE_LST]))
    {
        if (__opt_xmlsetup(dirs) != R_TRUE)
        {
            fprintf(stdout, "\n\tXML configuration list parse error: %s\n", dirs->setup[FILE_FILELIST].str);
            return (ENUM_SETUP_RETURN_OK + 3);
        }
    }
    if (
        (!dirs->setup[FILE_MASTER_REPO].str) ||
        (!dirs->setup[FILE_SPLIT_REPO].str) ||
        (!dirs->setup[FILE_FILELIST].str)
    )
    {
        fprintf(stdout, "\n\tRequired options: '-m ..', '-s ..', '-l ..'\n\n");
        return (ENUM_SETUP_RETURN_OK + 4);
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
        return (ENUM_SETUP_RETURN_OK + 5);
    }
    if (
        (!__BITTST(dirs->bitopt, OPT_VCS_SVN)) &&
        (!__BITTST(dirs->bitopt, OPT_VCS_GIT)) &&
        (!__BITTST(dirs->bitopt, OPT_VCS_HG))
    )
    {
        fprintf(stdout, "\n\tRequired options type VCS: '-t svn | git | hg'\n\n");
        return (ENUM_SETUP_RETURN_OK + 6);
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

    if (!dirs->setup[FILE_MASTER_NAME].str)
    {
        do
        {
            char *c = strrchr(dirs->setup[FILE_MASTER_REPO].str, __PSEPC);
            c = ((c == NULL) ? dirs->setup[FILE_MASTER_REPO].str : (c + 1));

            if (!string_append_auto(&dirs->setup[FILE_MASTER_NAME], c))
            {
                __param_err(options[FILE_MASTER_REPO].val, options[FILE_MASTER_REPO].name, help[FILE_MASTER_REPO]);
            }
        }
        while (0);
    }

    startedlog(dirs, argv[0]);
    return 0;
}
