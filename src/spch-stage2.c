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

#include "spch-stage2-xml.h"

typedef enum
{
    ENUM_XML_LIST_EMPTY = -1,
#define __FIELD(A) ENUM_XML_LIST_##A,
#include "spch-stage2-field.h"
    ENUM_XML_LIST_END_INDEX
} vcsfiles_e;

typedef struct
{
    int       rcode, stage;
    hash_t   *hd, *hf;
    string_s *s;
    paths_t  *dirs;
    struct zip_t *zip;
    string_s zlist;
} vcsfiles_c;

typedef struct
{
    vcsfiles_e id;
    string_s field;
} vcsfiles_b;

typedef struct
{
    vcsfiles_e  id, root;
    vcsfiles_c *data;
} vcsfiles_a;

static vcsfiles_b __field_vcsfiles[] =
{
#define __FIELD(A) { __SINIT(.id, ENUM_XML_LIST_##A), { __SINIT(.str, #A), __SINIT(.sz, __CSZ(#A)) }},
#include "spch-stage2-field.h"
};

static bool_t __stage2_vcsfile(vcsfiles_c *vcfc)
{
    int ret;
    size_t len;
    char b[BUFSIZ + 1] = {0};
    string_s from_b = { NULL, 0U },
             to_b = { NULL, 0U },
             to_dir_b = { NULL, 0U };

    string_s __AUTO(__autostring)
    *from_s = &from_b,
     *to_s = &to_b,
      *to_dir_s = &to_dir_b;

    vcfc->rcode = 0;

    if (vcfc->s == NULL)
    {
        vcfc->rcode = -1;
        return R_FALSE;
    }

    len = vcfc->s->sz;
    if (!memcpy((void*)b, (void*)vcfc->s->str, len))
    {
        vcfc->rcode = -4;
        return R_FALSE;
    }

    if (
        (b[(len - 1)] == '\n') ||
        (b[(len - 1)] == '\r')
    )
        len -= 1;

    b[len] = '\0';

    /*! Check duplicate path and skip if found */
    if (vcfc->hf->searchs(vcfc->hf->hash, b, len))
    {
        vcfc->rcode = 0;
        return R_TRUE;
    }

    (void) vcfc->hf->adds(vcfc->hf->hash, b, len);

#   if defined(BUILD_MSVC)
    __try
    {
#   endif

        /*! Create source file path */
        if (
            (!string_format(from_s, "%s" __PSEPS "%s", vcfc->dirs->setup[FILE_MASTER_REPO].str, b)) ||
            (pch_path_destination(vcfc->dirs, (char*)b, len, to_s) != R_TRUE)
        )
        {
            vcfc->rcode = -1;
            return R_FALSE;
        }

#       if defined(__DEBUG__)
        printf("%d) [%s]: %s -> %s\n", __LINE__, __func__, from_s->str, to_s->str);
#       endif

        /*! Check file modify - diff cdate/mdate/size */
        if (!__BITTST(vcfc->dirs->bitopt, OPT_FORCE))
        {
            if (pch_compare_file(vcfc->dirs, from_s, to_s) != R_TRUE)
            {
                if (__ISLOGPV(vcfc->dirs))
                {
                    pch_log_info(vcfc->dirs, "stage #2 skip: %s -> %s", from_s->str, to_s->str);
                }
                vcfc->rcode = 0;
                return R_TRUE;
            }
        }
        /*! BEGIN: Create and check sub-directory if found in path and not exist */
        if (strchr(b, __PSEPC))
        {
            switch (pch_path_dir(to_dir_s, to_s))
            {
            case R_NEGATIVE:
            {
                vcfc->rcode = -2;
                break;
            }
            case  R_FALSE:
            {
                vcfc->rcode = 0;
                break;
            }
            case R_TRUE:
            {
                vcfc->rcode = 0;

                if (!vcfc->hd->searchs(vcfc->hd->hash, to_dir_s->str, to_dir_s->sz))
                {
                    (void) vcfc->hd->addmap(vcfc->hd->hash, to_dir_s->str, to_dir_s->sz);
                }
                if (!pch_check_dir(to_dir_s))
                {
                    errno = 0;

                    if (
                        (_mkdir(to_dir_s->str) < 0) &&
                        (errno != EEXIST)
                    )
                    {
                        pch_log_error(vcfc->dirs, "stage #2 create directory error: %s", to_dir_s->str);
                        vcfc->rcode = -3;
                        break;
                    }
                }
            }
            default:
            {
                break;
            }
            }
        }
        /*! END: Create and check sub-directory if found in path and not exist */
        if (vcfc->rcode < 0)
            return R_FALSE;


#       if defined(__DEBUG__)
        printf("%d) [%s]: %s -> %s -> %s\n",
               __LINE__, __func__, from_s->str, to_s->str,
               ((to_dir_s->str) ? to_dir_s->str : "- none -")
              );
#       endif

        /*! Copy master-repo file to destination split-repo */
        if ((ret = pch_fcopy(from_s, to_s)))
        {
            pch_log_error(vcfc->dirs, "stage #2 copy file error: %s -> %s", from_s->str, to_s->str);
            vcfc->rcode = -1;
            return R_TRUE;
        }
        /*! Copy master-repo file content to BackUp zip archive */
        if (vcfc->zip)
        {
            if (!zip_entry_open(vcfc->zip, b))
            {
                (void) zip_entry_fwrite(vcfc->zip, from_s->str);
                (void) zip_entry_close(vcfc->zip);
                (void) string_appends(
                    &vcfc->zlist,
                    __STG2_XML_FILE_S,
                    b,
                    __STG2_XML_FILE_E
                );
            }
        }
        if (__ISLOGPV(vcfc->dirs))
        {
            pch_log_info(vcfc->dirs, "stage #2 %s: %s -> %s",
                         ((__BITTST(vcfc->dirs->bitopt, OPT_FORCE)) ? "replace" : "update"),
                         from_s->str, to_s->str
                        );
        }
        vcfc->stage = ((!ret) ? (vcfc->stage + 1) : vcfc->stage);

#   if defined(BUILD_MSVC)
    }
    __finally
    {
        if (from_s->str)
        {
            string_free(from_s);
        }
        if (to_s->str)
        {
            string_free(to_s);
        }
        if (to_dir_s->str)
        {
            string_free(to_dir_s);
        }
    }
#   endif

    return R_TRUE;
}

static void __stage2_vcs_add(unsigned int cnt, unsigned long hash, char *str, size_t sz, void * data)
{
    int ret;
    paths_t *dirs = (paths_t*)data;
    string_s sdir = { str, 0U };
    (void) hash;
    (void) sz;

    if ((ret = pch_vcs_add(dirs, &sdir)) != 0)
    {
        pch_log_error(dirs, "add VCS directory error: %d -> %u) %s", ret, cnt, sdir.str);
    }
}

static int __cb_opentag(void *v, int depth, char *name)
{
    unsigned i;
    vcsfiles_a *data = (vcsfiles_a*)v;
    data->id = ENUM_XML_LIST_EMPTY;

    (void) depth;

    for (i = 0U; i < __NELE(__field_vcsfiles); i++)
    {
        if (!strncmp(name, __field_vcsfiles[i].field.str, __field_vcsfiles[i].field.sz))
        {
            data->id = __field_vcsfiles[i].id;
            break;
        }
    }
    data->root = ((data->id == ENUM_XML_LIST_spchlist) ? ENUM_XML_LIST_spchlist : data->root);
    return 0;
}

static int __cb_text(void *v, int depth, char *text)
{
    size_t sz;
    vcsfiles_a *data = (vcsfiles_a*)v;
    (void) depth;

    if (data->root == ENUM_XML_LIST_spchlist)
    {
        if (
            (data->id == ENUM_XML_LIST_file) &&
            ((sz = strlen(text)))
        )
        {
            string_s str;
            str.str = text;
            str.sz = sz;
            data->data->s = &str;
            return (__stage2_vcsfile(data->data) != R_TRUE);
        }
        else if (
            (data->id == ENUM_XML_LIST_name) &&
            ((sz = strlen(text)))
        )
        {
            string_free(&data->data->dirs->setup[FILE_MASTER_NAME]);
            if (!string_append(&data->data->dirs->setup[FILE_MASTER_NAME], text, sz))
                return -1;

            if (__ISLOGPV(data->data->dirs))
                pch_log_info(
                    data->data->dirs,
                    "stage #2 repo list name: %s",
                    data->data->dirs->setup[FILE_MASTER_NAME].str
                );
        }
        else if (
            (data->id == ENUM_XML_LIST_date) &&
            (__ISLOGPV(data->data->dirs)) &&
            ((sz = strlen(text)))
        )
        {
            char b[100] = {0};
            struct tm *tms;
            time_t t = (time_t)strtoul(text, NULL, 10);

            if ((unsigned long)t == 0UL)
                return 0;

            tms = localtime(&t);
            if (!tms)
                return 0;

            (void) strftime(b, 100, "%d.%m.%Y %H:%M", tms);
            pch_log_info(
                data->data->dirs,
                "stage #2 repo list date: %s",
                b
            );
        }
    }
    return 0;
}

static bool_t __stage2_xmlfiles(vcsfiles_c *vcfc)
{
    int ret;
    vcsfiles_a vcfa;
    memset(&vcfa, 0, sizeof(vcsfiles_a));
    vcfa.id = vcfa.root = ENUM_XML_LIST_EMPTY;
    vcfa.data = vcfc;

    ret = xmlpf(vcfc->dirs->fp[PATHS_FILE_LST],
                &vcfa,
                __cb_opentag,
                NULL,
                NULL,
                NULL,
                __cb_text
               );

    return ((ret == -1) ? R_NEGATIVE :
            ((!ret) ? R_TRUE : R_FALSE)
           );
}

static bool_t __stage2_txtfiles(vcsfiles_c *vcfc)
{
    char b[BUFSIZ + 1] = {0};

    while (fgets(b, BUFSIZ, vcfc->dirs->fp[PATHS_FILE_LST]) != NULL)
    {
        string_s str;

        if (b[0] == '\0')
            continue;

        str.sz = strlen(b);

        if (!str.sz)
            continue;

        str.str = (char*)b;
        vcfc->s = &str;

        if (__stage2_vcsfile(vcfc) != R_TRUE)
            return R_FALSE;

        vcfc->s = NULL;
    }
    return R_TRUE;
}

static bool_t __stage2_create_zip_name(paths_t *dirs, string_s *str, unsigned long count)
{
    size_t ret;
    char brev[100] = {0};

    string_free(str);

    if (dirs->rev)
    {
        (void) pch_ultostr(brev, dirs->rev, 10);
    }
    else
    {
        struct tm *tms;
        time_t t = time(NULL);
        tms = localtime(&t);
        strftime(brev, 100, "%Y-%m-%d", tms);
    }
    if (count)
    {
        char bcnt[20] = {0};
        (void) pch_ultostr(bcnt, count, 10);
        ret = string_appends(
                  str,
                  dirs->setup[FILE_BACKUP].str,
                  ((dirs->setup[FILE_BACKUP].str[(dirs->setup[FILE_BACKUP].sz - 1)] == __PSEPC) ? "" : __PSEPS),
                  dirs->setup[FILE_MASTER_NAME].str,
                  "-",
                  brev,
                  "-",
                  bcnt,
                  ".zip"
              );
    }
    else
    {
        ret = string_appends(
                  str,
                  dirs->setup[FILE_BACKUP].str,
                  ((dirs->setup[FILE_BACKUP].str[(dirs->setup[FILE_BACKUP].sz - 1)] == __PSEPC) ? "" : __PSEPS),
                  dirs->setup[FILE_MASTER_NAME].str,
                  "-",
                  brev,
                  ".zip"
              );
    }
    if (!ret)
    {
        pch_log_error(dirs, "stage #2 zip name create error: %d", errno);
        return R_NEGATIVE;
    }
    return R_TRUE;
}

bool_t pch_stage2(paths_t *dirs)
{
    bool_t ret = R_FALSE;
    vcsfiles_c vfc;
    memset(&vfc, 0, sizeof(vcsfiles_c));
    vfc.dirs = dirs;
    vfc.hd = hash_init();
    vfc.hf = hash_init();

    /*! Create BackUp zip archive */
    if (__BITTST(dirs->bitopt, OPT_BACKUP))
    {
        unsigned long count = 0UL;
        string_s zname = { NULL, 0U };

        do
        {
            char bdate[20] = {0};
            bool_t rzip = R_NEGATIVE;

            do
            {
                rzip = __stage2_create_zip_name(dirs, &zname, count++);
                if (rzip == R_TRUE)
                {
                    if (!pch_check_file(&zname))
                        break;
                }
                else if (rzip == R_NEGATIVE)
                    break;
            }
            while (count < 300);

            if (rzip == R_NEGATIVE)
            {
                string_free(&zname);
                string_free(&dirs->setup[FILE_BACKUP]);
                break;
            }

            string_free(&dirs->setup[FILE_BACKUP]);
            dirs->setup[FILE_BACKUP].str = zname.str;
            dirs->setup[FILE_BACKUP].sz = zname.sz;

            if (!string_appends(
                        &vfc.zlist,
                        __STG2_XML_START,
                        __STG2_XML_HEAD_S,
                        __STG2_XML_REPO_S,
                        dirs->setup[FILE_MASTER_NAME].str,
                        __STG2_XML_REPO_E,
                        __STG2_XML_DATE_S,
                        pch_ultostr(bdate, (unsigned long)time(NULL), 10),
                        __STG2_XML_DATE_E,
                        __STG2_XML_HEAD_E
                    ))
                break;

            if (pch_check_file(&zname))
            {
                pch_log_error(dirs, "stage #2 zip archive name limit error: %s", zname.str);
                string_free(&dirs->setup[FILE_BACKUP]);
                break;
            }
            if (!(vfc.zip = zip_open(zname.str, ZIP_DEFAULT_COMPRESSION_LEVEL, 'w')))
            {
                pch_log_error(dirs, "stage #2 zip archive create error: %s", zname.str);
                string_free(&dirs->setup[FILE_BACKUP]);
                break;
            }
            if (__ISLOGP)
            {
                pch_log_info(dirs, "stage #2 zip archive created: %s", zname.str);
            }
        }
        while (0);
    }

    /*! Select input source list (type) */
    if (__BITTST(dirs->bitopt, OPT_INPUT_XML))
    {
        ret = __stage2_xmlfiles(&vfc);
    }
    else if (__BITTST(dirs->bitopt, OPT_INPUT_TXT))
    {
        ret = __stage2_txtfiles(&vfc);
    }

    /*! Close BackUp zip archive */
    if (vfc.zip)
    {
        if ((vfc.stage) && (vfc.zlist.str))
        {
            do
            {
                if (!string_append(&vfc.zlist, __STG2_XML_END, __CSZ(__STG2_XML_END)))
                    break;
                if (zip_entry_open(vfc.zip, __STG2_XML_ENTRY_NAME))
                    break;
                (void) zip_entry_write(vfc.zip, vfc.zlist.str, vfc.zlist.sz);
                (void) zip_entry_close(vfc.zip);
            }
            while (0);
        }
        zip_close(vfc.zip);
        vfc.zip = NULL;

        if (!vfc.stage)
        {
            (void) remove(dirs->setup[FILE_BACKUP].str);
            string_free(&dirs->setup[FILE_BACKUP]);
        }
    }

    /*! Check all return value */
    if (ret == R_TRUE)
    {
        if ((!vfc.rcode) && (vfc.stage > 0))
        {
            /* needed to add exist directory before */
            if (pch_vcs_add(dirs, &dirs->setup[FILE_SPLIT_REPO]) != 0)
            {
                pch_log_error(dirs, "stage #2 add VCS root directory error: -> %s", dirs->setup[FILE_SPLIT_REPO].str);
            }
            /* add all files from directory */
            vfc.hd->getmap(vfc.hd->hash, (void*)dirs, &__stage2_vcs_add);
        }

        if ((vfc.rcode < 0) && (__ISLOGP))
        {
            pch_log_error(dirs, "stage #2 produced error %d", vfc.rcode);
        }
        else if ((vfc.stage) && (__ISLOGP))
        {
            pch_log_info(dirs, "stage #2 split repo modified %d objects", vfc.stage);
        }
    }
    else
    {
        pch_log_error(dirs, "stage #2 returned error %d, update abort..", ret);
    }

    vfc.hf->free(vfc.hf->hash);
    free(vfc.hf);
    vfc.hd->free(vfc.hd->hash);
    free(vfc.hd);
    string_free(&vfc.zlist);

    return ((ret != R_TRUE) ? ret :
            ((vfc.rcode < 0) ? R_NEGATIVE :
             ((!vfc.stage) ? R_FALSE : R_TRUE)
            )
           );
}
