include(${CMAKE_CURRENT_LIST_DIR}/modules/Locale.cmake)


# -------------------- 选项 --------------------
g_localize(DLibdeflate0 _AUTO_INSTALL_DOC)
option(LIBDEFLATE_AUTO_INSTALL ${_AUTO_INSTALL_DOC} ON)

g_localize(DLibdeflate1 _GIT_TAG_DOC)
set(LIBDEFLATE_GIT_TAG "v1.26" CACHE STRING ${_GIT_TAG_DOC})

g_localize(DLibdeflate2 _INSTALL_PREFIX_DOC)
set(LIBDEFLATE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/third-party" CACHE PATH ${_INSTALL_PREFIX_DOC})

g_localize(DLibdeflate3 _BUILD_SHARED_DOC)
option(LIBDEFLATE_BUILD_SHARED ${_BUILD_SHARED_DOC} OFF)

g_localize(DLibdeflate4 _DIR_DOC)


# -------------------- 查找现有 libdeflate 源码目录 --------------------
# libdeflate 通过 add_subdirectory 构建。安装后才会生成 CMake config 包，
# 直接 fetch 的源码树没有 config 文件，因此不能用 find_package 的 CONFIG 模式
# 查找。这里检测 libdeflate-src 目录是否已存在，避免重复下载。
if (NOT Libdeflate_DIR AND NOT DEFINED ENV{LIBDEFLATE_DIR})
    set(_libdeflate_search_paths
            "${CMAKE_SOURCE_DIR}/third-party"
            "${LIBDEFLATE_INSTALL_PREFIX}"
            "/usr/local"
            "/usr"
    )

    foreach (_path ${_libdeflate_search_paths})
        if (EXISTS "${_path}/libdeflate-src/CMakeLists.txt")
            set(Libdeflate_DIR "${_path}/libdeflate-src" CACHE PATH ${_DIR_DOC})
            break()
        endif ()
    endforeach ()

endif ()

# 检查 LIBDEFLATE_DIR 环境变量
if (NOT Libdeflate_DIR AND DEFINED ENV{LIBDEFLATE_DIR})
    if (EXISTS "$ENV{LIBDEFLATE_DIR}/CMakeLists.txt")
        set(Libdeflate_DIR "$ENV{LIBDEFLATE_DIR}" CACHE PATH ${_DIR_DOC})
    endif ()
endif ()


# -------------------- 构建或复用 libdeflate --------------------
set(_libdeflate_source_dir "")
set(_libdeflate_binary_dir "${CMAKE_BINARY_DIR}/dependencies/libdeflate-build")

# libdeflate 是 C 库，而消费者可能只启用了 CXX。
if (NOT CMAKE_C_COMPILER_LOADED)
    enable_language(C)
endif ()

if (NOT TARGET libdeflate_static AND NOT TARGET libdeflate_shared)

    # 作为子项目只构建库本体：关闭命令行程序、测试和安装规则。
    # STATIC/SHARED 由 LIBDEFLATE_BUILD_SHARED 统一决定，避免同时编译两份。
    if (LIBDEFLATE_BUILD_SHARED)
        set(LIBDEFLATE_BUILD_STATIC_LIB OFF CACHE BOOL "Build the libdeflate static library" FORCE)
        set(LIBDEFLATE_BUILD_SHARED_LIB ON CACHE BOOL "Build the libdeflate shared library" FORCE)
    else ()
        set(LIBDEFLATE_BUILD_STATIC_LIB ON CACHE BOOL "Build the libdeflate static library" FORCE)
        set(LIBDEFLATE_BUILD_SHARED_LIB OFF CACHE BOOL "Build the libdeflate shared library" FORCE)
    endif ()
    set(LIBDEFLATE_BUILD_GZIP OFF CACHE BOOL "Build the libdeflate-gzip program" FORCE)
    set(LIBDEFLATE_BUILD_TESTS OFF CACHE BOOL "Build the libdeflate test programs" FORCE)
    set(LIBDEFLATE_INSTALL OFF CACHE BOOL "Install libdeflate targets" FORCE)

    if (Libdeflate_DIR AND EXISTS "${Libdeflate_DIR}/CMakeLists.txt")
        # 源码已存在——跳过下载（检测以避免重复下载）
        g_l_info_fmt(ILibdeflate0 ${Libdeflate_DIR})
        set(_libdeflate_source_dir "${Libdeflate_DIR}")

        add_subdirectory("${_libdeflate_source_dir}" "${_libdeflate_binary_dir}" EXCLUDE_FROM_ALL)

    elseif (LIBDEFLATE_AUTO_INSTALL)
        # 下载并构建
        g_l_info_fmt(ILibdeflate1 ${LIBDEFLATE_GIT_TAG})

        include(FetchContent)
        FetchContent_Declare(libdeflate
                GIT_REPOSITORY https://github.com/ebiggers/libdeflate.git
                GIT_TAG ${LIBDEFLATE_GIT_TAG}
                GIT_SHALLOW TRUE
                # libdeflate 没有子模块；显式置空可避免 CMake 触发
                # git submodule update（Windows 下会依赖 sh.exe）。
                GIT_SUBMODULES ""
                SOURCE_DIR "${LIBDEFLATE_INSTALL_PREFIX}/libdeflate-src"
                BINARY_DIR "${_libdeflate_binary_dir}"
                EXCLUDE_FROM_ALL
        )

        FetchContent_MakeAvailable(libdeflate)
        set(_libdeflate_source_dir "${libdeflate_SOURCE_DIR}")
        set(Libdeflate_DIR "${libdeflate_SOURCE_DIR}" CACHE PATH ${_DIR_DOC} FORCE)

    else ()
        # 未找到且未启用自动安装
        g_l_info(ILibdeflate3)

    endif ()

    if (TARGET libdeflate_static OR TARGET libdeflate_shared)
        g_l_info(ILibdeflate2)
    elseif (_libdeflate_source_dir)
        g_l_warning(WLibdeflate0)
    endif ()

endif ()


# -------------------- 设置结果变量 --------------------
set(_libdeflate_target "")

if (TARGET libdeflate_static)
    set(_libdeflate_target libdeflate_static)
elseif (TARGET libdeflate_shared)
    set(_libdeflate_target libdeflate_shared)
endif ()

if (_libdeflate_target)
    # 上游已提供 libdeflate::libdeflate_static / libdeflate::libdeflate_shared，
    # 这里兜底并额外提供统一的 Libdeflate::Libdeflate，便于消费者使用。
    if (NOT TARGET libdeflate::${_libdeflate_target})
        add_library(libdeflate::${_libdeflate_target} ALIAS ${_libdeflate_target})
    endif ()
    if (NOT TARGET Libdeflate::Libdeflate)
        add_library(Libdeflate::Libdeflate ALIAS ${_libdeflate_target})
    endif ()

    set(Libdeflate_LIBRARIES Libdeflate::Libdeflate)

    # 源码树中的 libdeflate.h 位于根目录；直接给出源码路径，
    # 避免把 INTERFACE_INCLUDE_DIRECTORIES 里的 BUILD_INTERFACE 生成表达式
    # 泄漏到结果变量中。
    set(Libdeflate_INCLUDE_DIRS "${_libdeflate_source_dir}")
    if (NOT Libdeflate_INCLUDE_DIRS)
        get_target_property(Libdeflate_INCLUDE_DIRS ${_libdeflate_target} INTERFACE_INCLUDE_DIRECTORIES)
    endif ()

    # 版本号优先从 libdeflate.h 读取，读不到再回退到 Git 标签。
    if (_libdeflate_source_dir AND EXISTS "${_libdeflate_source_dir}/libdeflate.h")
        file(STRINGS "${_libdeflate_source_dir}/libdeflate.h" _libdeflate_version_line
                REGEX "#define[ \t]+LIBDEFLATE_VERSION_STRING[ \t]+\"([0-9]+\\.[0-9]+(\\.[0-9]+)?)\"")
        if (_libdeflate_version_line MATCHES "\"([0-9]+\\.[0-9]+(\\.[0-9]+)?)\"")
            set(Libdeflate_VERSION "${CMAKE_MATCH_1}")
        endif ()
    endif ()

    if (NOT Libdeflate_VERSION)
        string(REGEX REPLACE "^v" "" Libdeflate_VERSION "${LIBDEFLATE_GIT_TAG}")
    endif ()

endif ()


# -------------------- 标准 FindPackage 处理 --------------------
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Libdeflate
        REQUIRED_VARS Libdeflate_LIBRARIES Libdeflate_INCLUDE_DIRS
        VERSION_VAR Libdeflate_VERSION
        HANDLE_COMPONENTS
)


if (Libdeflate_FOUND)
    g_l_info_fmt(ILibdeflate4 ${Libdeflate_INCLUDE_DIRS})
    g_l_info_fmt(ILibdeflate5 ${Libdeflate_LIBRARIES})
    if (Libdeflate_VERSION)
        g_l_info_fmt(ILibdeflate6 ${Libdeflate_VERSION})
    endif ()
endif ()
