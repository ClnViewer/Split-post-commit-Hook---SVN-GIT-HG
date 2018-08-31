#if !defined(SPCH_H)
#define SPCH_H

#if ( \
        defined(_WIN32) || defined(__WIN32__) || defined(_Windows) || \
        defined(__WINNT) || defined(__WINNT__) || defined(WINNT) || \
        defined(_WIN64) || defined(__WIN64__) || \
        defined(__CYGWIN__) || defined(__MINGW32__) || defined(__MINGW64__))

#   define OS_WIN 1
#   if (defined(_WIN32) || defined(__WIN32__) || defined(_Windows))
#       define OS_WIN32 1
#   elif (defined(_WIN64) || defined(__WIN64__))
#       define OS_WIN64 1
#   else
#       define OS_NONE 1
#   endif

#   if  (defined(OS_WIN32) && defined(_MSC_VER))
#       define BUILD_MSVC 1
#       define BUILD_MSVC32 1
#   elif  (defined(OS_WIN64) && defined(_MSC_VER))
#       define BUILD_MSVC 1
#       define BUILD_MSVC64 1
#   elif  (defined(OS_WIN32) && defined(__CYGWIN__))
#       define BUILD_CYGWIN 1
#       define BUILD_CYGWIN32 1
#   elif  (defined(OS_WIN64) && defined(__CYGWIN__))
#       define BUILD_CYGWIN 1
#       define BUILD_CYGWIN64 1
#   elif  (defined(OS_WIN32) && defined(__MINGW32__))
#       define BUILD_MINGW 1
#       define BUILD_MINGW32 1
#   elif  (defined(OS_WIN64) && defined(__MINGW64__))
#       define BUILD_MINGW 1
#       define BUILD_MINGW64 1
#   endif

#endif

#define __AUTO(x) __attribute__((cleanup(x)))

#if defined(OS_WIN)
#   define __PSEPC '\\'
#   define __PSEPS "\\"
#   define __BNAME ((strrchr(__func__, __PSEPC)) ? (strrchr(__func__, __PSEPC) + 1) : __func__)
#   define __PID (int)GetCurrentProcessId(), 0
int _mkdir(const char*);
int _chdir(const char*);

#   include "windows.h"

#else
#   define __PSEPC '/'
#   define __PSEPS "/"
#   define _mkdir(A) mkdir(A, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH)
#   define _chdir chdir
#   define __BNAME basename((char*) __func__)
#   define __PID (int)getpid(), (int)getppid()

#   include <libgen.h>
#   include <unistd.h>
#endif

#define __CSZ(a)  (int const) (sizeof(a)-1)
#define __NELE(a) (sizeof(a) / sizeof(a[0]))
#define __BITBOOL(x) (!(!(x)))
#define __BITSET(arg,mask) ((arg) | (1UL << (mask)))
#define __BITCLR(arg,mask) ((arg) & ~(1UL << (mask)))
#define __BITTST(arg,mask) __BITBOOL((arg) & (1UL << (mask)))
#define __BITFLP(arg,mask) ((arg) ^ (1UL << (mask)))

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>

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
    OPT_VCS_HQ,
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
    OPT_DEMONIZE
} setup_opt_e;

typedef enum
{
    TYPE_DIR,
    TYPE_FILE,
    TYPE_UID
} type_io_e;

typedef struct
{
    char *str;
    size_t sz;
} string_s;

typedef struct
{
    unsigned long bitopt, rev;
    FILE *fp[2];
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
int    pch_compare_file(paths_t*, string_s*, string_s*);
size_t pch_check_(string_s*, type_io_e);
size_t pch_path_format(string_s*, const char*, ...);
size_t pch_path_dump(string_s*, const char*);
int    pch_path_dir(string_s*, string_s*);
int    pch_path_time(char [], const char*);
int    pch_path_setuid(paths_t*, int);
void   pch_path_free(paths_t*);
int    pch_path_destination(paths_t*, char*, size_t, string_s*);
int    pch_fcopy(string_s*, string_s*);
int    pch_exec(paths_t*, const char *const []);
int    pch_stage1(paths_t*);
int    pch_stage2(paths_t*);
int    pch_stage3(paths_t*);
int    pch_stage4(paths_t*);

const char * pch_vcs_type(unsigned long);
int    pch_vcs_bincheck(paths_t*);
int    pch_vcs_update(paths_t*, string_s*);
int    pch_vcs_commit(paths_t*);
int    pch_vcs_add(paths_t*, string_s*);
int    pch_vcs_create(paths_t*);

void   startedlog(paths_t*, const char*);
void   endedlog(paths_t*);

static inline void __attribute__((always_inline)) __autofree(void *v)
{
    if (v)
    {
        void *x = *(void**)v;
        if (x)
        {
            free(x);
            // cppcheck-suppress unreadVariable
            x = ((void*)0);
        }
    }
}
static inline void __attribute__((always_inline)) __autofclose(void *v)
{
    if (v)
    {
        void *x = *(void**)v;
        if (x)
        {
            fclose(x);
            // cppcheck-suppress unreadVariable
            x = ((void*)0);
        }
    }
}
static inline void __attribute__((always_inline)) __autostring(void *v)
{
    if (v)
    {
        string_s *x = (string_s*)(*(void**)v);
        if (x)
        {
            if (x->str)
                free(x->str);

            x->str = ((void*)0);
            x->sz  = 0U;
        }
    }
}
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

