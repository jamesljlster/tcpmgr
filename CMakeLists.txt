cmake_minimum_required(VERSION 2.8)

# Environment variables
set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/install" CACHE PATH "Installation directory")
set(BUILD_TEST OFF CACHE BOOL "Enable building test")
set(BUILD_CS_LIB OFF CACHE BOOL "Build library for Visual C# only")

set(VERSION_MAJOR 0)
set(VERSION_MINOR 1)
set(VERSION_PATCH 0)

# Library paths
set(LIB_PATHS ${CMAKE_CURRENT_SOURCE_DIR}/lib)

# Utility paths
if(${BUILD_TEST})
	set(UTIL_PATHS ${UTIL_PATHS} ${CMAKE_CURRENT_SOURCE_DIR}/test)
endif()

# If build C#
if(${BUILD_CS_LIB})
	set(LIB_PATHS ${CMAKE_CURRENT_SOURCE_DIR}/cs/lib)

	if(${BUILD_TEST})
		set(UTIL_PATHS ${CMAKE_CURRENT_SOURCE_DIR}/cs/test)
	endif()
endif()

project(tcpmgr C)

# Include subdirectory
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/doc)

# Include cmake filess
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/lib.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/util.cmake)