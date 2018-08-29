#!/bin/bash

    SYS="`uname`"

    if [[ "${SYS}" == "" ]] ;
    then
        SYS="unix"
    fi

    mkdir -p ${SYS}
    rm -Rf ${SYS}/*
    rm -f bin/spch
    cd ${SYS}

    cmake -G"Unix Makefiles" .. -DCMAKE_BUILD_TYPE=Release
    cmake --build . --config "Release"
    cmake --build . --target "install"

    cd ../
