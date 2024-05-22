cmake_minimum_required(VERSION 3.18)

project(figcone_ini VERSION 1.1.0 DESCRIPTION "inifile-cpp parser adapter for figcone library")
include(external/seal_lake)

SealLake_Bundle(
        NAME figcone_inicpp
        GIT_REPOSITORY https://github.com/kamchatka-volcano/inifile-cpp.git
        GIT_TAG v0.1.0
        SKIP_LOAD
        TEXT_REPLACEMENTS
            "namespace ini" "namespace figcone::ini"
)

SealLake_Bundle(
        NAME figcone_ini_sfun
        GIT_REPOSITORY https://github.com/kamchatka-volcano/sfun.git
        GIT_TAG        v5.1.0
        TEXT_REPLACEMENTS
            "namespace sfun" "namespace figcone::ini::sfun"
)

SealLake_IsInstallEnabled(INSTALL_FIGCONE_TREE)
SealLake_Import(
        figcone_tree 2.1.0
        GIT_REPOSITORY https://github.com/kamchatka-volcano/figcone_tree.git
        GIT_TAG      v2.1.0
)

SealLake_ObjectLibrary(
        NAMESPACE figcone
        COMPILE_FEATURES cxx_std_17
        SOURCES
            src/parser.cpp
            src/paramparser.cpp
            src/stream.cpp
            src/utils.cpp
        INCLUDES
            ${SEAL_LAKE_SOURCE_figcone_inicpp}/include
        INTERFACE_LIBRARIES
            figcone::figcone_tree
        LIBRARIES
            figcone_ini_sfun::figcone_ini_sfun
        DEPENDENCIES
            figcone_tree 2.1.0
)

SealLake_OptionalSubProjects(tests)
