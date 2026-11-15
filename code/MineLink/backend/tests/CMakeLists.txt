project(Minelink_Core_Test VERSION 1.0.0)

file(GLOB_RECURSE MINELINK_CORE_TEST_SRC CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/core/*.cpp")

add_executable(Minelink_Core_Test "${MINELINK_CORE_TEST_SRC}")

target_link_libraries(Minelink_Core_Test PRIVATE Minelink::Core GTest::gtest_main)

add_test(NAME Minelink_Core_Test COMMAND Minelink_Core_Test)

set_tests_properties(Minelink_Core_Test PROPERTIES
    TIMEOUT 120
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)
