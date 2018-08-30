/*
    MIT License

    Copyright (c) 2018 PS
    GitHub: https://github.com/ClnViewer/LinkedHashMap
    See example: https://github.com/ClnViewer/LinkedHashMap/blob/master/test/main.c

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

#if !defined(LINK_HASH_MAP_H)
#define LINK_HASH_MAP_H

typedef struct __hash_tbl
{
    unsigned long val;
    struct
    {
        char *str;
        size_t sz;
    } string;
    struct __hash_tbl *next;
} hash_tbl;

/*!
    CallBack used for (hash_t*)->getmap(...)
    format:
      unsigned int - counter,
      unsigned long - hash value,
      char*  - string,
      size_t - string size,
      void*  - user data
 */
typedef void (*hash_map_cb)(unsigned int, unsigned long, char*, size_t, void*);

typedef struct __hash_t
{
    hash_tbl      **hash;
    unsigned long (*hahs)(const char*, size_t);
    unsigned long (*add)(hash_tbl**, unsigned long);
    unsigned long (*adds)(hash_tbl**, const char*, size_t);
    unsigned long (*addmap)(hash_tbl**, const char*, size_t);
    void          (*getmap)(hash_tbl**, void*, hash_map_cb);
    int           (*del)(hash_tbl**, hash_tbl*);
    int           (*deln)(hash_tbl**, unsigned long);
    int           (*dels)(hash_tbl**, const char*, size_t);
    int           (*sort)(hash_tbl**);
    hash_tbl    * (*search)(hash_tbl**, unsigned long);
    unsigned long (*searchs)(hash_tbl**, const char*, size_t);
    const char  * (*searchmap)(hash_tbl**, unsigned long);
    unsigned long (*count)(hash_tbl**);
    void          (*free)(hash_tbl**);
    void          (*dump)(hash_tbl**);

} hash_t;

/*!
    Main one function: create `hashList` instance.
    Other `hashList` method available in structure `hash_t`.
*/
hash_t * hash_init(void);

#endif
