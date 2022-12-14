add_executable(thread_safe_tests)
file(GLOB_RECURSE thread_safe_tests_SOURCE_FILES "*.cpp")
target_sources(thread_safe_tests PRIVATE ${thread_safe_tests_SOURCE_FILES})
target_link_libraries(thread_safe_tests PRIVATE thread_safe gtest gtest_main)
