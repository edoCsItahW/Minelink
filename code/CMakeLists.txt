list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules")

include(Functions)
include(CommonSettings)

cmake_minimum_required(VERSION 4.5)
project(MineLink VERSION 1.0)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_SCAN_FOR_MODULES ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


st_l_options(ENABLE_CCACHE DC0 ON)
st_l_options(ENABLE_PAPALLEL DC1 ON)
st_l_options(ENABLE_ARTIFACTS DC2 ON)
st_l_options(ENABLE_DEP_CACHE DC3 ON)
st_l_options(LIMIT_LINKED_MEMORY DC4 OFF)
st_l_options(ENABLE_AUTO_LINKER DC5 ON)
# GLOBAL_ARTIFACTS_DIR
# CMAKE_BUILD_PARALLEL_LEVEL


set(BUILD_CHILDREN CLIENT)  # , BOT, CLUSTER, NET, PROTO)

resolve_feature_tree(BUILD)

if (ENABLE_ARTIFACTS)
    if (DEFINED GLOBAL_ARTIFACTS_DIR AND NOT GLOBAL_ARTIFACTS_DIR STREQUAL "")
        get_filename_component(GLOBAL_ARTIFACTS_DIR "${GLOBAL_ARTIFACTS_DIR}" ABSOLUTE)
    else ()
        set(GLOBAL_ARTIFACTS_DIR "${CMAKE_SOURCE_DIR}/artifacts")
    endif ()
endif ()
if (ENABLE_DEP_CACHE AND (NOT DEFINED GLOBAL_DEP_CACHE_DIR OR GLOBAL_DEP_CACHE_DIR STREQUAL ""))
    set(GLOBAL_DEP_CACHE_DIR "${CMAKE_SOURCE_DIR}/third-party")
endif ()
if (NOT WIN32 AND NOT DEFINED CMAKE_POSITION_INDEPENDENT_CODE)
    set(CMAKE_POSITION_INDEPENDENT_CODE ON CACHE BOOL "" FORCE)
endif ()

macro(g_auto_library TARGET SOURCES INCLUDE_DIR)
    enable_library_cache("${ENABLE_ARTIFACTS}" "${GLOBAL_ARTIFACTS_DIR}" "${TARGET}" "${SOURCES}" "${INCLUDE_DIR}" ${ARGN})
endmacro()

macro(g_auto_executable TARGET SOURCES INCLUDE_DIR)
    enable_executable_cache("${ENABLE_ARTIFACTS}" "${GLOBAL_ARTIFACTS_DIR}" "${TARGET}" "${SOURCES}" "${INCLUDE_DIR}" ${ARGN})
endmacro()


if (ANY_BUILD)
    if (ENABLE_CCACHE)
        enable_compiler_cache()
    endif ()

    if (ENABLE_PAPALLEL AND NOT DEFINED CMAKE_BUILD_PARALLEL_LEVEL)
        set(CMAKE_BUILD_PARALLEL_LEVEL 4)
    endif ()

    if (LIMIT_LINKED_MEMORY)
        limit_linked_memory()
    endif ()

    if (ENABLE_AUTO_LINKER)
        auto_find_linker()
    endif ()

    if (ANY_TESTS)
        set(GOOGLETEST_INSTALL_PREFIX "${GLOBAL_DEP_CACHE_DIR}")

        find_package(GoogleTest REQUIRED)
    endif ()

    add_subdirectory(modules)

endif ()
