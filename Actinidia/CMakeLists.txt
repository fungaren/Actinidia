if (UNIX)
    # Find dependencies
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
    link_directories(
        ${PROJECT_SOURCE_DIR}/external/bass
    )

    # Set BASS architecture
    if (CMAKE_HOST_SYSTEM_PROCESSOR STREQUAL "arm")
        set(BASS "bass-hardfp")
    elseif (CMAKE_HOST_SYSTEM_PROCESSOR STREQUAL "aarch64")
        set(BASS "bass-aarch64")
    elseif (CMAKE_HOST_SYSTEM_PROCESSOR STREQUAL "i386")
        set(BASS "bass-x86")
    elseif (CMAKE_HOST_SYSTEM_PROCESSOR STREQUAL "i686")
        set(BASS "bass-x86")
    elseif (CMAKE_HOST_SYSTEM_PROCESSOR STREQUAL "x86_64")
        set(BASS "bass-x86_64")
    else()
        message(FATAL_ERROR "Unsupported processor: ${CMAKE_HOST_SYSTEM_PROCESSOR}")
    endif()
    message(STATUS "Using lib${BASS}.so")

    set(EXTRA_LIBS
        ${LUA_LIBRARIES}
        ${GTK3_LIBRARIES}
        ${PNG_LIBRARIES}
        ${JPEG_LIBRARIES}
        ${ZLIB_LIBRARIES}
        ${BASS}
        stdc++fs
        pthread
    )
elseif (WIN32)
    include_directories(
        ${PROJECT_SOURCE_DIR}/external/bass
        ${PROJECT_SOURCE_DIR}/external/zlib
        ${PROJECT_SOURCE_DIR}/external/libpng
        ${PROJECT_SOURCE_DIR}/external/libjpeg
        ${lua_SOURCE_DIR}
    )
    link_directories(
        ${PROJECT_SOURCE_DIR}/external/bass
        ${PROJECT_SOURCE_DIR}/external/libjpeg
        ${PROJECT_SOURCE_DIR}/external/libpng
        ${PROJECT_SOURCE_DIR}/external/zlib
        ${lua_BINARY_DIR}
    )
    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(EXTRA_LIBS
            bass.lib
            liblua.lib
            jpegd.lib
            libpng16d.lib
            zlibd.lib
        )
    else()
        set(EXTRA_LIBS
            bass.lib
            liblua.lib
            jpeg.lib
            libpng16.lib
            zlib.lib
        )
    endif()
endif()

# Add sources
aux_source_directory(${PROJECT_SOURCE_DIR}/common SOURCES)
list(APPEND SOURCES
    "interface.cpp"
    "actinidia.cpp"
)

if (UNIX)
    # Create desktop icon
    configure_file(
        "static/actinidia.desktop.in"
        "static/actinidia.desktop"
    )
elseif (WIN32)
    # Resource file
    configure_file(
        "static/actinidia.rc.in"
        "static/actinidia.rc"
    )
    list(APPEND SOURCES "static/actinidia.rc")
    list(APPEND SOURCES "static/actinidia.manifest")
endif()

set(TARGET actinidia)
add_executable(${TARGET} ${SOURCES})

# Link libraries
target_link_libraries(${TARGET} ${EXTRA_LIBS})

if (UNIX)
    # Install the program
    install(TARGETS ${TARGET} RUNTIME DESTINATION bin)

    # Install the bass library
    install(FILES "${PROJECT_SOURCE_DIR}/external/bass/lib${BASS}.so" TYPE LIB)

    # Set runpath
    # set_property(TARGET ${TARGET} PROPERTY INSTALL_RPATH ${CMAKE_INSTALL_LIBDIR})

    # Copy resource pack if provided
    install(FILES "static/game.res"
        DESTINATION ${CMAKE_INSTALL_PREFIX}/share/${PROJECT_NAME}
        PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ
        OPTIONAL
    )

    # Copy logo
    install(FILES
        "static/logo.png"
        DESTINATION ${CMAKE_INSTALL_PREFIX}/share/${PROJECT_NAME}
        PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ
    )

    # Create desktop icon
    install(FILES
        "static/actinidia.desktop"
        DESTINATION ${CMAKE_INSTALL_PREFIX}/share/applications
        PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ
    )

elseif (WIN32)
    # Required DLLs on Windows
    set(ALL_DLLS_DEBUG
        "${PROJECT_SOURCE_DIR}/external/bass.dll"
        "${PROJECT_SOURCE_DIR}/external/jpeg62d.dll"
        "${PROJECT_SOURCE_DIR}/external/libpng16d.dll"
        "${PROJECT_SOURCE_DIR}/external/zlibd.dll"
    )
    set(ALL_DLLS_RELEASE
        "${PROJECT_SOURCE_DIR}/external/bass.dll"
        "${PROJECT_SOURCE_DIR}/external/jpeg62.dll"
        "${PROJECT_SOURCE_DIR}/external/libpng16.dll"
        "${PROJECT_SOURCE_DIR}/external/zlib.dll"
    )
    set(ALL_DLLS $<$<CONFIG:Debug>:${ALL_DLLS_DEBUG}>$<$<CONFIG:Release>:${ALL_DLLS_RELEASE}>)
    set(STATIC_RESOURCES
        "static/logo.png"
        "static/actinidia.visualelementsmanifest.xml"
    )
    # Copy DLLs and static resources
    add_custom_command(TARGET ${TARGET} POST_BUILD
        COMMAND_EXPAND_LISTS
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${ALL_DLLS} ${PROJECT_BINARY_DIR}
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${STATIC_RESOURCES} ${PROJECT_BINARY_DIR}
        COMMENT "Copying DLLs and static resources" VERBATIM
    )
endif()
