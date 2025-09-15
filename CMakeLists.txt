cmake_minimum_required(VERSION 3.16)
project(MasterBot VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(PkgConfig REQUIRED)
find_package(CURL REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(Filesystem REQUIRED)

include_directories(include)

file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE HEADERS "include/*.h")

add_executable(masterbot ${SOURCES} ${HEADERS})

target_link_libraries(masterbot 
    PRIVATE 
    CURL::libcurl
    nlohmann_json::nlohmann_json
    stdc++fs
)

target_include_directories(masterbot PRIVATE include)

enable_testing()
add_subdirectory(tests)