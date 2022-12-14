add_executable(thread_pool_tests)
file(GLOB_RECURSE thread_pool_tests_SOURCE_FILES "*.cpp")
target_sources(thread_pool_tests PRIVATE ${thread_pool_tests_SOURCE_FILES})
target_link_libraries(thread_pool_tests PRIVATE thread_pool gtest gtest_main)
