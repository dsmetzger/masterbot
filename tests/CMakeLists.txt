find_package(GTest QUIET)

if(GTest_FOUND)
    file(GLOB_RECURSE TEST_SOURCES "*.cpp")
    
    add_executable(masterbot_tests ${TEST_SOURCES})
    
    target_link_libraries(masterbot_tests 
        PRIVATE 
        GTest::gtest_main
        CURL::libcurl
        nlohmann_json::nlohmann_json
    )
    
    target_include_directories(masterbot_tests PRIVATE ../include)
    
    include(GoogleTest)
    gtest_discover_tests(masterbot_tests)
else()
    message(STATUS "GTest not found. Skipping tests.")
endif()