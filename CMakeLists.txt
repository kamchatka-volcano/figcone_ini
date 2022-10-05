cmake_minimum_required(VERSION 3.18)

project(figcone_ini VERSION 0.11.0 DESCRIPTION "inifile-cpp parser adapter for figcone library")
include(GNUInstallDirs)
include(external/seal_lake)

SealLake_Download(
        GIT_REPOSITORY https://github.com/Rookfighter/inifile-cpp.git
        GIT_TAG master
        DESTINATION include/figcone_ini/detail/external
        FILES include/inicpp.h
        TEXT_REPLACEMENTS
            "namespace ini" "namespace figcone::ini"
            "INICPP_" "FIGCONE_INICPP_"
)

SealLake_Download(
        GIT_REPOSITORY https://github.com/kamchatka-volcano/sfun.git
        GIT_TAG        cmake_maintenance
        DESTINATION include/figcone_ini/detail/external
        DIRECTORIES include/sfun
        TEXT_REPLACEMENTS
            "namespace sfun" "namespace figcone::ini::sfun"
            "SFUN_" "FIGCONE_INI_SFUN_"
)

set(INSTALL_FIGCONE_TREE ON)
SealLake_Import(
        figcone_tree 0.11.0
        GIT_REPOSITORY https://github.com/kamchatka-volcano/figcone_tree.git
        GIT_TAG        cmake_maintenance
)

SealLake_HeaderOnlyLibrary(
        NAMESPACE figcone
        COMPILE_FEATURES cxx_std_17
        LIBRARIES figcone::figcone_tree
        DEPENDENCIES
            figcone_tree 0.11.0
)

SealLake_OptionalBuildSteps(tests)
