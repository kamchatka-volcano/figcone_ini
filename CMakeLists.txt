cmake_minimum_required(VERSION 3.18)

project(figcone_ini VERSION 0.11.0 DESCRIPTION "inifile-cpp parser adapter for figcone library")
include(GNUInstallDirs)
include(external/seal_lake)

SealLake_Download(
        GIT_REPOSITORY https://github.com/Rookfighter/inifile-cpp.git
        GIT_TAG master
        DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/external
        FILES include/inicpp.h
        TEXT_REPLACEMENTS
            "namespace ini" "namespace figcone::ini"
)

SealLake_Import(
        sfun 2.4.0
        GIT_REPOSITORY https://github.com/kamchatka-volcano/sfun.git
        GIT_TAG        cmake_maintenance
        TEXT_REPLACEMENTS
            "namespace sfun" "namespace figcone::ini::sfun"
)

set(INSTALL_FIGCONE_TREE ON)
SealLake_Import(
        figcone_tree 0.11.0
        GIT_REPOSITORY https://github.com/kamchatka-volcano/figcone_tree.git
        GIT_TAG        cmake_maintenance
)

SealLake_ObjectLibrary(
        NAMESPACE figcone
        COMPILE_FEATURES cxx_std_17
        SOURCES
            src/parser.cpp
            src/paramparser.cpp
            src/stream.cpp
            src/utils.cpp
        INCLUDES ${CMAKE_CURRENT_BINARY_DIR}/external
        INTERFACE_LIBRARIES figcone::figcone_tree
        LIBRARIES sfun::sfun
        DEPENDENCIES
            figcone_tree 0.11.0
)

SealLake_OptionalBuildSteps(tests)
