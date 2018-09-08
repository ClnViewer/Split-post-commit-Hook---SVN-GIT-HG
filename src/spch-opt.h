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
    "                         'ctime' - the date of creation,\n" \
    "                         'mtime' - the modification date,\n" \
    "                         'size'  - in size,\n" \
    "                         'all'   - for all of the above\n" \
    "                        parameters, the default value: 'all'\n\n" \
    "  -r, --revision 'val'  Set the current revision of copies of files\n" \
    "                        relative to master repository.\n\n" \
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

static char *help[] =
{
    "master repository, from copy",
    "split repository, to copy",
    "file list name, search in master repository",
    "root directory to SVN/GIT/HG repository",
    "binary execute VCS directory",
    "journal log path include file name",
    "run as 'user' privilege (Linux only)",
    "run deploy script before commit",
    "rename files, prefix example 'new' or 'old=new'",
    "VCS type: [svn|git|hg]",
    "file compare: [mtime|ctime|size|all]",
    "current VCS revision",
    "create ChangeLog, format: [md|gnu]",
    "use file 'deploy.yaml', run test/check before commit",
    "force overwrite all destination files",
    "fork and daemonize, no-loop VCS mode",
    "quiet mode, no print message to console",
    "full command information help page",
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
    { "uid",      required_argument,  NULL, 'u' },
    { "deploy",   required_argument,  NULL, 'd' },
    { "rename",   required_argument,  NULL, 'x' },
    { "vcs",      required_argument,  NULL, 't' },
    { "check",    required_argument,  NULL, 'c' },
    { "revision", required_argument,  NULL, 'r' },
    { "chnglog",  required_argument,  NULL, 'g' },
    { "yaml",     no_argument,        NULL, 'y' },
    { "force",    no_argument,        NULL, 'f' },
    { "nonloop",  no_argument,        NULL, 'k' },
    { "quiet",    no_argument,        NULL, 'q' },
    { "info",     no_argument,        NULL, 'i' },
    { "help",     no_argument,        NULL, 'h' },
    { 0, 0, 0, 0 }
};


#endif
