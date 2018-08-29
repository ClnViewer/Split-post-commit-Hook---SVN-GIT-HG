#include "spch.h"
#include "spch-link-hash.h"
#define __ISLOG ((dirs->fp[1]) ? 1 : 0)

static void __stage2_vcs_add(unsigned int cnt, unsigned long hash, char *str, size_t sz, void * data)
{
    int ret;
    paths_t *dirs = (paths_t*)data;
    string_s sdir = { str, 0U };
    (void) hash;
    (void) sz;

    if ((ret = pch_vcs_aad(dirs, &sdir)) != 0)
    {
        pch_log_error(dirs, "add VCS directory error: %d -> %u) %s", ret, cnt, sdir.str);
    }
}

int pch_stage1(paths_t *dirs)
{
    int ret;
    if ((ret = pch_vcs_update(dirs)) != 0)
    {
        pch_log_error(dirs, "update VCS error: %d", ret);
        return 0;
    }
    return 1;
}

int pch_stage2(paths_t *dirs)
{
    int rcode = 0, stage = 0;
    char b[BUFSIZ + 1] = {0};
    hash_t *hd, *hf;
    hd = hash_init();
    hf = hash_init();

    while (fgets(b, BUFSIZ, dirs->fp[0]) != NULL)
    {
        int ret;
        string_s from_s, to_s, to_dir_s;
        string_s __AUTO(__autostring)
        *from_free = &from_s,
         *to_free = &to_s,
          *to_dir_free = &to_dir_s;

        memset(&from_s,   0, sizeof(string_s));
        memset(&to_s,     0, sizeof(string_s));
        memset(&to_dir_s, 0, sizeof(string_s));

        size_t len = strlen(b);
        len = ((len > 0U) ? (len - 1U) : 0U);

        if (!len)
            continue;

        b[len] = '\0';

        /* skip duplicate */
        if (hf->searchs(hf->hash, b, len))
            continue;

        (void) hf->adds(hf->hash, b, len);

        if (
            (!pch_path_format(&from_s, "%s" __PSEPS "%s", dirs->setup[FILE_MASTER_REPO].str, b)) ||
            (!pch_path_destination(dirs, (char*)b, len, &to_s))
        )
        {
            rcode = -1;
            break;
        }

        printf("%d) [%s]: %s -> %s\n", __LINE__, __func__, from_s.str, to_s.str);

        if (!__BITTST(dirs->bitopt, OPT_FORCE))
        {
            if (!pch_compare_file(dirs, &from_s, &to_s))
            {
                if (__ISLOG)
                {
                    pch_log_info(dirs, "stage #2 skip: %s -> %s", from_s.str, to_s.str);
                }
                continue;
            }
        }
        if (strchr(b, __PSEPC))
        {
            switch (pch_path_dir(&to_dir_s, &to_s))
            {
            case -1:
            {
                rcode = -2;
                break;
            }
            case  0:
            {
                rcode = 0;
                break;
            }
            default:
            {
                if (!hd->searchs(hd->hash, to_dir_s.str, to_dir_s.sz))
                {
                    (void) hd->addmap(hd->hash, to_dir_s.str, to_dir_s.sz);
                }
                if (!pch_check_dir(&to_dir_s))
                {
                    errno = 0;

                    if (
                        (_mkdir(to_dir_s.str) < 0) &&
                        (errno != EEXIST)
                    )
                    {
                        pch_log_error(dirs, "create directory error: %s", to_dir_s.str);
                        rcode = -3;
                        break;
                    }
                }
            }
            }
        }
        if (rcode < 0)
            break;

        printf("%d) [%s]: %s -> %s -> %s\n",
               __LINE__, __func__, from_s.str, to_s.str,
               ((to_dir_s.str) ? to_dir_s.str : "- none -")
              );

        if ((ret = pch_fcopy(&from_s, &to_s)))
        {
            pch_log_error(dirs, "copy file error: %s -> %s", from_s.str, to_s.str);
            continue;
        }
        if (__ISLOG)
        {
            pch_log_info(dirs, "stage #2 %s: %s -> %s",
                         ((__BITTST(dirs->bitopt, OPT_FORCE)) ? "replace" : "update"),
                         from_s.str, to_s.str
                        );
        }
        stage = ((!ret) ? (stage + 1) : stage);
    }

    if ((!rcode) && (stage > 0))
    {
        /* needed to add exist directory before */
        if (pch_vcs_aad(dirs, &dirs->setup[FILE_SPLIT_REPO]) != 0)
        {
            pch_log_error(dirs, "add VCS root directory error: -> %s", dirs->setup[FILE_SPLIT_REPO].str);
        }
        /* add all files from directory */
        hd->getmap(hd->hash, (void*)dirs, &__stage2_vcs_add);
    }

    hf->free(hf->hash);
    free(hf);
    hd->free(hd->hash);
    free(hd);

    return ((rcode < 0) ? rcode : stage);
}

int pch_stage3(paths_t *dirs, int status)
{
    int ret;
    if (status <= 0)
        return 0;

    if ((ret = pch_vcs_aad(dirs, &dirs->setup[FILE_SPLIT_REPO])) != 0)
    {
        pch_log_error(dirs, "add VCS root directory error: %d -> %s", ret, dirs->setup[FILE_SPLIT_REPO].str);
    }
    if ((ret = pch_vcs_commit(dirs)) != 0)
    {
        pch_log_error(dirs, "commit VCS error: %d", ret);
        return -1;
    }
    return 1;
}
