# ============================================================================
# Debug.cmake
# 统一消息输出接口。
#
# 提供 g_message() 及 g_debug()/g_info()/g_warning()/g_error() 包装。
# 所有消息统一添加 [GLOBAL] 前缀，便于在构建日志中过滤。
# ============================================================================

# g_message(<LEVEL> <MESSAGE>)
# 按 LEVEL 输出消息：INFO/WARNING/ERROR/DEBUG。
# MESSAGE 为空时直接返回。
function(g_message LEVEL MESSAGE)
    if (NOT MESSAGE)
        return()
    endif ()

    if (LEVEL STREQUAL "INFO")
        message(STATUS "[GLOBAL] - ${LEVEL}: ${MESSAGE}")
    elseif (LEVEL STREQUAL "WARNING")
        message(WARNING "[GLOBAL] - ${LEVEL}: ${MESSAGE}")
    elseif (LEVEL STREQUAL "ERROR")
        message(FATAL_ERROR "[GLOBAL] - ${LEVEL}: ${MESSAGE}")
    elseif (LEVEL STREQUAL "DEBUG")
        message(DEBUG "[GLOBAL] - ${LEVEL}: ${MESSAGE}")
    endif ()
endfunction()

# g_debug(<MESSAGE>) 输出 DEBUG 级消息。
function(g_debug MESSAGE)
    g_message(DEBUG ${MESSAGE})
endfunction()

# g_info(<MESSAGE>) 输出 INFO 级消息。
function(g_info MESSAGE)
    g_message(INFO ${MESSAGE})
endfunction()

# g_warning(<MESSAGE>) 输出 WARNING 级消息。
function(g_warning MESSAGE)
    g_message(WARNING ${MESSAGE})
endfunction()

# g_error(<MESSAGE>) 输出 ERROR 级消息，并终止配置。
function(g_error MESSAGE)
    g_message(ERROR ${MESSAGE})
endfunction()