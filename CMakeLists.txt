cmake_minimum_required(VERSION 3.18)

project(figcone_ini VERSION 0.11.0 DESCRIPTION "inifile-cpp parser adapter for figcone library")
include(GNUInstallDirs)
include(external/seal_lake)
include(external/ini)

SealLake_FindOrDownload(
        sfun 2.4.0
        GIT_REPOSITORY https://github.com/kamchatka-volcano/sfun.git
        GIT_TAG        cmake_maintenance
)
set(INSTALL_FIGCONE_TREE ON)
SealLake_FindOrDownload(
        figcone_tree 0.11.0
        GIT_REPOSITORY https://github.com/kamchatka-volcano/figcone_tree.git
        GIT_TAG        cmake_maintenance
)

SealLake_HeaderOnlyLibrary(
        NAMESPACE figcone
        COMPILE_FEATURES cxx_std_17
        INCLUDES figcone_ini/detail/external
        BUILD_STAGE_INCLUDES
            ${sfun_SOURCE_DIR}/include/
            ${ini_SOURCE_DIR}/include/
        LIBRARIES figcone::figcone_tree
)

SealLake_OptionalBuildSteps(
        IF_ENABLED_AND_STANDALONE tests
)

SealLake_Install(
        PATH figcone_ini/detail/external
        DIRECTORIES
            ${sfun_SOURCE_DIR}/include/sfun
        FILES
            ${ini_SOURCE_DIR}/include/inicpp.h
)