#add_library(lib_minelink_cli)
#
#add_library(Minelink::CLI ALIAS lib_minelink_cli)
#
#
#file(GLOB_RECURSE MINELINK_CLI_MODULES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/**/*.cxx")
#
#file(GLOB_RECURSE MINELINK_CLI_SRC CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/**/*.cpp")
#
#target_sources(lib_minelink_cli
#        PUBLIC
#        FILE_SET CXX_MODULES
#        FILES "${MINELINK_CLI_MODULES}"
#        PRIVATE
#        "${MINELINK_CLI_SRC}"
#)
#
#target_compile_features(lib_minelink_cli PUBLIC cxx_std_26)
#
#target_compile_options(lib_minelink_cli PUBLIC
#        $<$<COMPILE_LANG_AND_ID:CXX,GNU>:-freflection>
#        $<$<COMPILE_LANG_AND_ID:CXX,GNU>:-fcontracts>
#)
#
#target_link_libraries(lib_minelink_cli PRIVATE stdc++exp PUBLIC Minelink::Core)
#
#add_executable(Minelink_CLI main.cpp)
#
#target_link_libraries(Minelink_CLI Minelink::CLI)
