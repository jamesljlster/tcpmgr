cmake_minimum_required(VERSION 2.8)

# Environment variables
set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/install" CACHE PATH "Installation directory")
set(BUILD_TEST OFF CACHE BOOL "Enable building test")

set(VERSION_MAJOR 0)
set(VERSION_MINOR 1)
set(VERSION_PATCH 0)

project(tcpmgr C)

# Include subdirectory
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/lib)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/doc)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/lib)
if(${BUILD_TEST})
	add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/test)
endif()
