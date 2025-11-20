find_package(PkgConfig REQUIRED)

pkg_check_modules(PKG_WAYLAND wayland-client REQUIRED)
pkg_check_modules(WAYLAND_PROTOCOLS wayland-protocols REQUIRED)

find_program(WAYLAND_SCANNER wayland-scanner REQUIRED)

find_path(SURFACE_INCLUDE_DIR NAMES "wayland-client.h" HINTS ${PKG_WAYLAND_INCLUDE_DIRS})
find_library(SURFACE_LIBRARIES NAMES "wayland-client" HINTS ${PKG_WAYLAND_LIBRARY_DIRS})

if (NOT SURFACE_LIBRARIES OR NOT SURFACE_INCLUDE_DIR)
    message(FATAL_ERROR "Wayland cient library couldn't be found make sure you've installed it correctly")
endif()

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

# === XKB ===
pkg_check_modules(XKB REQUIRED xkbcommon)

list(APPEND SURFACE_LIBRARIES ${XKB_LIBRARIES})
list(APPEND SURFACE_INCLUDE_DIR ${XKB_INCLUDE_DIRS})
