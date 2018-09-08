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

#if !defined(BUILDT_H)
#define BUILDT_H

#if ((!defined(__STDC_VERSION__) || (__STDC_VERSION__ < 199901L)) && \
      !defined(__STDC_C99) && !defined(__C99_RESTRICT))
#   if !defined(restrict)
#      if defined(__GNUC__) || defined(__clang__)
#         define restrict __restrict
#      elif defined(_MSC_VER)
#         define restrict
#      elif defined(__SUNPRO_C)
#         define restrict _Restrict
#      else
#         define restrict
#      endif
#   endif
#   if !defined(inline)
#      if defined(__GNUC__) || defined(__clang__)
#         define inline inline __attribute__((always_inline))
#      elif defined(_MSC_VER)
#         define inline __forceinline
#      else
#         define inline
#      endif
#   endif
#endif

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

#if defined(OS_WIN)

#   define __PSEPC '\\'
#   define __PSEPS "\\"
#   define __BNAME ((strrchr(__func__, __PSEPC)) ? (strrchr(__func__, __PSEPC) + 1) : __func__)
#   define __PID (int)GetCurrentProcessId(), 0

#   if defined(BUILD_MSVC)
#      define __AUTO(x)
#      define __attribute__(x)
#      define __func__ __FUNCTION__
#      define _CRT_SECURE_NO_WARNINGS 1
#      define __snprintf _snprintf
#      define __SINIT(x, ...) __VA_ARGS__
#   else
#      define __AUTO(x) __attribute__((cleanup(x)))
#      define __snprintf snprintf
#      define __SINIT(x, ...) x = __VA_ARGS__
#   endif

#   include "windows.h"

#else

#   if !defined(_GNU_SOURCE)
#      define _GNU_SOURCE 1
#   endif

#   define __PSEPC '/'
#   define __PSEPS "/"
#   define _mkdir(A) mkdir(A, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH)
#   define _chdir chdir
#   define __snprintf snprintf
#   define __BNAME basename((char*) __func__)
#   define __PID (int)getpid(), (int)getppid()
#   define __SINIT(x, ...) x = __VA_ARGS__

#   if defined(__GNUC__)
#      define __AUTO(x) __attribute__((cleanup(x)))
#      define __func__ __FUNCTION__
#   else
#      define __AUTO(x)
#      define __attribute__(x)
#   endif

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
#include <stdint.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "boolt.h"

#if defined(OS_WIN)
int _mkdir(const char*);
int _chdir(const char*);
const char * _strptime(const char*, const char*, struct tm*);
#define strptime (char*)_strptime
#endif

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

#endif

