project(actinidia)
cmake_minimum_required(VERSION 3.4.1)
find_package(ZLIB REQUIRED)
find_package(PNG REQUIRED)
find_package(JPEG REQUIRED)
find_package(Lua REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK3 REQUIRED gtk+-3.0)

include_directories(
    ${ZLIB_INCLUDE_DIRS}
    ${PNG_INCLUDE_DIRS}
    ${JPEG_INCLUDE_DIRS}
    ${LUA_INCLUDE_DIRS}
    ${GTK3_INCLUDE_DIRS}
)
set(CMAKE_CXX_STANDARD 17)
aux_source_directory(../Tools/Common SOURCES)
list(APPEND SOURCES
    "Interface.cpp"
    "Actinidia.cpp"
)
set(target ${PROJECT_NAME})
add_executable(${target} ${SOURCES})

target_link_libraries(
    ${target}
    ${LUA_LIBRARIES}
    ${GTK3_LIBRARIES}
    ${PNG_LIBRARIES}
    ${JPEG_LIBRARIES}
    ${ZLIB_LIBRARIES}
    stdc++fs
    pthread
)

add_definitions(
    -D_GTK
)

string(CONCAT CMAKE_CXX_FLAGS_DEBUG
    "-Wall "            # display all warnings
    "-g "               # with debug information
    "-O0 "               # no optimize
    "-Wno-parentheses"   # disable parentheses warning
    )

string(CONCAT CMAKE_CXX_FLAGS_RELEASE
    "-O3"               # enable optimize
    )

message(STATUS "CMAKE_BUILD_TYPE: " ${CMAKE_BUILD_TYPE})
