cmake_minimum_required(VERSION 3.18)

project(figcone_ini_deps)
include(seal_lake.cmake)

SealLake_v040_Bundle(
        NAME figcone_ini_eel
        URL https://github.com/kamchatka-volcano/eel/releases/download/v0.3.0/eel-v0.3.0.zip
        SKIP_LOAD
        DESTINATION deps/figcone_ini_eel
        TEXT_REPLACEMENTS
        "namespace eel" "namespace figcone::ini::eel"
        "EEL_" "FIGCONE_INI_EEL_"
)

SealLake_v040_Bundle(
        NAME figcone_tree
        URL https://github.com/kamchatka-volcano/figcone_tree/releases/download/v2.2.0/figcone_tree-v2.2.0.zip
        SKIP_LOAD
        DESTINATION deps/figcone_tree
)

SealLake_v040_Bundle(
        NAME figcone_inicpp
        GIT_REPOSITORY https://github.com/kamchatka-volcano/inifile-cpp.git
        GIT_TAG v0.1.0
        SKIP_LOAD
        DIRECTORIES include
        DESTINATION deps/figcone_inicpp/
        TEXT_REPLACEMENTS
        "namespace ini" "namespace figcone::ini"
)
