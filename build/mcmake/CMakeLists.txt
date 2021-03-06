cmake_minimum_required(VERSION 2.6)
project(spch)
enable_language(C)

set( CMAKE_COLOR_MAKEFILE ON )
set( CMAKE_CONFIGURATION_TYPES "Release" CACHE STRING "Configs" FORCE )
set( CMAKE_SUPPRESS_REGENERATION true )
set( CMAKE_SKIP_INSTALL_ALL_DEPENDENCY true )
set( CMAKE_SKIP_PACKAGE_ALL_DEPENDENCY true )
set_property( GLOBAL PROPERTY USE_FOLDERS ON )
set( PREDEFINED_TARGETS_FOLDER "CmakeDepends" )

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/../../src
    ${CMAKE_CURRENT_SOURCE_DIR}/../../src/libs/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../src/libs/include/win
)

set(app_srcs
    ${CMAKE_CURRENT_SOURCE_DIR}/../../src/spch.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../src/spch-exec.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../src/spch-file.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../src/spch-link-hash.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../src/spch-log.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../src/spch-opt.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../src/spch-stage.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../src/spch-vcs.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../src/spch-vcs-log.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../src/libs/string_s.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../src/libs/xmlp.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../src/libs/kzip.c
)
set(app_hdrs
    ${CMAKE_CURRENT_SOURCE_DIR}/../../src/spch.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../../src/spch-opt.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../../src/spch-opt-data.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../../src/spch-vcs.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../../src/spch-link-hash.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../../src/spch-stage3-yaml.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../../src/spch-stage2-field.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../../src/version.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../../src/libs/include/buildt.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../../src/libs/include/boolt.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../../src/libs/include/string_s.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../../src/libs/include/xmlp.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../../src/libs/include/kzip.h
)

add_executable( ${PROJECT_NAME} ${app_srcs} ${app_hdrs} )

set(CMAKE_BUILD_TYPE "Release")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c11")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wstrict-prototypes")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wextra")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pedantic")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -ffunction-sections -fdata-sections -Wl,--gc-sections")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wl,--print-gc-sections")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -s")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D__BUILD_SPCH=1" )

