
# Split post commit Hook - `SVN`/`GIT`/`HG`

**SPLIT POST COMMIT HOOK** позволяет создавать и поддерживать новый Репозиторий `SVN`/`GIT`/`HG` на основании списка файлов. Может использоваться для различных решений автоматизации.

## возможности и особенности

- поддерживает автоматическую работу с наиболее популярными `VCS`: `SVN`/`GIT`/`HG`.
- автоматически выполняет необходимые `VCS` команды по обновлению, изменению и публикации репозитория, в зависимости от прошедших обновлений.
- копирует файлы согласно списка. Список должен находиться в директории `master` репозитория, его путь указывается относительно корня репозитория.
- как автоматически, так и с помощью ключа __`-u`__ устанавливает пользователя и группу на создаваемые директории и файлы. Опция работает только для `POSIX` совместимых операционных систем. 
- с помощью ключа __`-x`__ можно задавать или суффикс к копируемым файлам, или выражение замены части имени файла на другое.
- с помощью ключа __`-j`__ можно задать имя лог файла для дальнейшего анализа его работы.
- с помощью ключа __`-d`__ указывается скрипт или программа проверки изменений, например сборка проекта и иные тесты. В случае успешного выполнения скрипта, он должен вернуть `0`, иначе `commit` произведен не будет.
- при публикации `split`репозитория, указывает в комментариях ревизию основного `master` репозитория и собственную версию. Это облегчает дальнейший поиск и анализ логирования транзакций.

## примеры использования

- Проект [`Большая библиотека`](docs/EXAMPLE-USE1.md)  создание `разделенных частей` библиотеки.
- Проект [`Очищенный релиз`](docs/EXAMPLE-USE2.md)  создание `очищенного` релиза для публикации.

## Ключи командной строки

         -m, --master 'val'     master репозиторий, откуда копируем
         -s, --split 'val'      split репозиторий, куда копируем
         -l, --list 'val'       список копируемых файлов, задаеться от
                                корня master репозитория
         -o, --rootsvn 'val'    корневая директория SVN/GIT/HG
                                репозиториев, необязательный параметр
         -e, --execdir 'val'    binary execute VCS directory
         -j, --log 'val'        путь и имя файла логирования
         -u, --suid 'val'       установить пользователя для файловых
                                операций (только Linux)
         -d, --deploy 'val'     deploy сценарий (скрипт), выполняеться
                                перед операцией `commit`, возвращает `0`
                                если тесты/сборка прошла удачна,
                                в противном случае `commit` выполнен
                                не будет. (опционально)
         -x, --rename 'val'     переименование или добавление префикса к
                                копируемым файлам: 'old=new' или 'new' 
         -t, --vcs 'val'        тип используемой VCS: svn|git|hq
         -c, --check 'val'      проверять обновляемые файлы:
                                ctime - дате создания,
                                mtime - дате модификации,
                                size - по размеру,
                                all - по всем выше перечисленным
                                параметрам, значение по умолчанию
         -r, --revision 'val'   установить текущую ревизию копий файлов
                                относительно master репозитория
         -f, --force            перезаписывать все файлы, не проверяя
                                на изменения 
         -q, --quiet            'тихий режм', не выводить ничего на
                                консоль, работает только после обработки
                                параметров конфигурации заданных из
                                командной строки
         -h, --help             показать помощь по командам


## Сборка и Инсталляция

Сборка производиться системой `Cmake` и не отличается по шагам от типовых случаев. Порядок действий:

1. Получите дистрибутив исходных кодов `spch` любым удобным для вас способом: [релиз]() | [текущая версия]()
2. Распакуйте полученный архив и перейдите в директорию `build`.
3. Для `Windows` платформы запустите `cmake-build-mingw.cmd`, для `POSIX` систем запустите `cmake-build-gcc.sh`.
4. Если все прошло удачно, и сборка прошла без ошибок, исполняемый файл `spch` или `spch.exe` вы найдете в директории `build\bin`.
5. Для `POSIX` систем в пакетный файл сборки включена команда `install`, если вы не переопределяли путь, исполняемый файл будет скопирован в директорию `/usr/local/bin`.

> Для `Windows` платформы используйте компилятор `MinGW`, для `POSIX` платформ должен используется компилятор `gcc` или `clang`. Возможно собрать `spch` в `MSVC/MSVS`, для этого используйте пакетный файл `cmake-build-msvc.cmd`.
> По умолчанию для `MSVC/MSVS` сборка указана для `Visual Studio 15 2017`. 

Запускаться `spch` должен из скрипта `VCS` `post-commit`, в зависимости от типа `VCS`. Файлы хуков `VCS` могут иметь отличные имена и располагаться согласно документации.  

    # пример для `svn`, файл `post-commit` находиться в директории
    # текущего svn репозитория в подпапке `hooks`.

    # структура `svn` репозитория:
        conf/
        db/
        hooks/
        locks/
        README.txt
        format

    # вносим изменения в hooks/post-commit:
    /usr/bin/spch \
       -u svn \
       -e /usr/bin \
       -r "${1}" \
       -t svn \
       -o /home/svn/repo \
       -m /home/user/MyLibRootRepo \
       -s /home/user/__SplitRepo/MyLibSplitRepo_auto \
       -l buildDirectory/split-repo.list \
       -j /home/svn/spch_MyLibSplit.log \
       -x mylib=libexport

## License

_MIT_

