# === Setup ===
cmake_minimum_required(VERSION 3.12)
cmake_policy(SET CMP0048 NEW)

project(Sisyphus
    VERSION 0.1.0
    DESCRIPTION "C/C++ library for an engine using vulkan as it's graphic API and platform specific surfaces."
    HOMEPAGE_URL "https://github.com/antartii/Sisyphus"
    LANGUAGES C
)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED True)

configure_file(cmake/sisyphus_config.h.in include/sisyphus_config.h)

# === Options ===
set(SURFACE "wayland" CACHE STRING "Select your surface api" ) # Linux specific

# === Create the target ===
add_library(${PROJECT_NAME}
    ${PROJECT_SOURCE_DIR}/src/sisyphus.c
)

target_include_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${PROJECT_BINARY_DIR}/include
)

## === Installing ===
if (WIN32)
    set(INCLUDES_DESTINATION include/${PROJECT_NAME})
    set(INSTALL_INTERFACE_INCLUDE include)
else()
    set(INCLUDES_DESTINATION include/${PROJECT_NAME})
    set(INSTALL_INTERFACE_INCLUDE include/${PROJECT_NAME})
endif()

install(TARGETS ${PROJECT_NAME}
    DESTINATION lib
)
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/ ${PROJECT_BINARY_DIR}/include/ DESTINATION ${INCLUDES_DESTINATION})

