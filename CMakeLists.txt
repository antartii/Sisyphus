# === Setup ===
cmake_minimum_required(VERSION 3.12)
cmake_policy(SET CMP0048 NEW)

project(Sisyphus
    VERSION 0.1.0
    DESCRIPTION "C/C++ library for an engine using vulkan as it's graphic API and platform specific surfaces."
    HOMEPAGE_URL "https://github.com/antartii/Sisyphus"
    LANGUAGES C
)

set(COMPAT_VERSION 0.1.0)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(ENGINE_NAME ${PROJECT_NAME})
set(LIB_DIR ${CMAKE_BINARY_DIR}/lib)

# === Compiling shaders ===
set(SLANGC "$ENV{HOME}/VulkanSDK/1.4.309.0/x86_64/bin/slangc")
set(SHADER_OUTPUT ${CMAKE_BINARY_DIR}/shaders.spv)

add_custom_command(
    OUTPUT ${SHADER_OUTPUT}
    COMMAND ${SLANGC}
            ${CMAKE_CURRENT_SOURCE_DIR}/shaders/shaders.slang
            -target spirv
            -profile spirv_1_4
            -emit-spirv-directly
            -fvk-use-entrypoint-name
            -entry vertMain
            -entry fragMain
            -o ${SHADER_OUTPUT}
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/shaders/shaders.slang
    COMMENT "Compiling shaders.slang → ${SHADER_OUTPUT}"
)

add_custom_target(compile_shaders ALL
    DEPENDS ${SHADER_OUTPUT}
)

# === Options ===
set(SURFACE "" CACHE STRING "Select your surface api" )

if ("${SURFACE}" STREQUAL "wayland")
    find_package(PkgConfig)
    pkg_check_modules(PKG_WAYLAND wayland-client)
    find_path(SURFACE_INCLUDE_DIR NAMES "wayland-client.h" HINTS ${PKG_WAYLAND_INCLUDE_DIRS})
    find_library(SURFACE_LIBRARIES NAMES "wayland-client" HINTS ${PKG_WAYLAND_LIBRARY_DIRS})

    find_program(WAYLAND_SCANNER wayland-scanner REQUIRED)
    pkg_check_modules(WAYLAND_PROTOCOLS wayland-protocols)
    set(WAYLAND_PROTOCOLS_DIR ${WAYLAND_PROTOCOLS_PREFIX}/share/wayland-protocols)
    set(XDG_PROTOCOLS_XML ${WAYLAND_PROTOCOLS_DIR}/stable/xdg-shell/xdg-shell.xml)
    set(XDG_PROTOCOLS_OUTPUT ${LIB_DIR}/wayland-protocols)
    
    add_custom_command(
        OUTPUT ${XDG_PROTOCOLS_OUTPUT}/xdg-shell-protocol.c
            ${XDG_PROTOCOLS_OUTPUT}/xdg-shell-client-protocol.h
        COMMAND ${CMAKE_COMMAND} -E make_directory ${XDG_PROTOCOLS_OUTPUT}
        COMMAND ${WAYLAND_SCANNER} private-code ${XDG_PROTOCOLS_XML} ${XDG_PROTOCOLS_OUTPUT}/xdg-shell-protocol.c
        COMMAND ${WAYLAND_SCANNER} client-header ${XDG_PROTOCOLS_XML} ${XDG_PROTOCOLS_OUTPUT}/xdg-shell-client-protocol.h
        DEPENDS ${XDG_PROTOCOLS_XML}
    )

    set(SURFACE_SRC
        ${PROJECT_SOURCE_DIR}/src/surfaces/wayland.c
        ${XDG_PROTOCOLS_OUTPUT}/xdg-shell-protocol.c
    )
    list(APPEND SURFACE_INCLUDE_DIR ${XDG_PROTOCOLS_OUTPUT})
else()
    message(FATAL_ERROR "Surface invalid or not specified")
endif()

if (NOT SURFACE_INCLUDE_DIR OR NOT SURFACE_INCLUDE_DIR)
    message(FATAL_ERROR "Libraries and/or include directory for the specified surface not found")
endif()

foreach(dir IN LISTS SURFACE_INCLUDE_DIR)
    if(dir MATCHES "^${CMAKE_BINARY_DIR}")
        list(APPEND SURFACE_INCLUDE_BUILD ${dir})
    else()
        list(APPEND SURFACE_INCLUDE_SYSTEM ${dir})
    endif()
endforeach()

# === Create the target ===
add_library(${PROJECT_NAME}
    ${PROJECT_SOURCE_DIR}/src/sisyphus.c
    ${PROJECT_SOURCE_DIR}/src/engine.c
    ${PROJECT_SOURCE_DIR}/src/renderer.c
    ${PROJECT_SOURCE_DIR}/src/window.c
    ${PROJECT_SOURCE_DIR}/src/errors.c
    ${PROJECT_SOURCE_DIR}/src/utils.c
    ${PROJECT_SOURCE_DIR}/src/object.c
    ${PROJECT_SOURCE_DIR}/src/camera.c
    ${SURFACE_SRC}
)

add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

find_package(Vulkan REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC
    Vulkan::Vulkan
    ${SURFACE_LIBRARIES}
)

target_include_directories(${PROJECT_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/include>
    $<BUILD_INTERFACE:${SURFACE_INCLUDE_BUILD}>
    ${SURFACE_INCLUDE_SYSTEM}
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

configure_file(cmake/sisyphus_config.h.in include/sisyphus_config.h)

# === Definitions ===
if ("${SURFACE}" STREQUAL "wayland")
    target_compile_definitions(${PROJECT_NAME} PUBLIC VK_USE_PLATFORM_WAYLAND_KHR)
endif()

if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
    target_compile_definitions(${PROJECT_NAME} PUBLIC DEBUG)
endif()

# === Installing ===
if (WIN32)
    set(INCLUDES_DESTINATION include/${PROJECT_NAME})
    set(INSTALL_INTERFACE_INCLUDE include)
else()
    set(INCLUDES_DESTINATION include/${PROJECT_NAME})
    set(INSTALL_INTERFACE_INCLUDE include/${PROJECT_NAME})
endif()

include(GenerateExportHeader)

set_target_properties(${PROJECT_NAME} PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${COMPAT_VERSION}
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    C_VISIBILITY_PRESET hidden
    VISIBILITY_INLINES_HIDDEN TRUE
)

generate_export_header(
    ${PROJECT_NAME}
    EXPORT_FILE_NAME
    export/${PROJECT_NAME}/export_${PROJECT_NAME}.hpp
)

target_include_directories(${PROJECT_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/export>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>   
)

include(GNUInstallDirs)

install(TARGETS ${PROJECT_NAME}
    EXPORT ${PROJECT_NAME}Targets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/ ${PROJECT_BINARY_DIR}/include/ DESTINATION ${INCLUDES_DESTINATION})

include(CMakePackageConfigHelpers)

write_basic_package_version_file(
    "${PROJECT_NAME}ConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

configure_package_config_file(
    "${CMAKE_CURRENT_LIST_DIR}/cmake/${PROJECT_NAME}Config.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
    INSTALL_DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/cmake/${PROJECT_NAME}
)

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
    DESTINATION lib/cmake/${PROJECT_NAME}
)

install(EXPORT ${PROJECT_NAME}Targets
    FILE ${PROJECT_NAME}Targets.cmake
    NAMESPACE ${PROJECT_NAME}::
    DESTINATION lib/cmake/${PROJECT_NAME}
)
