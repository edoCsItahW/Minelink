if (BUILD_CORE)
    add_subdirectory(core)
endif ()

if (BUILD_INTERFACES)
    add_subdirectory(interfaces)
endif ()

if (BUILD_TESTS)
    add_subdirectory(tests)
endif ()