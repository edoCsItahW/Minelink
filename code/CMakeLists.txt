cmake_minimum_required(VERSION 4.5)
project(MineLink VERSION 1.0)

# 特性
set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_SCAN_FOR_MODULES ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# cmake库
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules")

include(Functions)

# 选项
g_l_options(ENABLE_DEP_CACHE DC1 ON)

if (ENABLE_DEP_CACHE AND (NOT DEFINED GLOBAL_DEP_CACHE_DIR OR GLOBAL_DEP_CACHE_DIR STREQUAL ""))
    set(GLOBAL_DEP_CACHE_DIR "${CMAKE_SOURCE_DIR}/third-party")
endif ()

# 特性树
set(BUILD_CHILDREN CORE INTERFACES TESTS)

g_resolve_feature_tree(BUILD)

# 主体
if (ANY_BUILD)

    add_subdirectory(MineLink)

    if (ANY_TESTS)

        set(GOOGLETEST_INSTALL_PREFIX "${GLOBAL_DEP_CACHE_DIR}")
        find_package(GoogleTest REQUIRED)

    endif ()

endif ()