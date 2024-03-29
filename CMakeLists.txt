cmake_minimum_required(VERSION 3.11)
if (${CMAKE_VERSION} VERSION_GREATER_EQUAL "3.20")
    cmake_policy(SET CMP0115 NEW)
endif()
project(actinidia)

set(PROJECT_HOMEPAGE_URL "https://github.com/mooction/Actinidia")
set(PROJECT_DESCRIPTION "Actinidia game engine and tools allow you \
to use Lua to write your games running on Windows10/Linux/Android.")
set(PROJECT_COPYRIGHT "Copyright (C) 2021")
set(PROJECT_VERSION_MAJOR 1)
set(PROJECT_VERSION_MINOR 0)
set(PROJECT_VERSION_PATCH 1)
message(STATUS "Actinidia version: ${PROJECT_VERSION_MAJOR}.\
${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}")

# For std::filesystem
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set build type
if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release")
endif()

if (UNIX)
    string(CONCAT CMAKE_CXX_FLAGS_DEBUG
        "-Wall "            # Display all warnings
        "-g "               # With debug information
        "-O0 "              # No optimize
    )
    string(CONCAT CMAKE_CXX_FLAGS_RELEASE
        "-O3 "               # Enable optimize
    )
    add_definitions(-D_GTK)
elseif (WIN32)
    add_definitions(-DUNICODE -D_UNICODE)
endif()

message(STATUS "CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")
# Default path is /usr/local/
message(STATUS "CMAKE_INSTALL_PREFIX: ${CMAKE_INSTALL_PREFIX}")

if (WIN32)
    include(external/lua/CMakeLists.txt)
endif()
add_subdirectory(actinidia)
add_subdirectory(tools)

if (UNIX)
    # Create package
    set(CPACK_PACKAGE_NAME ${PROJECT_NAME})
    set(CPACK_PACKAGE_DESCRIPTION ${PROJECT_DESCRIPTION})
    set(CPACK_PACKAGE_DESCRIPTION_SUMMARY ${PROJECT_DESCRIPTION})
    set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
    set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
    set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})
    # Set deb package configs
    set(CPACK_GENERATOR "DEB")
    set(CPACK_DEBIAN_FILE_NAME "DEB-DEFAULT")
    set(CPACK_DEBIAN_PACKAGE_HOMEPAGE ${PROJECT_HOMEPAGE_URL})
    set(CPACK_DEBIAN_PACKAGE_SECTION "Game")
    set(CPACK_DEBIAN_PACKAGE_PRIORITY "Optional")
    set(CPACK_DEBIAN_PACKAGE_MAINTAINER "FANG")
    set(CPACK_DEBIAN_PACKAGE_DEPENDS
        "libgtk-3-0, zlib1g, libpng16-16, liblua5.3-0, libjpeg62-turbo|libjpeg62"
    )
    include(CPack)
endif()
