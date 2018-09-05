[![Build Travis](https://travis-ci.com/ClnViewer/Split-post-commit-Hook---SVN-GIT-HG.svg)](https://travis-ci.com/ClnViewer/Split-post-commit-Hook---SVN-GIT-HG)
[![Build Appveyor](https://ci.appveyor.com/api/projects/status/bvq8v28e5lhj0l2i?svg=true)](https://ci.appveyor.com/project/ClnViewer/split-post-commit-hook-svn-git-hg)
[![License MIT](https://img.shields.io/badge/License-MIT-brightgreen.svg?style=flat)](https://github.com/ClnViewer/Split-post-commit-Hook---SVN-GIT-HG/blob/master/LICENSE)

# Split post commit Hook - SVN/GIT/HG

## пример использования на репозитории SVN

1. `Условия`: Большой проект где много сопутствующих разработке данных не предназначенных для публикации.
2. `Задача`: Использовать `очищенный проект`, публиковать только материалы согласно списку файлов.

В таком случае возможно использовать опцию игнорирования файлов, но если в проекте много тестовых материалов различных форматов, то поддерживать игнорируемый список весьма затруднительно. Особенно в `SVN` репозитории.

Примерная схема возможного проекта:

    /MyProjRootDirectory
      /docsDirectory
         INSTALL.md
      /buildDirectory
         CMakeLists.txt
      /srcDirectory
         function1.php
         function2.php
         function2_test.php
         function3.php
         function3_test.php
         ...
         function100.php
         testZipRequest.zip
		 testRtfInput.rtf
		 testPdfInput.pdf
      /testDirectory
         AllFunctionTest.php


Создаем список, расположим его `/buildDirectory/split-repo.list`:

    srcDirectory/function1.php
    srcDirectory/function2.php
    srcDirectory/function3.php
    ...
    srcDirectory/function100.php

Отредактируем `hook` `post-commit` в репозитории относящимся к копии `MyProjRootDirectory`:

    # пример для `svn`, файл находиться в директории текущего svn
    # репозитория в подпапке `hooks`.
    # вносим изменения:

    /usr/bin/spch \
       -u svn \
       -e /usr/bin \
       -r "${2}" \
       -t svn \
       -m /home/user/MyProjRootDirectory \
       -s /home/user/__SplitRepo/MyProjRootDirectory_splitRelease \
       -l buildDirectory/split-repo.list \
       -j /home/svn/spch_splitRelease.log \
       -x Release_

Создаем `MyProjRootDirectory_splitRelease` Репозиторий в директории доступной `SVN` серверу:

    sudo -u svnadmin create /home/svn/MyProjRootDirectory_splitRelease

Создаем `MyProjRootDirectory_splitRelease` рабочую копию Репозитория с которой будет работать `spch` в директории доступной пользователю, необходимо чтоб директория принадлежала пользователю `svn`:

    cd /home/user/__SplitRepo
	shown -R svn:svn /home/user/__SplitRepo
    sudo -u svn checkout file:///home/svn/MyProjRootDirectory_splitRelease

Директория `/home/user/__SplitRepo` должна быть доступна на запись пользователю `svn`.

Теперь при изменении файлов в проекте библиотеки, файлы включенные в список будут автоматически копироваться в Репозиторий `MyProjRootDirectory_splitRelease`.

> Подключение к проекту разумно делать через `extended` атрибут для `SVN`,  
> или через `submodule` для `GIT`.   

Ключ __`-x`__ позволит добавлять к файлам указанный префикс, например: `srcDirectory/function1.php`  `->`  `srcDirectory/Release_function1.php`. 


## License

_MIT_

