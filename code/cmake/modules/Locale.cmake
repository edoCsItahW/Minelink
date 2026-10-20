#[=======================================================================[.rst:
Locale
------

提供构建消息的本地化支持。

消息 ID 格式为 ``G_{Level}{Component}{Number}``：
- Level: I/D/W/E
- Component: 组件全名，如 Functions、CommonSettings、Dependencies
- Number: 两位序号

可用变量：
``GLOBAL_LANG``
  当前消息语言，默认 ``en-us``。

提供的命令：
``g_localize(<MSG_ID> <OUT_VAR>)``
  将消息 ID 解析为当前语言文本。

``g_localize_fmt(<MSG_ID> <OUT_VAR> [ARG...])``
  解析消息并替换 ``{0}``、``{1}`` 等占位符。

``g_l_debug`` / ``g_l_info`` / ``g_l_warning`` / ``g_l_error``
  本地化后输出对应级别消息，均有 ``_fmt`` 变体。
#]=======================================================================]

include(${CMAKE_CURRENT_LIST_DIR}/Debug.cmake)

# 默认语言；可通过 -DGLOBAL_LANG=zh-hans 覆盖。
set(GLOBAL_DEFAULT_LANG "en-us")
if (NOT GLOBAL_LANG)
    set(GLOBAL_LANG "${GLOBAL_DEFAULT_LANG}" CACHE STRING "Language for build message (e.g., en-us, zh-hans)")
endif ()

set(_G_LOCALE_DIR "${CMAKE_CURRENT_LIST_DIR}/locales")

# 按需加载当前语言文件；语言变化时重新加载。
macro(_g_load_locale)
    if (NOT DEFINED _G_LOADED_LANG OR NOT "${_G_LOADED_LANG}" STREQUAL "${GLOBAL_LANG}")
        set(CURRENT_LOCALE_FILE "${_G_LOCALE_DIR}/${GLOBAL_LANG}.cmake")

        if (EXISTS ${CURRENT_LOCALE_FILE})
            include(${CURRENT_LOCALE_FILE})
        else ()
            include(${_G_LOCALE_DIR}/${GLOBAL_DEFAULT_LANG}.cmake)
        endif ()

        set(_G_LOADED_LANG "${GLOBAL_LANG}")
    endif ()
endmacro()

# g_localize(<MSG_ID> <OUT_VAR>)
# 将 MSG_ID 解析为当前语言文本，结果写入 OUT_VAR。
# 若未找到对应条目，则回退为 "[<lang>_<MSG_ID>]"。
macro(g_localize MSG_ID OUT_VAR)
    _g_load_locale()

    set(G_LOC_VAR "G_${MSG_ID}")

    if (DEFINED ${G_LOC_VAR})
        set(${OUT_VAR} "${${G_LOC_VAR}}")
    else ()
        set(${OUT_VAR} "[${GLOBAL_LANG}_${MSG_ID}]")
    endif ()

    unset(G_LOC_VAR)
endmacro()

# g_localize_fmt(<MSG_ID> <OUT_VAR> [ARG...])
# 解析消息并依次替换 {0}、{1} 等占位符。
function(g_localize_fmt MSG_ID OUT_VAR)
    g_localize(${MSG_ID} _CONTENT)

    set(_ARG_IDX 0)

    foreach (_ARG ${ARGN})
        string(REPLACE "{${_ARG_IDX}}" "${_ARG}" _CONTENT "${_CONTENT}")
        math(EXPR _ARG_IDX "${_ARG_IDX} + 1")
    endforeach ()

    set(${OUT_VAR} "${_CONTENT}" PARENT_SCOPE)
endfunction()

# 未本地化的便捷宏：直接输出消息文本。
macro(l_message MSG_ID)
    g_localize(${MSG_ID} _CONTENT)
    message(${_CONTENT})
    unset(_CONTENT)
endmacro()

macro(l_message_fmt MSG_ID)
    g_localize_fmt(${MSG_ID} _CONTENT ${ARGN})
    message(${_CONTENT})
    unset(_CONTENT)
endmacro()

# 本地化 DEBUG/INFO/WARNING/ERROR 消息。
function(g_l_debug MSG_ID)
    g_localize(${MSG_ID} _CONTENT)
    g_debug(${_CONTENT})
endfunction()

function(g_l_debug_fmt MSG_ID)
    g_localize_fmt(${MSG_ID} _CONTENT ${ARGN})
    g_debug(${_CONTENT})
endfunction()

function(g_l_info MSG_ID)
    g_localize(${MSG_ID} _CONTENT)
    g_info(${_CONTENT})
endfunction()

function(g_l_info_fmt MSG_ID)
    g_localize_fmt(${MSG_ID} _CONTENT ${ARGN})
    g_info(${_CONTENT})
endfunction()

function(g_l_warning MSG_ID)
    g_localize(${MSG_ID} _CONTENT)
    g_warning(${_CONTENT})
endfunction()

function(g_l_warning_fmt MSG_ID)
    g_localize_fmt(${MSG_ID} _CONTENT ${ARGN})
    g_warning(${_CONTENT})
endfunction()

function(g_l_error MSG_ID)
    g_localize(${MSG_ID} _CONTENT)
    g_error(${_CONTENT})
endfunction()

function(g_l_error_fmt MSG_ID)
    g_localize_fmt(${MSG_ID} _CONTENT ${ARGN})
    g_error(${_CONTENT})
endfunction()