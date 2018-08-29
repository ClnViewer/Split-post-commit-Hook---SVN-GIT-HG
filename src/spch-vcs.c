#include "spch.h"
#include "version.h"

// Version Control System

#if defined(OS_WIN)
#   define __BINEXT ".exe"
#else
#   define __BINEXT
#endif

typedef enum
{
    VCS_BIN_ACT,
    VCS_BIN_ADMIN
} vcs_bin_e;

typedef struct
{
    char *update[12];
    char *commit[12];
    char *push[12];
    char *add[12];
    char *checkout[12];
    char *admin[12];
} vcs_t;

/*!
    \todo (ClnViewer#1#08/26/18): Check another VCS command line options
    http://hyperpolyglot.org/version-control#HG-NAME
    https://www.perforce.com/blog/list-of-equivalent-commands-in-git-mercurial-and-svn
 */

static vcs_t __vcs[3] =
{
    [0] = {
        .update = {
            NULL, "update", "--ignore-externals", "--quiet", NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
        },
        .commit = {
            NULL, "commit", "--force-log", NULL, "--quiet", NULL, NULL, NULL, NULL, NULL, NULL, NULL
        },
        .push = {
            NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
        },
        .add = {
            NULL, "add", "--force", ".", "--auto-props", "--non-interactive", "--depth", "infinity", "--quiet", NULL, NULL, NULL
        },
        .checkout = {
            NULL, "checkout", "--ignore-externals", "--quiet", NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
        },
        .admin  = {
            NULL, "create", NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
        }
    },
    [1] = {
        .update = {
            NULL, "checkout", NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
        },
        .commit = {
            NULL, "commit", "--amend", NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
        },
        .push = {
            NULL, "push", "origin", NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
        },
        .add = {
            NULL, "add", NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
        },
        .checkout = {
            NULL, "checkout", NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
        },
        .admin  = {
            NULL, "init", "--bare", NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
        }
    },
    [2] = {
        .update = {
            NULL, "update", NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
        },
        .commit = {
            NULL, "commit", "--amend", NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
        },
        .push = {
            NULL, "push", " -r", ".", NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
        },
        .add = {
            NULL, "add", NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
        },
        .checkout = {
            NULL, "checkout", NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
        },
        .admin  = {
            NULL, "init", NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
        }
    }
};

static const char *vcs_bin[][2] =
{
    { "svn" __BINEXT, "svnadmin" __BINEXT },
    { "git" __BINEXT, "git" __BINEXT },
    { "hq"  __BINEXT, "hq" __BINEXT },
};

static int __select_vcs_type(unsigned long bitopt)
{
    return (
               ((__BITTST(bitopt, OPT_VCS_HQ)) ? OPT_VCS_HQ :
                ((__BITTST(bitopt, OPT_VCS_GIT)) ? OPT_VCS_GIT : OPT_VCS_SVN
                )));
}
static const char * __select_vcs_bin(paths_t *dirs, vcs_bin_e type)
{
    if (dirs->bins[type].str)
        return (const char*)dirs->bins[type].str;

    if (!pch_path_format(
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

int pch_vcs_update(paths_t *dirs)
{
    if (_chdir(dirs->setup[FILE_SPLIT_REPO].str) < 0)
    {
        return -1;
    }
    const char** args = (const char**)__vcs[__select_vcs_type(dirs->bitopt)].update;
    if (!(args[0] = __select_vcs_bin(dirs, VCS_BIN_ACT)))
        return -1;

    return pch_exec(dirs, (const char**)__vcs[__select_vcs_type(dirs->bitopt)].update);
}

int pch_vcs_commit(paths_t *dirs)
{
    int ret = -1;
    string_s arg3;
    string_s __AUTO(__autostring) *arg_free = &arg3;
    memset(&arg3, 0, sizeof(string_s));

    if (_chdir(dirs->setup[FILE_SPLIT_REPO].str) < 0)
    {
        return ret;
    }
    do
    {
        const char** args = (const char**)__vcs[__select_vcs_type(dirs->bitopt)].commit;
        if (!(args[0] = __select_vcs_bin(dirs, VCS_BIN_ACT)))
            break;

        if (dirs->rev)
        {
            if (!pch_path_format(
                        &arg3,
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
            if (!pch_path_format(
                        &arg3,
                        "-m\"split auto commit (spch v.%s)\"",
                        SPCH_FULLVERSION_STRING
                    )
               )
            {
                break;
            }
        }
        args[3] = arg3.str;
        ret = pch_exec(dirs, args);

        if (ret) break;

        args = (const char**)__vcs[__select_vcs_type(dirs->bitopt)].push;

        if ((!args[1]) || (!(args[0] = __select_vcs_bin(dirs, VCS_BIN_ACT))))
            break;

        ret = pch_exec(dirs, args);
    }
    while (0);

    return ret;
}

int pch_vcs_aad(paths_t *dirs, string_s *dir)
{
    if (_chdir(dir->str) < 0)
    {
        return -1;
    }
    const char** args = (const char**)__vcs[__select_vcs_type(dirs->bitopt)].add;
    if (!(args[0] = __select_vcs_bin(dirs, VCS_BIN_ACT)))
        return -1;

    return pch_exec(dirs, args);
}

int pch_vcs_create(paths_t *dirs)
{
    int ret = -1;
    string_s arg2;
    string_s __AUTO(__autostring) *arg_free = &arg2;
    memset(&arg2, 0, sizeof(string_s));

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

        if (!pch_path_format(
                    &arg2,
                    "%s" __PSEPS "%s",
                    dirs->setup[FILE_ROOTVCS].str,
                    mhome
                )
           )
        {
            break;
        }
        args[2] = arg2.str;
        if ((ret = pch_exec(dirs, args)))
            return ret;
    }
    while (0);

    return ret;
}
