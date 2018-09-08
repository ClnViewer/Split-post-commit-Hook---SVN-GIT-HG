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

#if !defined(SPCH_H)
#define SPCH_H

#include "libs/include/buildt.h"
#include "libs/include/boolt.h"
#include "libs/include/string_s.h"

typedef enum
{
    FILE_MASTER_REPO,
    FILE_SPLIT_REPO,
    FILE_FILELIST,
    FILE_ROOTVCS,
    FILE_BINDIR,
    FILE_FLOG,
    FILE_UUID,
    FILE_DEPLOY,
    FILE_RENAME1,
    FILE_RENAME2,
    FILE_NONE_IDX
} setup_files_e;

typedef enum
{
    OPT_VCS_SVN,
    OPT_VCS_GIT,
    OPT_VCS_HG,
    OPT_FORCE,
    OPT_QUIET,
    OPT_UUID,
    OPT_RENAME,
    OPT_PREFIX,
    OPT_FCHECK_CTIME,
    OPT_FCHECK_MTIME,
    OPT_FCHECK_SIZE,
    OPT_FCHECK_ALL,
    OPT_DEPLOY,
    OPT_YAML,
    OPT_DEMONIZE,
    OPT_CHLOG_MD,
    OPT_CHLOG_GNU
} setup_opt_e;

typedef enum
{
    TYPE_DIR,
    TYPE_FILE,
    TYPE_UID
} type_io_e;

typedef struct
{
    unsigned long bitopt, rev;
    FILE *fp[3];
    long     fpos;
    string_s setup[FILE_NONE_IDX];
    string_s bins[2];
} paths_t;

#define pch_check_dir(A) pch_check_(A, TYPE_DIR)
#define pch_check_file(A) pch_check_(A, TYPE_FILE)
#define pch_check_uid(A) pch_check_(A, TYPE_UID)

#define pch_log_error(A,B,...) \
    pch_log_(0,A,"[%d/%d][%s:%d] -> " B " -> [%d = %s]\n",__PID,__BNAME,__LINE__,__VA_ARGS__, errno, strerror(errno))
#define pch_log_info(A,B,...)  \
    pch_log_(1,A,"[%d/%d][%s:%d] -> " B "\n",__PID,__BNAME,__LINE__,__VA_ARGS__)

void   pch_log_(int, paths_t*, const char*, ...);
int    pch_option(paths_t*, char* [], int);
const char * pch_option_chkmode(paths_t*);
const char * pch_ultostr(char*, unsigned long, int);
bool_t pch_compare_file(paths_t*, string_s*, string_s*);
size_t pch_check_(string_s*, type_io_e);
bool_t pch_path_dir(string_s*, string_s*);
bool_t pch_path_time(char [], const char*);
bool_t pch_path_setuid(paths_t*, int);
void   pch_path_free(paths_t*);
bool_t pch_path_destination(paths_t*, char*, size_t, string_s*);
int    pch_fcopy(string_s*, string_s*);
int    pch_exec(paths_t*, const char *const [], FILE*);
bool_t pch_fork(int, char* []);
bool_t pch_stage1(paths_t*);
bool_t pch_stage2(paths_t*);
bool_t pch_stage3(paths_t*);
bool_t pch_stage4(paths_t*);

bool_t spch_xmllog(paths_t*, FILE*, FILE*);

const char * pch_vcs_type(unsigned long);
int    pch_vcs_bincheck(paths_t*);
int    pch_vcs_update(paths_t*, string_s*);
int    pch_vcs_commit(paths_t*);
int    pch_vcs_add(paths_t*, string_s*);
int    pch_vcs_create(paths_t*);
int    pch_vcs_log(paths_t*, string_s*);
bool_t pch_vcs_changelog(paths_t*);

void   startedlog(paths_t*, const char*);
void   endedlog(paths_t*);

static inline void __attribute__((always_inline)) __autopathst(void *v)
{
    if (v)
    {
        paths_t *x = (paths_t*)(*(void**)v);
        if (x)
        {
            pch_path_free(x);
        }
    }
}

#endif

