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
#include "libs/include/xmlp.h"

typedef enum
{
    ENUM_EMPTY = -1,
#define __ATTR(...)
#define __FIELD(A) ENUM_##A,
#include "spch-vcs-log-field.h"
    ENUM_END_INDEX
} vcslog_e;

typedef struct
{
    int      id;
    string_s field;
} vcslog_b;

typedef struct
{
    FILE    *fp;
    paths_t *dirs;
    int      id;
    string_s field[ENUM_END_INDEX];
} vcslog_a;

static vcslog_b __field_vcslog[] =
{
#define __ATTR(...)
#define __FIELD(A) { __SINIT(.id, ENUM_##A), { __SINIT(.str, #A), __SINIT(.sz, __CSZ(#A)) }},
#include "spch-vcs-log-field.h"
};

static string_s __attr_vcslog[] =
{
#define __FIELD(...)
#define __ATTR(A) { __SINIT(.str, #A), __SINIT(.sz, __CSZ(#A)) },
#include "spch-vcs-log-field.h"
};

static void __vcslog_a_free(vcslog_a *data)
{
    unsigned i;

    for (i = 0U; i < ENUM_END_INDEX; i++)
    {
        string_free(&data->field[i]);
    }
}

static int __cb_opentag(void *v, int depth, char *name)
{
    unsigned i;
    vcslog_a *data = (vcslog_a*)v;
    data->id = ENUM_EMPTY;

    (void) depth;

    for (i = 0U; i < __NELE(__field_vcslog); i++)
    {
        if (!strncmp(name, __field_vcslog[i].field.str, __field_vcslog[i].field.sz))
        {
            data->id = __field_vcslog[i].id;
            break;
        }
    }
    return 0;
}

static int __cb_closetag(void *v, int depth, char* name)
{
    vcslog_a *data = (vcslog_a*)v;
    (void) depth;

    if (
        (!strncmp(name, __field_vcslog[0].field.str, __field_vcslog[0].field.sz)) &&
        (data->field[ENUM_msg].str)
    )
    {
        char *fmt = ((__BITTST(data->dirs->bitopt, OPT_CHLOG_MD)) ?
                     "\n#### %s   \n - [%s]() : %s   \n" : "%s\n\t#%s - %s\n"
                    );
        fprintf(data->fp, fmt,
                ((data->field[ENUM_date].str) ? data->field[ENUM_date].str : "(no date)"),
                ((data->field[ENUM_logentry].str) ? data->field[ENUM_logentry].str : "?"),
                ((data->field[ENUM_msg].str) ? data->field[ENUM_msg].str : "- empty -")
               );

        __vcslog_a_free(data);
    }
    data->id = ENUM_EMPTY;
    return 0;
}

static int __cb_attribute(void *v, int depth, char* name, char* value)
{
    vcslog_a *data = (vcslog_a*)v;
    (void) depth;

    if (
        (data->id == ENUM_logentry) &&
        (!strncmp(name, __attr_vcslog[0].str, __attr_vcslog[0].sz)) &&
        ((data->field[data->id].sz  = strlen(value)))
    )
    {
        string_free(&data->field[data->id]);
        if (!string_append_auto(&data->field[data->id],value))
            return -1;
    }
    return 0;
}

static int __cb_text(void *v, int depth, char *text)
{
    size_t sz;
    vcslog_a *data = (vcslog_a*)v;
    (void) depth;

    if (
        (data->id != ENUM_EMPTY) &&
        ((sz  = strlen(text)))
    )
    {
        switch(data->id)
        {
        case ENUM_date:
        {
            string_s src = { text, sz };
            string_free(&data->field[data->id]);

            if (!(data->field[data->id].str = string_timeformat_s(&src, "%Y-%m-%dT%H:%M:%S%Ez","%d %B %Y %H:%M")))
            {
                if (!string_append(&data->field[data->id], text, sz))
                    return -1;
            }
            else
            {
                data->field[data->id].sz = strlen(data->field[data->id].str);
            }
            break;
        }
        default:
        {
            string_free(&data->field[data->id]);
            if (!string_append(&data->field[data->id], text, sz))
                return -1;
            break;
        }
        }
    }
    return 0;
}

bool_t spch_xmllog(paths_t *dirs, FILE *fpin, FILE *fpout)
{
    int ret;
    vcslog_a vlc;
    memset(&vlc, 0, sizeof(vcslog_a));
    vlc.dirs = dirs;

    if (!(vlc.fp = ((fpout) ? fpout : dirs->fp[PATHS_FILE_OUT])))
        return R_NEGATIVE;

    if (!fpin)
        return R_NEGATIVE;

    ret = xmlpf(fpin,
                &vlc,
                __cb_opentag,
                __cb_closetag,
                __cb_attribute,
                NULL,
                __cb_text
               );

    __vcslog_a_free(&vlc);

    return ((ret == -1) ? R_NEGATIVE :
            ((!ret) ? R_TRUE : R_FALSE)
           );
}
