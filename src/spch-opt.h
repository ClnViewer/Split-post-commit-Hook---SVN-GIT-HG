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

#if !defined(SPCH_OPT_H)
#define SPCH_OPT_H

static char *info =
    "  -m, --master 'val'    Master repository, from where we copy.\n\n" \
    "  -s, --split 'val'     Split repository, where we copy it.\n\n" \
    "  -l, --list 'val'      List of copied files, set by root master\n" \
    "                        repository.\n\n" \
    "  -o, --rootsvn 'val'   Root directory SVN/GIT/HG repositories.\n\n" \
    "  -e, --execdir 'val'   Path to VCS executables, for 'POSIX' systems\n" \
    "                        by default '/usr/bin'.\n\n" \
    "  -j, --log 'val'       Path and log file name.\n\n" \
    "  -u, --uid 'val'       Set user for file operations. (*nix only)\n\n" \
    "  -d, --deploy 'val'    Deploy script, execute before the 'commit' operation,\n" \
    "                        returns '0' if the tests/assembly was successful,\n" \
    "                        otherwise, 'commit is executed will not be.\n\n" \
    "  -y, --yaml            Look for configuration file 'deploy.yaml' in the root\n" \
    "                        of the 'split' repository and in the case success,\n" \
    "                        execute the commands described in it.\n" \
    "                        For work, the installed shell 'bash' or 'ksh'.\n" \
    "                        Format of the 'yaml' file is described README.md.\n\n" \
    "  -x, --rename 'val'    Rename or add prefix to copied files:\n" \
    "                         add prefix - 'new'\n" \
    "                         replace    - 'old = new'\n\n" \
    "  -t, --vcs 'val'       Type of used VCS:\n" \
    "                         'svn' - Subversion,\n" \
    "                         'git' - Git SCM,\n" \
    "                         'hg'  - Mercurial SCM.\n\n" \
    "  -c, --check 'val'     Check the updated files:\n" \
    "                         'c' - the date of creation,\n" \
    "                         'm' - the modification date,\n" \
    "                         's' - in size,\n" \
    "                         'd' - default value: 'm' and 's',\n" \
    "                         'a' - for all of the above\n" \
    "                        syntax example: '--check mc' or '--check cs'\n\n" \
    "  -r, --revision 'val'  Set the current revision of copies of files\n" \
    "                        relative to master repository.\n\n" \
    "  -g, --chnglog 'val'   Keep 'ChangeLog' in the 'split' repository,\n" \
    "                        based on the commits in the 'master' repository,\n" \
    "                        the possible formats are:\n" \
    "                         'md'  - markdown format,\n" \
    "                         'gnu' - text '.txt' gnu standart format\n\n" \
    "  -b, --backup 'val'    Create Backup, value as directory path\n\n" \
    "  -f, --force           Overwrite all files without checking\n" \
    "                        to change.\n\n" \
    "  -k, --nonloop         Prevent simultaneous 'commit' and\n" \
    "                        'update' in the repository wizard,\n" \
    "                        daemonizes the process at runtime,\n" \
    "                        together with this flag is highly desirable\n" \
    "                        use the option to write the log '-j'.\n\n" \
    "  -q, --quiet           Silent mode, do not output anything on\n" \
    "                        console, only works after processing\n" \
    "                        configuration parameters from command line.\n\n" \
    "  -i, --info            Detailed description of the commands (this).\n\n" \
    "  -h, --help            Show help on commands.\n\n"
    ;

typedef enum
{
#define __OPT(A,B,C,D,E) ENUM_SETUP_##A,
#include "spch-opt-data.h"
    ENUM_SETUP_RETURN_OK,
    ENUM_SETUP_EXT_DATE,
    ENUM_SETUP_EXT_NAME,
} setup_options_e;

static char *help[] =
{
#define __OPT(A,B,C,D,E) E,
#include "spch-opt-data.h"
    NULL
};

static struct option options[] =
{
#define __OPT(A,B,C,D,E) { __SINIT(.name, #A), __SINIT(.has_arg, B), __SINIT(.flag, C), __SINIT(.val, D) },
#include "spch-opt-data.h"
    { 0, 0, 0, 0 }
};

#define __XMLTAG "<?xml version=\"1.0\""

static string_s xmltag = { __XMLTAG, __CSZ(__XMLTAG) };

#endif
