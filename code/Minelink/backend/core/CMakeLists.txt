project(Minelink_Core VERSION 1.0.0)

add_library(lib_minelink_core)

add_library(Minelink::Core ALIAS lib_minelink_core)


file(GLOB_RECURSE MINELINK_CORE_MODULES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/**/*.cxx")

file(GLOB_RECURSE MINELINK_CORE_SRC CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/**/*.cpp")

target_sources(lib_minelink_core
    PUBLIC
        FILE_SET CXX_MODULES
        FILES "${MINELINK_CORE_MODULES}"
    PRIVATE
        "${MINELINK_CORE_SRC}"
)

target_compile_features(lib_minelink_core PUBLIC cxx_std_26)

target_compile_options(lib_minelink_core PUBLIC
        $<$<COMPILE_LANG_AND_ID:CXX,GNU>:-freflection>
        $<$<COMPILE_LANG_AND_ID:CXX,GNU>:-fcontracts>
)

target_link_libraries(lib_minelink_core stdc++exp)
