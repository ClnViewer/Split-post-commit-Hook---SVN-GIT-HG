
[![Build Travis](https://travis-ci.com/ClnViewer/Split-post-commit-Hook---SVN-GIT-HG.svg)](https://travis-ci.com/ClnViewer/Split-post-commit-Hook---SVN-GIT-HG)
[![Build Appveyor](https://ci.appveyor.com/api/projects/status/bvq8v28e5lhj0l2i?svg=true)](https://ci.appveyor.com/project/ClnViewer/split-post-commit-hook-svn-git-hg)
[![License MIT](https://img.shields.io/badge/License-MIT-brightgreen.svg?style=flat)](https://github.com/ClnViewer/Split-post-commit-Hook---SVN-GIT-HG/blob/master/LICENSE)


# Split post commit Hook - `SVN`/`GIT`/`HG`

**SPLIT POST COMMIT HOOK** позволяет создавать и поддерживать новый Репозиторий `SVN`/`GIT`/`HG` на основании списка файлов. Может использоваться для различных решений автоматизации.

## возможности и особенности

- Поддерживает автоматическую работу с наиболее популярными `VCS`: `SVN`/`GIT`/`HG`.
- Автоматически выполняет необходимые `VCS` команды по обновлению, изменению и публикации репозитория, в зависимости от прошедших обновлений.
- Копирует файлы согласно списка. Список должен находиться в директории `master` репозитория, его путь указывается относительно корня репозитория.
- Отслеживает и убирает возможные дубликаты файлов и директорий в списке.
- Как автоматически, так и с помощью ключа __`-u`__ устанавливает пользователя и группу на создаваемые директории и файлы. Опция работает только для `POSIX` совместимых операционных систем. 
- Возможно задавать или суффикс к копируемым файлам, или выражение замены части имени файла на другое, с помощью ключа __`-x`__ .
- Возможность вести журнал операций для дальнейшего анализа проведенных обновлений, задать имя лог файла можно с помощью ключа __`-j`__ .
- Возможность контролировать изменения в репозитории с помощью запуска тестов и по результатам принимать решение о внесении изменений. Есть два пути, возможно использовать оба варианта в одном репозитории:
  - ключ __`-d`__ указывает на скрипт или программу проверки изменений, например сборка проекта или иные тесты.
  - ключ __`-y`__ определяет поиск конфигурационного файла `deploy.yaml` в корне `split` репозитория и выполнение команд описанных в нем.
  - В обеих случаях, успешное выполнение команд считается возврат `0`, иначе `commit` произведен не будет. Рабочие примеры скрипта [`deploy.sh`](https://github.com/ClnViewer/LinkedHashMap/blob/master/build/buildDeploy.sh) и файла [`deploy.yaml`](https://github.com/ClnViewer/LinkedHashMap/blob/master/deploy.yaml).
- При публикации `split`репозитория, указывает в комментариях ревизию основного `master` репозитория и собственную версию. Это облегчает дальнейший поиск и анализ логирования транзакций.

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
         -e, --execdir 'val'    путь к исполняемым файлам VCS,
                                для 'POSIX' систем по умолчанию /usr/bin
         -j, --log 'val'        путь и имя файла логирования
         -u, --uid 'val'        установить пользователя для файловых
                                операций (только Linux)
         -d, --deploy 'val'     deploy сценарий (скрипт), выполняеться
                                перед операцией 'commit', возвращает '0'
                                если тесты/сборка прошла удачна,
                                в противном случае 'commit' выполнен
                                не будет. (опционально)
         -y, --yaml             искать конфигурационный файл 'deploy.yaml'
                                в корне 'split' репозитория и в случае
                                успеха, выполнить команды описанные в нем.
                                Для работы необходим установленный
                                'bash shell' или 'ksh shell'(только Linux)
                                Формат 'yaml' файла описан ниже.
         -x, --rename 'val'     переименование или добавление префикса к
                                копируемым файлам: 'old=new' или 'new' 
         -t, --vcs 'val'        тип используемой VCS: svn|git|hg
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
         -k, --nonloop          предотвратить одновременный 'commit' и
                                'update' в мастер репозитории,
                                демонизирует процесс на время выполнения,
                                совместно с этим флагом крайне желательно 
                                использывать опцию записи лога '-j'  
         -q, --quiet            'тихий режм', не выводить ничего на
                                консоль, работает только после обработки
                                параметров конфигурации заданных из
                                командной строки
         -i, --info             подробное описание команд
         -h, --help             показать помощь по командам


> Внимание, при использовании в скрипте `hooks/post-commit` использование ключа `-k`, алиас `--nonloop` - **ОБЯЗАТЕЛЬНО**!  
> 
> В противном случае, например для `svn` будет происходить ошибка при попытке использовать метод `update`, репозиторий будет находиться в состоянии `.lock` от предыдущей операции `commit` из которой и вызывается данная программа.

Файл справки в [фрмате `man`](https://raw.githubusercontent.com/ClnViewer/Split-post-commit-Hook---SVN-GIT-HG/master/docs/spch.1)  

## Формат файла `deploy.yaml`

Файл должен находиться в корне `split` репозитория и содержать три значимых секции для команд:

- Секция `pre` - команды выполняемые перед тестами или сборкой, код возврата команд не проверяется.
- Секция `build` - команды сборки и теста, проверяется код возврата команд, в случае успеха он должен быть `0`.  
- Секция `post` - команды выполняемые после секции `build`, код возврата команд не проверяется.

Пример файла [`deploy.yaml`](https://github.com/ClnViewer/LinkedHashMap/blob/master/deploy.yaml):

       pre:
         - cd build/
       build:
         - gcc -std=gnu99 -lm -I../src -o Test01 ../src/abc.c ../src/cba.c
         - ./Test01
       post:
         - rm -f ./Test01
         - echo "Ok"

Порядок выполнения в случае если указаны `-d (--deploy)` и `-y (--yaml)` ключи, следующий: сперва выполняется анализ и выполнение `yaml` конфигурационного файла, если проверка и тесты прошли успешно, выполняется скрипт заданный ключом `-d` и проверяется его возвращаемое значение. 

### Доступные предопределенные переменные

При описании команд можно использовать предопределенные переменные:

- `$_ROOT` - корневая директория репозитория: [`полный путь`].
- `$_VCS_REVISION` - текущая ревизия репозитория: [`число`].
- `$_VCS_TYPE` - тип `VCS`: [ `svn` | `git` | `hg` ] .
- `$_RC` - код возврата последний команды из секции `build`.

### Пример фрагмента лога выполнения команд файла `deploy.yaml`

     Setup -> Root directory: /repo/split
     Setup -> VCS revision: 160
     Setup -> VCS type: svn

     Pre-build cmd: cd build/
     Examine cmd: /usr/bin/gcc -std=gnu99 -Wall -pedantic -I../src 
      -Wfatal-errors -pedantic-errors 
      ../src/hashmap-link-hash.c ../test/main.c -o ./testDeployBuild
     Examine cmd: ./testDeployBuild
        17) Add: [217374]
        19) Add: [925358]
        ...
        37)      [925358]
     All Tests return 0
     Post-build cmd: rm -f ./testDeployBuild
     Post-build cmd: echo "Ok"
       "Ok"

## Сборка и Инсталляция

Сборка производиться системой `Cmake` и не отличается по шагам от типовых случаев. Порядок действий:

1. Получите дистрибутив исходных кодов `spch` любым удобным для вас способом: клонируйте данный репозиторий, или загрузите его в формате zip архива.  
2. Распакуйте полученный архив и перейдите в директорию `build`.
3. Для `Windows` платформы запустите `cmake-build-mingw.cmd`, для `POSIX` систем запустите `cmake-build-gcc.sh`.  
4. Если все прошло удачно, и сборка прошла без ошибок, исполняемый файл `spch` или `spch.exe` вы найдете в директории `build\bin`.  
5. Для `POSIX` систем в пакетный файл сборки включена команда `install`, если вы не переопределяли путь, исполняемый файл будет скопирован в директорию `/usr/local/bin`.  

Так же вы можете взять собранные исполняемые и архивные файлы для `32/64` `Windows` платформ: [w32 версия](https://github.com/ClnViewer/Split-post-commit-Hook---SVN-GIT-HG/raw/master/docs/distro/spch32.zip) | [w64 версия](https://github.com/ClnViewer/Split-post-commit-Hook---SVN-GIT-HG/raw/master/docs/distro/spch64.zip) | [релиз](https://github.com/ClnViewer/Split-post-commit-Hook---SVN-GIT-HG/releases)      

> Для `Windows` платформы используйте компилятор `MinGW`, для `POSIX` платформ должен используется компилятор `gcc` или `clang`. Возможно собрать `spch` в `MSVC/MSVS`, для этого используйте пакетный файл `cmake-build-msvc.cmd`.
> По умолчанию для `MSVC/MSVS` сборка указана для `Visual Studio 15 2017`. 

Запускаться `spch` должен из скрипта `VCS` `post-commit`, точное название  и местоположение зависимости от типа `VCS` и операционной системы. Файлы хуков `VCS` могут иметь отличные имена и располагаться согласно документации.  

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
       -r "${2}" \
       -t svn \
       -k \
       -y \
       -o /home/svn/repo \
       -m /home/user/MyLibRootRepo \
       -s /home/user/__SplitRepo/MyLibSplitRepo_auto \
       -l buildDirectory/split-repo.list \
       -j /home/svn/spch_MyLibSplit.log \
       -x mylib=libexport


    # если используем файл 'yaml', не забываем добавить глобальное
    # игнорирование файла deploy.yaml:

    svn propset svn:global-ignores deploy.yaml
    # или на уровне выбранного репозитория
    svn propset svn:ignore deploy.yaml

> Для `Windows` платформ ключ __`-y`__ не поддерживается, в связи с отсутствием полноценного `bash/ksh` интерпретатора в базовой поставке. 


## Пример файла журнала

     [30060/30059][startedlog:102] -> spch v.0.0.9.62() 18.08
     [30060/30059][startedlog:107] -> started at 08/30/18-06:16PM
     [30060/30059][main:78] -> stage #0 non-loop mode: Parent 30060 successful exit
     [30061/1][main:97] -> stage #0 check files mode: all - create, modify time and size
     [30061/1][pch_stage2:154] -> stage #2 update: /repo/master/spch-link-hash.h -> /repo/split/hashmap-link-hash.h
     [30061/1][pch_stage2:154] -> stage #2 update: /repo/master/spch-link-hash.c -> /repo/split/hashmap-link-hash.c
     [30061/1][main:112] -> stage #2 changed repo objects: 2 -> /repo/split
     [30061/1][pch_stage3:217] -> deploy script [buildDeploy.sh] start: 

        17) Add: [217374]
        19) Add: [925358]
        ...
        54) Dump
        1)      [139920]
        2)      [440839]
        3)      [925358]

     [30061/1][pch_stage3:217] -> deploy script [buildDeploy.sh] - OK
     [30061/1][main:136] -> stage #3 changed commit success: /repo/split
     [30061/1][endedlog:119] -> ended at 08/30/18-06:16PM

## License

_MIT_

