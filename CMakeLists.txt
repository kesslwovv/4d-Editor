
cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

project(editor)

file(GLOB_RECURSE cxx_sources 4d\ Object\ Editor/*.cpp)
file(GLOB_RECURSE cxx_external_sources Externals/*.cpp)
file(GLOB_RECURSE c_external_sources Externals/*.c)

message("project cpp files: ${cxx_sources}")
message("external cpp files: ${cxx_external_sources}")
message("external c files: ${c_external_sources}")


include_directories(${CMAKE_SOURCE_DIR}/4d\ Object\ Editor)
include_directories(${CMAKE_SOURCE_DIR}/4d\ Object\ Editor/Renderer)
include_directories(${CMAKE_SOURCE_DIR}/4d\ Object\ Editor/utils)

include_directories(SYSTEM ${CMAKE_SOURCE_DIR}/Externals)
include_directories(SYSTEM ${CMAKE_SOURCE_DIR}/Externals/imgui)
include_directories(SYSTEM ${CMAKE_SOURCE_DIR}/Externals/FileBrowser)
include_directories(SYSTEM ${CMAKE_SOURCE_DIR}/Externals/json)

add_executable(editor ${cxx_sources} ${cxx_external_sources} ${c_external_sources})

target_link_libraries(editor ${CMAKE_SOURCE_DIR}/Externals/glfw/libglfw3.a)


if (APPLE)

find_library(APP_KIT AppKit)
if (NOT APP_KIT)
    message(FATAL_ERROR "AppKit not found")
endif()
find_library(CORE_FOUNDATION CoreFoundation)
if (NOT CORE_FOUNDATION)
    message(FATAL_ERROR "CoreFoundation not found")
endif()
find_library(FOUNDATION Foundation)
if (NOT CORE_FOUNDATION)
    message(FATAL_ERROR "Foundation not found")
endif()
find_library(CORE_GRAPHICS CoreGraphics)
if (NOT CORE_GRAPHICS)
    message(FATAL_ERROR "CoreGraphics not found")
endif()
find_library(IO_KIT IOKit)
if (NOT IO_KIT)
    message(FATAL_ERROR "IOKit not found")
endif()

target_link_libraries(editor ${APP_KIT})
target_link_libraries(editor ${CORE_FOUNDATION})
target_link_libraries(editor ${FOUNDATION})
target_link_libraries(editor ${CORE_GRAPHICS})
target_link_libraries(editor ${IO_KIT})

endif() # APPLE

