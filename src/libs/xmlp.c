/* Copyright (c) 2010, huxingyi@msn.com
 * Copyright (c) 2018 PS (rewriting)
 * GitHub: https://github.com/ClnViewer/Split-post-commit-Hook---SVN-GIT-HG
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <spch.h>
#include <xmlp.h>

#if defined(OS_WIN)
// #pragma warning(disable:4013)
#endif

#define XML_OPENTAG_NAME    10
#define XML_CLOSETAG_NAME   20
#define XML_ATTRIBUTE_PRE   30
#define XML_ATTRIBUTE_NAME  40
#define XML_ATTRIBUTE_VALUE 50
#define XML_ATTRIBUTE_EQUAL 60
#define XML_TEXT            70
#define XML_CDATA           80
#define XML_COMMENT         90

#define XML_COMMENT_OPEN_TAG  "<!--"
#define XML_COMMENT_CLOSE_TAG "-->"
#define XML_HEAD_OPEN_TAG     "<?"
#define XML_CDATA_OPEN_TAG    "<![CDATA["
#define XML_CDATA_CLOSE_TAG   "]]>"
#define XML_TEXT_CLOSE_TAG    "</"

#define __xml_isname(_c) ( (_c) > 32 && (_c) < 122 && isalnum(_c) )

int xmlpf(FILE *fp,
          void *param,
          int (*xml_onopentag_cb)(void*, int, char*),
          int (*xml_onclosetag_cb)(void*, int, char*),
          int (*xml_onattribute_cb)(void*, int, char*, char*),
          int (*xml_onendattribute_cb)(void*, int),
          int (*xml_ontext_cb)(void*, int, char*)
         )
{
    long ssz;
    char __AUTO(__autofree) *s_xml = NULL;

    if (!fp)
    {
        errno = EINVAL;
        return R_NEGATIVE;
    }
    if (
        (fseek(fp, 0L, SEEK_END)) ||
        (!(ssz = ftell(fp)))
    )
    {
        errno = ((errno) ? errno : ENFILE);
        return R_NEGATIVE;
    }

#   if defined(BUILD_MSVC)
    __try
    {
#   endif

        if (!(s_xml = (char*) calloc(1, (size_t)(ssz + 1L))))
            return R_NEGATIVE;

        ;
        if (
            (fseek(fp, 0L, SEEK_SET)) ||
            (fread(s_xml, 1U, (size_t)ssz, fp) == 0U)
        )
            return R_NEGATIVE;

        (void) fseek(fp, 0L, SEEK_SET);

        return xmlpb(
                     s_xml,
                     param,
                     xml_onopentag_cb,
                     xml_onclosetag_cb,
                     xml_onattribute_cb,
                     xml_onendattribute_cb,
                     xml_ontext_cb
                 );

#   if defined(BUILD_MSVC)
    }
    __finally
    {
        if (s_xml)
        {
            __autofree(&s_xml);
        }
    }
#   endif

}

int xmlpb(char *xml,
          void *param,
          int (*xml_onopentag_cb)(void*, int, char*),
          int (*xml_onclosetag_cb)(void*, int, char*),
          int (*xml_onattribute_cb)(void*, int, char*, char*),
          int (*xml_onendattribute_cb)(void*, int),
          int (*xml_ontext_cb)(void*, int, char*)
         )
{
    int status = XML_TEXT,
        cb_return = 0,
        cb_depth = 0,
        cb_moved = 0;
    char tmp = 0,
         tmp2 = 0,
         *attr_name_start = 0,
          *attr_name_stop = 0,
           *attr_value_start = 0,
            *attr_value_stop = 0,
             *opentag_start = 0,
              *opentag_stop = 0,
               *closetag_start = 0,
                *closetag_stop = 0,
                 *text_start = 0,
                  *text_stop = 0,
                   *p = xml;

    if (0 == p)
    {
        return XMLERR_PARAM;
    }

    while (0 != p &&
            '\0' != (*p))
    {
        //printf("%c ", *p);

        cb_moved = 0;

        switch (*p)
        {
        case '<': // <? <foo </foo <!-- <![CDATA[
        {
            if (XML_TEXT == status)
            {
                if (0 == strncmp(p,
                                 XML_CDATA_OPEN_TAG,
                                 strlen(XML_CDATA_OPEN_TAG))) // <![CDATA[
                {
                    status = XML_CDATA;
                    p += strlen(XML_CDATA_OPEN_TAG);
                    cb_moved = 1;
                    text_start = p;
                }
                else if (0 == strncmp(p,
                                      XML_COMMENT_OPEN_TAG,
                                      strlen(XML_COMMENT_OPEN_TAG))) // <!--
                {
                    status = XML_COMMENT;
                    p += strlen(XML_COMMENT_OPEN_TAG);
                    cb_moved = 1;
                }
                else if (0 == strncmp(p,
                                      XML_TEXT_CLOSE_TAG,
                                      strlen(XML_TEXT_CLOSE_TAG))) // </foo
                {
                    status = XML_CLOSETAG_NAME;
                    text_stop = p;
                    if (0 != text_start && 0 != text_stop)
                    {
                        tmp = *text_stop;
                        *text_stop = '\0';
                        cb_return = ((xml_ontext_cb) ?
                                     xml_ontext_cb(param, cb_depth, text_start) : 0
                                    );

                        *text_stop = tmp;
                        if (0 != cb_return)
                        {
                            return cb_return;
                        }
                        text_start = 0;
                    }
                    text_stop = 0;
                    p += strlen(XML_TEXT_CLOSE_TAG);
                    cb_moved = 1;
                    closetag_start = p;
                }
                else if (0 == strncmp(p,
                                      XML_HEAD_OPEN_TAG,
                                      strlen(XML_HEAD_OPEN_TAG))) // <?
                {
                    status = XML_OPENTAG_NAME;
                    p += strlen(XML_HEAD_OPEN_TAG);
                    cb_moved = 1;
                    opentag_start = p;
                }
                else // <foo
                {
                    status = XML_OPENTAG_NAME;
                    opentag_start = p + 1;
                }
            }
        }
        break;
        case '?': // ?>
        case '/': // />
        {
            if (XML_ATTRIBUTE_PRE == status)
            {
                if (0 != (p+1) && '>' == (*(p+1)))
                {
                    cb_return = ((xml_onendattribute_cb) ?
                                 xml_onendattribute_cb(param, cb_depth) : 0
                                );

                    if (0 != cb_return)
                    {
                        return cb_return;
                    }
                    status = XML_TEXT;
                    p += 2;
                    cb_moved = 1;
                    cb_return = ((xml_onclosetag_cb) ?
                                 xml_onclosetag_cb(param, cb_depth, "") : 0
                                );

                    if (0 != cb_return)
                    {
                        return cb_return;
                    }
                    --cb_depth;
                }
            }
            else if (XML_OPENTAG_NAME == status)
            {
                if (0 != (p+1) && '>' == (*(p+1)))
                {
                    status = XML_TEXT;
                    opentag_stop = p;
                    p += 2;
                    cb_moved = 1;
                    if (0 == opentag_start ||
                            0 == opentag_stop)
                    {
                        return XMLERR_OPENTAG;
                    }
                    ++cb_depth;
                    tmp = *opentag_stop;
                    *opentag_stop = '\0';
                    cb_return = ((xml_onopentag_cb) ?
                                 xml_onopentag_cb(param, cb_depth, opentag_start) : 0
                                );

                    *opentag_stop = tmp;
                    if (0 != cb_return)
                    {
                        return cb_return;
                    }
                    opentag_start = 0;
                    opentag_stop = 0;
                    cb_return = ((xml_onendattribute_cb) ?
                                 xml_onendattribute_cb(param, cb_depth) : 0
                                );

                    if (0 != cb_return)
                    {
                        return cb_return;
                    }
                    cb_return = ((xml_onclosetag_cb) ?
                                 xml_onclosetag_cb(param, cb_depth, "") : 0
                                );


                    if (0 != cb_return)
                    {
                        return cb_return;
                    }
                    --cb_depth;
                }
                else
                {
                    return XMLERR_OPENTAG;
                }
            }
        }
        break;
        case '>':
        {
            if (XML_ATTRIBUTE_PRE == status)
            {
                cb_return = ((xml_onendattribute_cb) ?
                             xml_onendattribute_cb(param, cb_depth) : 0
                            );

                if (0 != cb_return)
                {
                    return cb_return;
                }
                status = XML_TEXT;
                text_start = p + 1;
            }
            else if (XML_CLOSETAG_NAME == status)
            {
                status = XML_TEXT;
                closetag_stop = p;
                if (0 == closetag_start ||
                        0 == closetag_stop)
                {
                    return XMLERR_CLOSETAG;
                }
                tmp = *closetag_stop;
                *closetag_stop = '\0';
                cb_return = ((xml_onclosetag_cb) ?
                             xml_onclosetag_cb(param, cb_depth, closetag_start) : 0
                            );

                *closetag_stop = tmp;
                if (0 != cb_return)
                {
                    return cb_return;
                }
                --cb_depth;
                closetag_start = 0;
                closetag_stop = 0;
            }
            else if (XML_OPENTAG_NAME == status)
            {
                status = XML_TEXT;
                text_start = p + 1;
                opentag_stop = p;
                if (0 == opentag_start ||
                        0 == opentag_stop)
                {
                    return XMLERR_OPENTAG;
                }
                ++cb_depth;
                tmp = *opentag_stop;
                *opentag_stop = '\0';
                cb_return = ((xml_onopentag_cb) ?
                             xml_onopentag_cb(param, cb_depth, opentag_start) : 0
                            );

                *opentag_stop = tmp;
                if (0 != cb_return)
                {
                    return cb_return;
                }
                opentag_start = 0;
                opentag_stop = 0;
                cb_return = ((xml_onendattribute_cb) ?
                             xml_onendattribute_cb(param, cb_depth) : 0
                            );

                if (0 != cb_return)
                {
                    return cb_return;
                }
            }
        }
        break;
        case '"':
        {
            if (XML_ATTRIBUTE_EQUAL == status)
            {
                status = XML_ATTRIBUTE_VALUE;
                attr_value_start = p + 1;
            }
            else if (XML_ATTRIBUTE_VALUE == status)
            {
                status = XML_ATTRIBUTE_PRE;
                attr_value_stop = p;
                if (0 == attr_name_start
                        || 0 == attr_name_stop
                        || 0 == attr_value_start
                        || 0 == attr_value_stop)
                {
                    return XMLERR_ATTRIBUTE;
                }
                tmp = *attr_name_stop;
                tmp2 = *attr_value_stop;
                *attr_name_stop = '\0';
                *attr_value_stop = '\0';
                cb_return = ((xml_onattribute_cb) ?
                             xml_onattribute_cb(param, cb_depth, attr_name_start, attr_value_start) : 0
                            );

                *attr_name_stop = tmp;
                *attr_value_stop = tmp2;
                if (0 != cb_return)
                {
                    return cb_return;
                }
                attr_name_start = 0;
                attr_name_stop = 0;
                attr_value_start = 0;
                attr_value_stop = 0;
            }
        }
        break;
        case '-': // -->
        {
            if (XML_COMMENT == status)
            {
                if (0 == strncmp(p,
                                 XML_COMMENT_CLOSE_TAG,
                                 strlen(XML_COMMENT_CLOSE_TAG)))
                {
                    status = XML_TEXT;
                    p += strlen(XML_COMMENT_CLOSE_TAG);
                    cb_moved = 1;
                }
            }
        }
        break;
        case ']': // ]]>
        {
            if (XML_CDATA == status)
            {
                if (0 == strncmp(p,
                                 XML_CDATA_CLOSE_TAG,
                                 strlen(XML_CDATA_CLOSE_TAG)))
                {
                    status = XML_TEXT;
                    text_stop = p;
                    if (0 != text_start &&
                            0 != text_stop)
                    {
                        tmp = *text_stop;
                        *text_stop = '\0';
                        cb_return = ((xml_ontext_cb) ?
                                     xml_ontext_cb(param, cb_depth, text_start) : 0
                                    );

                        *text_stop = tmp;
                        if (0 != cb_return)
                        {
                            return cb_return;
                        }
                        text_start = 0;
                    }
                    text_stop = 0;
                    p += strlen(XML_CDATA_CLOSE_TAG);
                    cb_moved = 1;
                }
            }
        }
        break;
        case '=':
        {
            if (XML_ATTRIBUTE_NAME == status)
            {
                status = XML_ATTRIBUTE_EQUAL;
                if (0 == attr_name_stop)
                {
                    attr_name_stop = p;
                }
            }
        }
        break;
        case ' ':
        case '\t':
        case '\r':
        case '\n':
        {
            if (XML_ATTRIBUTE_NAME == status)
            {
                if (0 == attr_name_stop)
                {
                    attr_name_stop = p;
                }
            }
            else if (XML_OPENTAG_NAME == status)
            {
                status = XML_ATTRIBUTE_PRE;
                opentag_stop = p;
                if (0 == opentag_start ||
                        0 == opentag_stop)
                {
                    return XMLERR_OPENTAG;
                }
                ++cb_depth;
                tmp = *opentag_stop;
                *opentag_stop = '\0';
                cb_return = ((xml_onopentag_cb) ?
                             xml_onopentag_cb(param, cb_depth, opentag_start) : 0
                            );

                *opentag_stop = tmp;
                if (0 != cb_return)
                {
                    return cb_return;
                }
                opentag_start = 0;
                opentag_stop = 0;

            }
        }
        break;
        default:
        {

            if (XML_ATTRIBUTE_PRE == status)
            {
                if (__xml_isname(*p))
                {
                    status = XML_ATTRIBUTE_NAME;
                    attr_name_start = p;
                }
            }
        }
        }
        if (!cb_moved)
        {
            ++p;
        }
    }

    if (0 != cb_depth)
    {
        return XMLERR_UNMATCH;
    }

    return XMLERR_OK;
}

