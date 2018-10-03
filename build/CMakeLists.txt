cmake_minimum_required(VERSION 2.6)
project(spch)
enable_language(C)

if(OLD_CMAKE_MACRO)
   set( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/mcmake" CACHE STRING "Modules for CMake" FORCE )
   include(mcmake/CheckCCompilerFlag.cmake)
else()
   include(CheckCCompilerFlag)
endif()

set( CMAKE_COLOR_MAKEFILE ON )
set( CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "Configs" FORCE )
set( CMAKE_SUPPRESS_REGENERATION true )
set( CMAKE_SKIP_INSTALL_ALL_DEPENDENCY true )
set( CMAKE_SKIP_PACKAGE_ALL_DEPENDENCY true )
set_property( GLOBAL PROPERTY USE_FOLDERS ON )
set( PREDEFINED_TARGETS_FOLDER "CmakeDepends" )

find_program (
    YAMLSHELL1 NAMES bash PATHS /bin /usr/bin /usr/local/bin
)
find_program (
    YAMLSHELL2 NAMES ksh PATHS /bin /usr/bin /usr/local/bin
)

if(${YAMLSHELL1} STREQUAL "YAMLSHELL1-NOTFOUND")
    set(YAMLSHELL1 "")
endif()

if(${YAMLSHELL2} STREQUAL "YAMLSHELL2-NOTFOUND")
    set(YAMLSHELL2 "")
endif()

configure_file(
    spch-shell.h.in "${CMAKE_CURRENT_SOURCE_DIR}/../src/spch-shell.h"
)

if(MSVC)
   include_directories(
      ${CMAKE_CURRENT_SOURCE_DIR}/../src
      ${CMAKE_CURRENT_SOURCE_DIR}/../src/libs/include
      ${CMAKE_CURRENT_SOURCE_DIR}/../src/libs/include/win
      ${CMAKE_CURRENT_SOURCE_DIR}/../src/libs/include/msvc
   )
   set(app_srcs
      ${app_srcs}
      ${CMAKE_CURRENT_SOURCE_DIR}/../src/libs/strptime_w32.c
   )
elseif (WIN32)
   include_directories(
      ${CMAKE_CURRENT_SOURCE_DIR}/../src
      ${CMAKE_CURRENT_SOURCE_DIR}/../src/libs/include
      ${CMAKE_CURRENT_SOURCE_DIR}/../src/libs/include/win
   )
else()
   include_directories(
      ${CMAKE_CURRENT_SOURCE_DIR}/../src
      ${CMAKE_CURRENT_SOURCE_DIR}/../src/libs/include
   )
endif()

set(app_srcs
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/spch.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/spch-exec.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/spch-file.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/spch-link-hash.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/spch-log.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/spch-opt.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/spch-stage.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/spch-vcs.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/spch-vcs-log.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/libs/string_s.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/libs/xmlp.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/libs/kzip.c
)
set(app_hdrs
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/spch.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/spch-opt.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/spch-opt-data.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/spch-vcs.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/spch-link-hash.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/spch-stage3-yaml.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/spch-stage2-field.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/version.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/libs/include/buildt.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/libs/include/boolt.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/libs/include/string_s.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/libs/include/xmlp.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/libs/include/kzip.h
)

add_executable( ${PROJECT_NAME} ${app_srcs} ${app_hdrs} )
set( EXECUTABLE_OUTPUT_PATH  ${CMAKE_CURRENT_SOURCE_DIR}/bin )
set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin )

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release")
endif()

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" OR
    "${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")

     CHECK_C_COMPILER_FLAG("-std=c17" COMPILER_SUPPORTS_C17)
     if(COMPILER_SUPPORTS_C17)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c17")
     else()
        CHECK_C_COMPILER_FLAG("-std=c11" COMPILER_SUPPORTS_C11)
        if(COMPILER_SUPPORTS_C11)
           set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c11")
        else()
           CHECK_C_COMPILER_FLAG("-std=gnu99" COMPILER_SUPPORTS_C99)
           if(COMPILER_SUPPORTS_C99)
              set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu99")
           endif()
        endif()
     endif()

     CHECK_C_COMPILER_FLAG("-Wstrict-prototypes" COMPILER_SUPPORTS_C_WSP)
     if(COMPILER_SUPPORTS_C_WSP)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wstrict-prototypes")
     endif()

     CHECK_C_COMPILER_FLAG("-Wall" COMPILER_SUPPORTS_C_WALL)
     if(COMPILER_SUPPORTS_C_WALL)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")
     endif()

     CHECK_C_COMPILER_FLAG("-Wextra" COMPILER_SUPPORTS_C_WEXTRA)
     if(COMPILER_SUPPORTS_C_WEXTRA)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wextra")
     endif()

     CHECK_C_COMPILER_FLAG("-pedantic" COMPILER_SUPPORTS_C_PEDANTIC)
     if(COMPILER_SUPPORTS_C_PEDANTIC)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pedantic")
     endif()

     CHECK_C_COMPILER_FLAG("-Wno-unused-command-line-argument" COMPILER_SUPPORTS_C_NUCLA)
     if(COMPILER_SUPPORTS_C_NUCLA)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-unused-command-line-argument")
     endif()

     CHECK_C_COMPILER_FLAG("-ffunction-sections -fdata-sections -Wl,--gc-sections " COMPILER_SUPPORTS_C_FUNSEC)
     if(COMPILER_SUPPORTS_C_FUNSEC)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -ffunction-sections -fdata-sections -Wl,--gc-sections")
     endif()

     if ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug" )
         CHECK_C_COMPILER_FLAG("-Wl,--print-gc-sections" COMPILER_SUPPORTS_C_LPGCSEC)
         if(COMPILER_SUPPORTS_C_LPGCSEC)
            set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wl,--print-gc-sections")
         endif()

         message( STATUS "Build CFLAGS: " ${CMAKE_C_FLAGS} )

     else()
         CHECK_C_COMPILER_FLAG("-s" COMPILER_SUPPORTS_C_STRIP)
         if(COMPILER_SUPPORTS_C_STRIP)
            set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -s")
         endif()
     endif()

endif()

if(MSVC)
  if(ENABLE_SOLUTION_FOLDERS)
     set_target_properties( ${PROJECT_NAME} PROPERTIES FOLDER "${CMAKE_CURRENT_SOURCE_DIR}/build/docs" )
  endif()
  set_property( DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME} )
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /D__BUILD_SPCH=1" )
else()
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D__BUILD_SPCH=1" )
endif()

if( ${CMAKE_SYSTEM_NAME} MATCHES "Linux" )
    if(NOT CMAKE_INSTALL_PREFIX)
        set( CMAKE_INSTALL_PREFIX "/usr" CACHE PATH "Cmake prefix" FORCE )
    endif()
    if(NOT CMAKE_INSTALL_MANDIR)
        set( CMAKE_INSTALL_MANDIR "${CMAKE_INSTALL_PREFIX}/man" CACHE PATH "Cmake man dir" FORCE )
    endif()
    if(NOT CMAKE_INSTALL_BINDIR)
        set( CMAKE_INSTALL_BINDIR "${CMAKE_INSTALL_PREFIX}/bin" CACHE PATH "Cmake bin dir" FORCE )
    endif()

    message( STATUS "Install path to spch: " ${CMAKE_INSTALL_BINDIR} )

    install(
        FILES "${PROJECT_SOURCE_DIR}/../docs/${PROJECT_NAME}.1" DESTINATION "${CMAKE_INSTALL_MANDIR}/man1"
    )
    install(
        FILES "${PROJECT_SOURCE_DIR}/bin/${PROJECT_NAME}" DESTINATION "${CMAKE_INSTALL_BINDIR}"
    )
endif()

