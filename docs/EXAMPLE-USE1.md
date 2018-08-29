
# Split post commit Hook - SVN/GIT/HG

## пример использования на репозитории SVN

1. `Условия`: Большая библиотека с количеством функций более 500 и более 100 файлов.
2. `Задача`: Использовать определенный набор функций, предположим 10 в другом проекте.

В таком случае присоединять полностью библиотеку не целесообразно, а копировать отдельные файлы в ваш проект из библиотеки грозит сложностью поддержки и синхронизацией изменений в них.

Примерная схема возможной библиотеки:

    /MyLibRootDirectory
      /docsDirectory
         INSTALL.md
      /buildDirectory
         CMakeLists.txt
      /srcDirectory
         mylib_function1.cpp
         mylib_function2.cpp
         mylib_function3.cpp
         ...
         mylib_function100.cpp
      /testDirectory
         AllFunctionTest.cpp

Нам для работы в новом проекте необходимы только файлы `function2.cpp`, `function5.cpp`.  

Создаем список, расположим его `/buildDirectory/split-repo.list`:

    srcDirectory/mylib_function2.cpp
    srcDirectory/mylib_function5.cpp

Отредактируем `hook` `post-commit` в репозитории относящимся к  копии `MyLibRootDirectory`:

    # пример для `svn`, файл находиться в директории текущего svn
    # репозитория в подпапке `hooks`.
    # вносим изменения:

    /usr/bin/spch \
       -u svn \
       -e /usr/bin \
       -r "${2}" \
       -t svn \
       -m /home/user/MyLibRootDirectory \
       -s /home/user/__SplitRepo/MyLibSplitRepoDirectory_split25 \
       -l buildDirectory/split-repo.list \
       -j /home/svn/spch_split25.log \
       -x mylib=lib25

Создаем `MyLibSplitRepoDirectory_split25` Репозиторий в директории доступной `SVN` серверу:

    sudo -u svnadmin create /home/svn/MyLibSplitRepoDirectory_split25

Создаем `MyLibSplitRepoDirectory_split25` рабочую копию Репозитория с которой будет работать `spch` в директории доступной пользователю, разумнее всего чтоб директория принадлежала пользователю `svn`:

    cd /home/user/__SplitRepo
	shown -R svn:svn /home/user/__SplitRepo
    sudo -u svn checkout file:///home/svn/MyLibSVNRepoDirectory_split25

Директория `/home/user/__SplitRepo` должна быть доступна на запись пользователю `svn`.

Теперь при изменении файлов в проекте библиотеки, файлы `function2.cpp`, `function5.cpp` будут автоматически копироваться в Репозиторий `MyLibSplitRepoDirectory_split25`.

> Подключение к проекту разумно делать через `extended` атрибут для `SVN`,  
или через `submodule` для `GIT`.   

Ключ __`-x`__ позволит переименовать файлы в `srcDirectory/lib25_function2.cpp`. 
    

## License

_MIT_

