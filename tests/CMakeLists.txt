set(TEST_SOURCES testsDico.cpp ${PROJECT_SOURCE_DIR}/DicoSynonymes.cpp)

add_executable(testsDico ${TEST_SOURCES})

target_include_directories(testsDico PRIVATE ${PROJECT_SOURCE_DIR})

target_link_libraries(testsDico
        gtest_main
        gtest
        pthread
)

include(GoogleTest)

gtest_discover_tests(testsDico)
