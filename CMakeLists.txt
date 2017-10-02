cmake_minimum_required(VERSION 2.8)

# Environment variables
set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/install" CACHE PATH "Installation directory")
set(BUILD_TEST OFF CACHE BOOL "Enable building test")

project(tcpmgr C)

# Include subdirectory
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/tcpmgr)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/tcpmgr)
if(${BUILD_TEST})
	add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/test)
endif()
