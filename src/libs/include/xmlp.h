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

#if !defined(XML_P_H)
#define XML_P_H

typedef enum __tag_XMLERR
{
    XMLERR_OK = 0,
    XMLERR_PARAM,
    XMLERR_OPENTAG,
    XMLERR_CLOSETAG,
    XMLERR_ATTRIBUTE,
    XMLERR_UNMATCH,
} XMLERR;

int xmlpb(char*,
         void*,
         int (*xml_onopentag_cb)(void*, int, char*),
         int (*xml_onclosetag_cb)(void*, int, char*),
         int (*xml_onattribute_cb)(void*, int, char*, char*),
         int (*xml_onendattribute_cb)(void*, int),
         int (*xml_ontext_cb)(void*, int, char*)
        );

int xmlpf(FILE*,
         void*,
         int (*xml_onopentag_cb)(void*, int, char*),
         int (*xml_onclosetag_cb)(void*, int, char*),
         int (*xml_onattribute_cb)(void*, int, char*, char*),
         int (*xml_onendattribute_cb)(void*, int),
         int (*xml_ontext_cb)(void*, int, char*)
        );

#endif
