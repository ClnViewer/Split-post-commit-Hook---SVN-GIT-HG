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

#if !defined(SPCH_VCS_H)
#define SPCH_VCS_H

/* Version Control System */

#if defined(OS_WIN)
#   define __BINEXT ".exe"
#else
#   define __BINEXT
#endif

#if defined(BUILD_MSVC)
#   define __SINIT(x, ...) __VA_ARGS__
#else
#   define __SINIT(x, ...) x = __VA_ARGS__
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
    {
        __SINIT(.update, {
            NULL, "update", "--ignore-externals", "--quiet", NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
        }),
        __SINIT(.commit, {
            NULL, "commit", "--force-log", NULL, "--quiet", NULL, NULL, NULL, NULL, NULL, NULL, NULL
        }),
        __SINIT(.push, {
            NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
        }),
        __SINIT(.add, {
            NULL, "add", "--force", ".", "--auto-props", "--non-interactive", "--depth", "infinity", "--quiet", NULL, NULL, NULL
        }),
        __SINIT(.checkout, {
            NULL, "checkout", "--ignore-externals", "--quiet", NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
        }),
        __SINIT(.admin, {
            NULL, "create", NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
        })
    },{
        __SINIT(.update, {
            NULL, "checkout", NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
        }),
        __SINIT(.commit, {
            NULL, "commit", "--amend", NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
        }),
        __SINIT(.push, {
            NULL, "push", "origin", NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
        }),
        __SINIT(.add, {
            NULL, "add", NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
        }),
        __SINIT(.checkout, {
            NULL, "checkout", NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
        }),
        __SINIT(.admin, {
            NULL, "init", "--bare", NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
        })
    },{
        __SINIT(.update, {
            NULL, "update", NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
        }),
        __SINIT(.commit, {
            NULL, "commit", "--amend", NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
        }),
        __SINIT(.push, {
            NULL, "push", " -r", ".", NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
        }),
        __SINIT(.add, {
            NULL, "add", NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
        }),
        __SINIT(.checkout, {
            NULL, "checkout", NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
        }),
        __SINIT(.admin, {
            NULL, "init", NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
        })
    }
};

static const char *vcs_bin[][2] =
{
    { "svn" __BINEXT, "svnadmin" __BINEXT },
    { "git" __BINEXT, "git" __BINEXT },
    { "hg"  __BINEXT, "hg" __BINEXT },
};

#endif

