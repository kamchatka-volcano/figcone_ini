cmake_minimum_required(VERSION 3.18)

project(figcone_ini VERSION 0.10.0 DESCRIPTION "inifile-cpp parser adapter for figcone library")
include(GNUInstallDirs)

if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    set(FIGCONE_INI_SUBPROJECT OFF)
else()
    set(FIGCONE_INI_SUBPROJECT ON)
endif()

include(external/ini)
include(external/sfun)

find_package(figcone_tree 0.10.0 QUIET)
if (NOT figcone_tree_FOUND)
    message("Configuration info: figcone_tree library wasn't found on your system, downloading it from the GitHub repository")
    include (external/figcone_tree)
endif()

add_library(figcone_ini INTERFACE)
add_library(figcone::figcone_ini ALIAS figcone_ini)

target_compile_features(figcone_ini INTERFACE cxx_std_17)
target_include_directories(
        figcone_ini
        INTERFACE
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
target_include_directories(
    figcone_ini
    INTERFACE
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/figcone_ini/detail/external>
)
target_include_directories(
    figcone_ini
    INTERFACE
    $<BUILD_INTERFACE:${sfun_SOURCE_DIR}/include/>
)
target_include_directories(
    figcone_ini
    INTERFACE
    $<BUILD_INTERFACE:${ini_SOURCE_DIR}/include/>
)
target_link_libraries(figcone_ini INTERFACE figcone::figcone_tree)

option(ENABLE_TESTS "Enable tests" OFF)
if (ENABLE_TESTS AND NOT FIGCONE_INI_SUBPROJECT)
    enable_testing()
    add_subdirectory(tests)
endif()

option(INSTALL_FIGCONE_INI "Install figcone_ini library unconditionally" OFF)
if (NOT FIGCONE_INI_SUBPROJECT OR INSTALL_FIGCONE_INI)
    install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/figcone_ini       DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
    install(DIRECTORY ${sfun_SOURCE_DIR}/include/sfun                 DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/figcone_ini/detail/external)
    install(FILES     ${ini_SOURCE_DIR}/include/inicpp.h              DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/figcone_ini/detail/external)

    set(FIGCONE_INI_PACK_PATH ${CMAKE_INSTALL_LIBDIR}/cmake/figcone_ini)
    install(TARGETS figcone_ini
            EXPORT figcone_ini-targets
            PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/figcone_ini)
    install(EXPORT figcone_ini-targets
            FILE figcone_iniTargets.cmake
            NAMESPACE figcone::
            DESTINATION ${FIGCONE_INI_PACK_PATH})

    include(CMakePackageConfigHelpers)
    write_basic_package_version_file(
            ${CMAKE_CURRENT_BINARY_DIR}/figcone_iniConfigVersion.cmake
            COMPATIBILITY SameMajorVersion
            ARCH_INDEPENDENT
    )

    configure_package_config_file(${CMAKE_CURRENT_LIST_DIR}/cmake/figcone_iniConfig.cmake.in
            ${CMAKE_CURRENT_BINARY_DIR}/figcone_iniConfig.cmake
            INSTALL_DESTINATION ${FIGCONE_INI_PACK_PATH}
    )

    install(FILES
            ${CMAKE_CURRENT_BINARY_DIR}/figcone_iniConfig.cmake
            ${CMAKE_CURRENT_BINARY_DIR}/figcone_iniConfigVersion.cmake
            DESTINATION ${FIGCONE_INI_PACK_PATH}
    )
endif()
