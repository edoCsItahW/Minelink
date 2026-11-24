include(${CMAKE_CURRENT_LIST_DIR}/modules/Locale.cmake)


# minecraft-data 是纯数据仓库，不产生编译目标。本模块只负责获取并校验数据目录，
# 供协议代码生成（如 add_custom_command）使用，因此不会 add_subdirectory。


# -------------------- 选项 --------------------
g_localize(DMinecraftData0 _AUTO_INSTALL_DOC)
option(MINECRAFT_DATA_AUTO_INSTALL "${_AUTO_INSTALL_DOC}" ON)

g_localize(DMinecraftData1 _GIT_TAG_DOC)
set(MINECRAFT_DATA_GIT_TAG "master" CACHE STRING "${_GIT_TAG_DOC}")

g_localize(DMinecraftData2 _INSTALL_PREFIX_DOC)
set(MINECRAFT_DATA_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/third-party" CACHE PATH "${_INSTALL_PREFIX_DOC}")

g_localize(DMinecraftData3 _GAME_VERSION_DOC)
# 注意：CMake 4.5 的命令行 -D 会把未显式声明类型的 1.21.11 按数字解析成 1。
# 命令行设置版本号时请写成 -DMINECRAFT_DATA_GAME_VERSION:STRING=1.21.11。
set(MINECRAFT_DATA_GAME_VERSION "" CACHE STRING "${_GAME_VERSION_DOC}")

g_localize(DMinecraftData4 _DIR_DOC)


# -------------------- 查找现有 minecraft-data 源码目录 --------------------
# 用 data/dataPaths.json 作为数据完整性标记，避免把空目录或错误仓库当成数据源。
if (NOT MinecraftData_DIR AND NOT DEFINED ENV{MINECRAFT_DATA_DIR})
    set(_minecraft_data_search_paths
            "${CMAKE_SOURCE_DIR}/third-party"
            "${MINECRAFT_DATA_INSTALL_PREFIX}"
            "/usr/local"
            "/usr"
    )

    foreach (_path ${_minecraft_data_search_paths})
        if (EXISTS "${_path}/minecraft-data-src/data/dataPaths.json")
            set(MinecraftData_DIR "${_path}/minecraft-data-src" CACHE PATH "${_DIR_DOC}")
            break()
        endif ()
    endforeach ()

endif ()

# 检查 MINECRAFT_DATA_DIR 环境变量（指向仓库根目录）
if (NOT MinecraftData_DIR AND DEFINED ENV{MINECRAFT_DATA_DIR})
    if (EXISTS "$ENV{MINECRAFT_DATA_DIR}/data/dataPaths.json")
        set(MinecraftData_DIR "$ENV{MINECRAFT_DATA_DIR}" CACHE PATH "${_DIR_DOC}")
    endif ()
endif ()


# -------------------- 下载或复用 minecraft-data --------------------
set(_minecraft_data_source_dir "")
set(_minecraft_data_binary_dir "${CMAKE_BINARY_DIR}/dependencies/minecraft-data-build")

if (MinecraftData_DIR AND EXISTS "${MinecraftData_DIR}/data/dataPaths.json")
    # 源码已存在——跳过下载（检测以避免重复下载）
    g_l_info_fmt(IMinecraftData0 ${MinecraftData_DIR})
    set(_minecraft_data_source_dir "${MinecraftData_DIR}")

elseif (MINECRAFT_DATA_AUTO_INSTALL)
    # 下载数据仓库
    g_l_info_fmt(IMinecraftData1 ${MINECRAFT_DATA_GIT_TAG})

    include(FetchContent)
    FetchContent_Declare(minecraft_data
            GIT_REPOSITORY https://github.com/PrismarineJS/minecraft-data.git
            GIT_TAG ${MINECRAFT_DATA_GIT_TAG}
            GIT_SHALLOW TRUE
            # 数据仓库没有子模块；显式置空可避免 CMake 触发
            # git submodule update（Windows 下会依赖 sh.exe）。
            GIT_SUBMODULES ""
            SOURCE_DIR "${MINECRAFT_DATA_INSTALL_PREFIX}/minecraft-data-src"
            BINARY_DIR "${_minecraft_data_binary_dir}"
    )

    # 仓库根目录没有 CMakeLists.txt，MakeAvailable 只会 populate，不会加入构建。
    FetchContent_MakeAvailable(minecraft_data)
    set(_minecraft_data_source_dir "${minecraft_data_SOURCE_DIR}")
    set(MinecraftData_DIR "${minecraft_data_SOURCE_DIR}" CACHE PATH "${_DIR_DOC}" FORCE)

else ()
    # 未找到且未启用自动下载
    g_l_info(IMinecraftData3)

endif ()


# -------------------- 校验数据目录并设置结果变量 --------------------
if (_minecraft_data_source_dir)
    set(_minecraft_data_data_dir "${_minecraft_data_source_dir}/data")

    if (EXISTS "${_minecraft_data_data_dir}/dataPaths.json"
            AND IS_DIRECTORY "${_minecraft_data_data_dir}/pc")
        set(MinecraftData_DATA_DIR "${_minecraft_data_data_dir}")
        set(MinecraftData_DATA_PATHS_FILE "${_minecraft_data_data_dir}/dataPaths.json")
        set(MinecraftData_PC_DIR "${_minecraft_data_data_dir}/pc")

        if (IS_DIRECTORY "${_minecraft_data_data_dir}/bedrock")
            set(MinecraftData_BEDROCK_DIR "${_minecraft_data_data_dir}/bedrock")
        endif ()

        # latest/proto.yml 是上游维护的最新协议定义，可用于生成“最新版”协议代码。
        if (EXISTS "${MinecraftData_PC_DIR}/latest/proto.yml")
            set(MinecraftData_LATEST_PROTO "${MinecraftData_PC_DIR}/latest/proto.yml")
        endif ()

        g_l_info_fmt(IMinecraftData2 ${_minecraft_data_source_dir})
        g_l_info_fmt(IMinecraftData4 ${MinecraftData_DATA_DIR})
        g_l_info_fmt(IMinecraftData5 ${MinecraftData_PC_DIR})

        # 记录数据仓库修订号，便于生成代码时追溯数据来源。
        find_program(_minecraft_data_git NAMES git)
        if (_minecraft_data_git AND EXISTS "${_minecraft_data_source_dir}/.git")
            execute_process(
                    COMMAND "${_minecraft_data_git}" -C "${_minecraft_data_source_dir}" rev-parse --short HEAD
                    OUTPUT_VARIABLE MinecraftData_VERSION
                    OUTPUT_STRIP_TRAILING_WHITESPACE
                    ERROR_QUIET
            )
        endif ()

        if (NOT MinecraftData_VERSION)
            set(MinecraftData_VERSION "${MINECRAFT_DATA_GIT_TAG}")
        endif ()

        g_l_info_fmt(IMinecraftData7 ${MinecraftData_VERSION})

    else ()
        g_l_warning_fmt(WMinecraftData0 "${_minecraft_data_data_dir}")
    endif ()
endif ()


# -------------------- 可选：定位指定 Minecraft 版本的协议数据 --------------------
# MINECRAFT_DATA_GAME_VERSION 为空时只提供数据根目录，由调用方自行拼接版本路径。
if (MINECRAFT_DATA_GAME_VERSION AND MinecraftData_PC_DIR)
    if (MINECRAFT_DATA_GAME_VERSION MATCHES "^[0-9]+$")
        g_l_warning_fmt(WMinecraftData3 ${MINECRAFT_DATA_GAME_VERSION})
    endif ()

    set(_minecraft_data_game_dir "${MinecraftData_PC_DIR}/${MINECRAFT_DATA_GAME_VERSION}")

    if (EXISTS "${_minecraft_data_game_dir}/protocol.json")
        set(MinecraftData_GAME_DIR "${_minecraft_data_game_dir}")
        set(MinecraftData_PROTOCOL_FILE "${_minecraft_data_game_dir}/protocol.json")

        if (EXISTS "${_minecraft_data_game_dir}/proto.yml")
            set(MinecraftData_PROTO_YML "${_minecraft_data_game_dir}/proto.yml")
        endif ()

        g_l_info_fmt(IMinecraftData6 ${MinecraftData_GAME_DIR})

    elseif (IS_DIRECTORY "${_minecraft_data_game_dir}")
        g_l_warning_fmt(WMinecraftData2 ${MINECRAFT_DATA_GAME_VERSION})

    else ()
        g_l_warning_fmt(WMinecraftData1 ${MINECRAFT_DATA_GAME_VERSION})

    endif ()
endif ()


# -------------------- 标准 FindPackage 处理 --------------------
set(_minecraft_data_required_vars
        MinecraftData_DATA_DIR
        MinecraftData_PC_DIR
)

if (MINECRAFT_DATA_GAME_VERSION)
    list(APPEND _minecraft_data_required_vars
            MinecraftData_GAME_DIR
            MinecraftData_PROTOCOL_FILE
    )
endif ()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(MinecraftData
        REQUIRED_VARS ${_minecraft_data_required_vars}
        HANDLE_COMPONENTS
)
