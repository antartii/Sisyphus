# === Setup ===
cmake_minimum_required(VERSION 3.12)
cmake_policy(SET CMP0048 NEW)

project(Sisyphuspp
    VERSION 0.1.0
    DESCRIPTION "C++ library for an engine using vulkan as it's graphic API and platform specific surfaces."
    HOMEPAGE_URL "https://github.com/antartii/Sisyphus"
    LANGUAGES CXX
)

set(COMPAT_VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# === Options ===
set(SURFACE "" CACHE STRING "Select your surface api")

option(CXX_BUILD_STATIC "Build sisyphus with visibility of symbols not hidden" OFF)
set(C_BUILD_STATIC ON CACHE BOOL "C build static" FORCE)
if (CXX_BUILD_STATIC)
    set(CXX_LIB_TYPE STATIC)
else()
    set(CXX_LIB_TYPE SHARED)
endif()

# === Create the target ===
add_subdirectory(../ ${PROJECT_BINARY_DIR}/SisyphusC)

add_library(${PROJECT_NAME} ${CXX_LIB_TYPE}
    ${PROJECT_SOURCE_DIR}/src/engine.cpp
    ${PROJECT_SOURCE_DIR}/src/config.cpp
    ${PROJECT_SOURCE_DIR}/src/renderer.cpp
    ${PROJECT_SOURCE_DIR}/src/utils.cpp
    ${PROJECT_SOURCE_DIR}/src/window.cpp
    ${PROJECT_SOURCE_DIR}/src/camera.cpp
    ${PROJECT_SOURCE_DIR}/src/object.cpp
)

add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME} PRIVATE
    #Sisyphus::Sisyphus
    Sisyphus
)

target_include_directories(${PROJECT_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    C_VISIBILITY_PRESET hidden
    CXX_VISIBILITY_PRESET hidden
    VISIBILITY_INLINES_HIDDEN TRUE
    VERSION ${PROJECT_VERSION}
    SOVERSION ${COMPAT_VERSION}
)

if (CXX_BUILD_STATIC)
    set_target_properties(${PROJECT_NAME} PROPERTIES POSITION_INDEPENDENT_CODE ON)
endif()
