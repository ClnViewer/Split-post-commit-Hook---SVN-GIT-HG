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

bool_t pch_stage1(paths_t *dirs)
{
    int ret;

    if ((ret = pch_vcs_update(dirs, &dirs->setup[FILE_MASTER_REPO])) != 0)
    {
        if ((!dirs->fp[PATHS_FILE_OUT]) || (dirs->fpos != ftell(dirs->fp[PATHS_FILE_OUT])))
        {
            pch_log_info(dirs, "update VCS master repo loop?: %d -> %s", ret, dirs->setup[FILE_MASTER_REPO].str);
        }
    }
    if ((ret = pch_vcs_update(dirs, &dirs->setup[FILE_SPLIT_REPO])) != 0)
    {
        pch_log_error(dirs, "update VCS split repo error: %d -> %s", ret, dirs->setup[FILE_SPLIT_REPO].str);
        return R_FALSE;
    }
    return R_TRUE;
}
