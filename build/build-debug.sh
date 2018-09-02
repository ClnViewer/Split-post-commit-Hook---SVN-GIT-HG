#!/bin/bash

    SYS="`uname`"

    if [[ "${SYS}" == "" ]] ;
    then
        SYS="unix"
    fi

    cd ..
    sudo -u admin svn update
    cd build

    mkdir -p ${SYS}
    rm -Rf ${SYS}/*
    rm -f bin/spch
    cd ${SYS}

    cmake -G"Unix Makefiles" .. -DCMAKE_BUILD_TYPE=Debug
    cmake --build . --config "Debug"

    cd ../
    rm -Rf ${SYS}/*
    rm -f bin/spch

