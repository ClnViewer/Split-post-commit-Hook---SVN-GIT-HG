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

__OPT( master,   required_argument,  NULL, 'm', "master repository, from copy" )
__OPT( split,    required_argument,  NULL, 's', "split repository, to copy" )
__OPT( list,     required_argument,  NULL, 'l', "file list name, search in master repository" )
__OPT( rootsvn,  required_argument,  NULL, 'o', "root directory to SVN/GIT/HG repository" )
__OPT( execdir,  required_argument,  NULL, 'e', "binary execute VCS directory" )
__OPT( log,      required_argument,  NULL, 'j', "journal log path include file name" )
__OPT( uid,      required_argument,  NULL, 'u', "run as 'user' privilege (Linux only)" )
__OPT( deploy,   required_argument,  NULL, 'd', "run deploy script before commit" )
__OPT( backup,   required_argument,  NULL, 'b', "create Backup, directory path required" )
__OPT( rename,   required_argument,  NULL, 'x', "rename files, prefix example 'new' or 'old=new'" )
__OPT( vcs,      required_argument,  NULL, 't', "VCS type: [svn|git|hg]" )
__OPT( check,    required_argument,  NULL, 'c', "file compare: [m|c|s|a|d], see --info" )
__OPT( revision, required_argument,  NULL, 'r', "current VCS revision" )
__OPT( chnglog,  required_argument,  NULL, 'g', "create ChangeLog, format: [md|gnu]" )
__OPT( yaml,     no_argument,        NULL, 'y', "use file 'deploy.yaml', run test/check before commit" )
__OPT( force,    no_argument,        NULL, 'f', "force overwrite all destination files" )
__OPT( nonloop,  no_argument,        NULL, 'k', "fork and daemonize, no-loop VCS mode" )
__OPT( quiet,    no_argument,        NULL, 'q', "quiet mode, no print message to console" )
__OPT( info,     no_argument,        NULL, 'i', "full command information help page" )
__OPT( help,     no_argument,        NULL, 'h', "this help page" )

#undef __OPT
