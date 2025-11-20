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

# === Vulkan ===
find_package(Vulkan REQUIRED)

# === Surfaces ===
set(SURFACE "wayland" CACHE STRING "Select your surface api")

if ("${SURFACE}" STREQUAL "wayland")
    include(cmake/DetectWayland.cmake)
else()
    message(FATAL_ERROR "Surface invalid or not specified, you specified : ${SURFACE}")
endif()

if (NOT SURFACE_INCLUDE_DIR OR NOT SURFACE_LIBRARIES)
    message(FATAL_ERROR "Libraries and/or include directory for the specified surface not found")
endif()

foreach(dir IN LISTS SURFACE_INCLUDE_DIR)
    if(dir MATCHES "^${CMAKE_BINARY_DIR}")
        list(APPEND SURFACE_INCLUDE_BUILD ${dir})
    else()
        list(APPEND SURFACE_INCLUDE_SYSTEM ${dir})
    endif()
endforeach()

# === Visibility ===

option(BUILD_SHARED_LIBS "Build sisyphus with visibility of symbols not hidden" ON)
if (BUILD_SHARED_LIBS)
    set(C_LIB_TYPE SHARED)
else()
    set(C_LIB_TYPE STATIC)
endif()

# === Create the target ===
add_library(${PROJECT_NAME} ${C_LIB_TYPE}
    ${PROJECT_SOURCE_DIR}/src/sisyphus.c
    ${PROJECT_SOURCE_DIR}/src/engine.c
    ${PROJECT_SOURCE_DIR}/src/renderer.c
    ${PROJECT_SOURCE_DIR}/src/window.c
    ${PROJECT_SOURCE_DIR}/src/errors.c
    ${PROJECT_SOURCE_DIR}/src/utils.c
    ${PROJECT_SOURCE_DIR}/src/object.c
    ${PROJECT_SOURCE_DIR}/src/camera.c
    ${PROJECT_SOURCE_DIR}/src/input_manager.c

    ${PROJECT_SOURCE_DIR}/src/vulkan_context/vulkan_context.c
    ${PROJECT_SOURCE_DIR}/src/vulkan_context/ext_func.c
    ${PROJECT_SOURCE_DIR}/src/vulkan_context/buffer.c
    ${PROJECT_SOURCE_DIR}/src/vulkan_context/utils.c
    ${PROJECT_SOURCE_DIR}/src/vulkan_context/instance.c
    ${PROJECT_SOURCE_DIR}/src/vulkan_context/debug.c
    ${PROJECT_SOURCE_DIR}/src/vulkan_context/surface.c
    ${PROJECT_SOURCE_DIR}/src/vulkan_context/device.c
    ${PROJECT_SOURCE_DIR}/src/vulkan_context/swapchain.c
    ${PROJECT_SOURCE_DIR}/src/vulkan_context/graphic.c
    ${PROJECT_SOURCE_DIR}/src/vulkan_context/command_buffer.c
    ${PROJECT_SOURCE_DIR}/src/vulkan_context/sync_object.c
    ${PROJECT_SOURCE_DIR}/src/vulkan_context/texture.c

    ${SURFACE_SRC}
)

add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME} PUBLIC
    Vulkan::Vulkan
    m
    ${SURFACE_LIBRARIES}
)

include(GNUInstallDirs)

target_include_directories(${PROJECT_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/Sisyphus>
    $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/lib>
    $<BUILD_INTERFACE:${SURFACE_INCLUDE_BUILD}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    ${SURFACE_INCLUDE_SYSTEM}
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

if (NOT BUILD_SHARED_LIBS)
    set_target_properties(${PROJECT_NAME} PROPERTIES POSITION_INDEPENDENT_CODE ON)
endif()

# === Compiling shaders ===
set(SLANGC "$ENV{VULKAN_SDK}/bin/slangc")
set(SHADER_FILE shaders.spv)
set(SHADER_OUTPUT ${CMAKE_BINARY_DIR}/${SHADER_FILE})
if (NOT DEFINED SHADERS_PATH)
    set(SHADERS_PATH shaders.spv)
endif()

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

add_dependencies(${PROJECT_NAME} compile_shaders)

# === Definitions ===
if ("${SURFACE}" STREQUAL "wayland")
    target_compile_definitions(${PROJECT_NAME} PUBLIC VK_USE_PLATFORM_WAYLAND_KHR)
endif()

if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
    target_compile_definitions(${PROJECT_NAME} PUBLIC DEBUG)
endif()

# === Configure files ===
configure_file(cmake/sisyphus_config.h.in include/sisyphus_config.h @ONLY)
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/cmake/Uninstall.cmake.in" "${CMAKE_CURRENT_BINARY_DIR}/Uninstall.cmake" @ONLY)

# === Installing ===
install(
    TARGETS ${PROJECT_NAME}
    EXPORT ${PROJECT_NAME}Targets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(
    FILES ${PROJECT_BINARY_DIR}/include/sisyphus_config.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}
)

install(
    DIRECTORY ${PROJECT_SOURCE_DIR}/include/Sisyphus/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}
)

install(
    EXPORT ${PROJECT_NAME}Targets
    FILE ${PROJECT_NAME}Targets.cmake
    NAMESPACE ${PROJECT_NAME}::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)

# === Package ===
include(CMakePackageConfigHelpers)

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/SisyphusConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/SisyphusConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/SisyphusConfig.cmake"
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)

# === Uninstalling ===
add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/Uninstall.cmake
)

add_custom_target(force_uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_SOURCE_DIR}/cmake/ForceUninstall.cmake
    COMMENT "Uninstalling Sisyphus from system directories..."
)