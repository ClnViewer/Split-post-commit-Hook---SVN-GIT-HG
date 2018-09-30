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

#if !defined(SPCH_YAML_H)
#define SPCH_YAML_H

#define __YAMLNAME "deploy.yaml"

/*
    original bash yaml_parse author: Jonathan Peres
    Git: https://github.com/jasperes/bash-yaml
    Based on https://gist.github.com/pkuczynski/8665367
*/

static const char yamlscr[] =
{
    "export PATH=/usr/local/sbin:/usr/local/bin:/sbin:/bin:/usr/sbin:/usr/bin\n"

    "function parse_yaml() {\n"
    "    local yaml_file=$1\n"
    "    local prefix=$2\n"
    "    local s\n"
    "    local w\n"
    "    local fs\n"
    "    s='[[:space:]]*'\n"
    "    w='[a-zA-Z0-9_.-]*'\n"
    "    fs=\"$(echo @|tr @ '\\034')\"\n"
    "    (\n"
    "        sed -ne '/^--/s|--||g; s|\\\"|\\\\\\\"|g; s/\\s*$//g;' \\\n"
    "            -e \"/#.*[\\\"\\']/!s| #.*||g; /^#/s|#.*||g;\" \\\n"
    "            -e  \"s|^\\($s\\)\\($w\\)$s:$s\\\"\\(.*\\)\\\"$s\\$|\\1$fs\\2$fs\\3|p\" \\\n"
    "            -e \"s|^\\($s\\)\\($w\\)$s[:-]$s\\(.*\\)$s\\$|\\1$fs\\2$fs\\3|p\" |\n"
    "        awk -F\"$fs\" '{\n"
    "            indent = length($1)/2;\n"
    "            if (length($2) == 0) { conj[indent]=\"+\";} else {conj[indent]=\"\";}\n"
    "            vname[indent] = $2;\n"
    "            for (i in vname) {if (i > indent) {delete vname[i]}}\n"
    "                if (length($3) > 0) {\n"
    "                    vn=\"\"; for (i=0; i<indent; i++) {vn=(vn)(vname[i])(\"_\")}\n"
    "                    printf(\"%s%s%s%s=(\\\"%s\\\")\\n\", \"'\"$prefix\"'\",vn, $2, conj[indent-1],$3);\n"
    "                }\n"
    "            }' |\n"
    "        sed -e 's/_=/+=/g' |\n"
    "        awk 'BEGIN {\n"
    "                 FS=\"=\";\n"
    "                 OFS=\"=\"\n"
    "             }\n"
    "             /(-|\\.).*=/ {\n"
    "                 gsub(\"-|\\\\.\", \"_\", $1)\n"
    "             }\n"
    "             { print }'\n"
    "    ) < \"$yaml_file\"\n"
    "}\n"

    "    if [[ \"${0}\" == \"\" ]] ;\n"
    "    then\n"
    "        echo \"Unknown ROOT repo\"\n"
    "        exit 1;\n"
    "    else\n"
    "        _ROOT=\"${0}\"\n"
    "    fi\n"

    "    if [[ \"${1}\" == \"\" ]] ;\n"
    "    then\n"
    "        echo \"Unknown VCS REVISION\"\n"
    "        exit 1;\n"
    "    else\n"
    "        _VCS_REVISION=\"${1}\"\n"
    "    fi\n"

    "    if [[ \"${2}\" == \"\" ]] ;\n"
    "    then\n"
    "        echo \"Unknown VCS TYPE\"\n"
    "        exit 1;\n"
    "    else\n"
    "        _VCS_TYPE=\"${2}\"\n"
    "    fi\n"

    "    if [[ \"${3}\" == \"\" ]] ;\n"
    "    then\n"
    "        echo \"Unknown REPO name\"\n"
    "    else\n"
    "        _REPO_NAME=\"${3}\"\n"
    "    fi\n"

    "    if [[ \"${4}\" == \"\" ]] ;\n"
    "    then\n"
    "        echo \"Backup file not present\"\n"
    "    else\n"
    "        _BACKUP_ZIP=\"${4}\"\n"
    "    fi\n"

    "    if [[ ! -f \"${_ROOT}/" __YAMLNAME "\" ]] ;\n"
    "    then\n"
    "        echo \"ROOT not found yaml config: ${_ROOT}/" __YAMLNAME "\"\n"
    "        exit 1;\n"
    "    fi\n"

    "    echo -e \"\tSetup -> Root directory: ${_ROOT}\"\n"
    "    echo -e \"\tSetup -> VCS revision: ${_VCS_REVISION}\"\n"
    "    echo -e \"\tSetup -> VCS type: ${_VCS_TYPE}\\n\"\n"
    "    cd \"${_ROOT}/\"\n"

    "    eval $(parse_yaml \"${_ROOT}/" __YAMLNAME "\")\n"
    "    _RC=\"0\"\n"

    "    for i in \"${pre[@]}\" ;\n"
    "    do\n"
    "        echo -e \"\tPre-build cmd: ${i}\";\n"
    "        ${i}\n"
    "    done\n"

    "    for i in \"${build[@]}\" ;\n"
    "    do\n"
    "        echo -e \"\tExamine cmd: ${i}\";\n"
    "        ${i}\n"
    "        _RC=$?\n"
    "        if [[ \"${_RC}\" != \"0\" ]] ;\n"
    "        then\n"
    "            echo -e \"\t${_VCS_TYPE} rev.${_VCS_REVISION} - Examine error: ${_RC}\"\n"
    "            break\n"
    "        fi\n"
    "    done\n"

    "    echo -e \"\tAll Tests return ${_RC}\"\n"

    "    for i in \"${post[@]}\" ;\n"
    "    do\n"
    "        echo -e \"\tPost-build cmd: ${i}\";\n"
    "        ${i}\n"
    "    done\n"
    "    exit ${_RC}\n"

};

#endif
