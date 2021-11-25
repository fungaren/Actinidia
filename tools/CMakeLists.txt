include_directories(
    ${PROJECT_SOURCE_DIR}/common
    ${PROJECT_SOURCE_DIR}/tools
)
aux_source_directory(${PROJECT_SOURCE_DIR}/common SOURCES)
if (UNIX)
    # Find dependencies
    find_package(ZLIB REQUIRED)
    find_package(PNG REQUIRED)
    find_package(JPEG REQUIRED)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(GTK3 REQUIRED gtk+-3.0)

    include_directories(
        ${ZLIB_INCLUDE_DIRS}
        ${PNG_INCLUDE_DIRS}
        ${JPEG_INCLUDE_DIRS}
        ${GTK3_INCLUDE_DIRS}
    )

    set(EXTRA_LIBS
        ${PNG_LIBRARIES}
        ${JPEG_LIBRARIES}
        ${ZLIB_LIBRARIES}
        ${GTK3_LIBRARIES}
        stdc++fs
    )
    list(APPEND SOURCES "tools_cmd.cpp")
elseif (WIN32)
    include_directories(
        ${PROJECT_SOURCE_DIR}/external/zlib
        ${PROJECT_SOURCE_DIR}/external/libpng
        ${PROJECT_SOURCE_DIR}/external/libjpeg
        ${lua_SOURCE_DIR}
    )
    link_directories(
        ${PROJECT_SOURCE_DIR}/external/libjpeg
        ${PROJECT_SOURCE_DIR}/external/libpng
        ${PROJECT_SOURCE_DIR}/external/zlib
        ${lua_BINARY_DIR}
    )
    set(EXTRA_LIBS_DEBUG
        jpegd.lib
        libpng16d.lib
        zlibd.lib
    )
    set(EXTRA_LIBS_RELEASE
        jpeg.lib
        libpng16.lib
        zlib.lib
    )
    set(EXTRA_LIBS $<$<CONFIG:Debug>:${EXTRA_LIBS_DEBUG}>$<$<CONFIG:Release>:${EXTRA_LIBS_RELEASE}>)
    list(APPEND SOURCES
        "tools.cpp"
        "snake/snake.cpp"
    )
    # Resource file
    configure_file(
        "static/tools.rc.in"
        "static/tools.rc"
    )
    list(APPEND SOURCES
        "static/tools.rc"
        "static/tools.manifest"
    )
endif()

set(TARGET actools)
add_executable(${TARGET} WIN32 ${SOURCES})

# Link libraries
target_link_libraries(${TARGET} "${EXTRA_LIBS}")

if (UNIX)
    # Install the program
    install(TARGETS ${TARGET} RUNTIME DESTINATION bin)
elseif (WIN32)
    # Required DLLs on Windows
    set(ALL_DLLS_DEBUG
        "${PROJECT_SOURCE_DIR}/external/jpeg62d.dll"
        "${PROJECT_SOURCE_DIR}/external/libpng16d.dll"
        "${PROJECT_SOURCE_DIR}/external/zlibd.dll"
    )
    set(ALL_DLLS_RELEASE
        "${PROJECT_SOURCE_DIR}/external/jpeg62.dll"
        "${PROJECT_SOURCE_DIR}/external/libpng16.dll"
        "${PROJECT_SOURCE_DIR}/external/zlib.dll"
    )
    set(ALL_DLLS $<$<CONFIG:Debug>:${ALL_DLLS_DEBUG}>$<$<CONFIG:Release>:${ALL_DLLS_RELEASE}>)
    # Copy DLLs
    add_custom_command(TARGET ${TARGET} POST_BUILD
        COMMAND_EXPAND_LISTS
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${ALL_DLLS}" "${PROJECT_BINARY_DIR}/tools/"
        COMMENT "Copying DLLs" VERBATIM
    )
endif()
