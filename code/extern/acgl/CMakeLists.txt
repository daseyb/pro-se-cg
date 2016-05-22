#
# Use this to compile ACGL as a library. To include it statically compiled into 
# your own project you can also just include the CMakeListsStaticInclude.txt:
# INCLUDE(${CMAKE_CURRENT_SOURCE_DIR}/CMakeListsStaticInclude.txt)
#
CMAKE_MINIMUM_REQUIRED (VERSION 2.6) 

# include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/CMakeMacros.txt)

################################################################################
# Global Settings
################################################################################

# Name of the Project 
PROJECT(ACGL)
# (${CMAKE_CURRENT_SOURCE_DIR}/cmake/CompileOptions.txt)

# Specify dependencies
SET(NEED_ACGL_GLOBAL_EXTERN_DIR TRUE)
# INCLUDE(${CMAKE_CURRENT_SOURCE_DIR}/cmake/GlobalAndLocalExternACGL.txt)

# OpenGL Support
IF(NOT DEFINED ACGL_OPENGL_SUPPORT)
    SET(ACGL_OPENGL_SUPPORT CORE_42 CACHE STRING "")
    SET_PROPERTY(CACHE ACGL_OPENGL_SUPPORT PROPERTY STRINGS CORE_32 CORE_33 CORE_40 CORE_41 CORE_42 CORE_43 CORE_42 FULL_21 FULL_30 FULL_31 FULL_32 FULL_33 FULL_40 FULL_41 FULL_42 FULL_43 ES_20)
ENDIF()

# Error Level
IF(NOT DEFINED ACGL_ERROR_LEVEL)
    SET(ACGL_ERROR_LEVEL EC0 CACHE STRING "")
    SET_PROPERTY(CACHE ACGL_ERROR_LEVEL PROPERTY STRINGS EC0 EC1 EC2 EC3)
ENDIF()

# UseQT
IF(NOT DEFINED ACGL_COMPILE_WITH_QT)
    SET(ACGL_COMPILE_WITH_QT No CACHE STRING "")
    SET_PROPERTY(CACHE ACGL_COMPILE_WITH_QT PROPERTY STRINGS Yes No)
ENDIF()

################################################################################
# Basic Configuration
################################################################################

# Where to find the cmake finders?
SET(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})

# Target directory
SET(LIBRARY_OUTPUT_PATH "${CMAKE_CURRENT_SOURCE_DIR}/lib")

################################################################################
# Project Files
################################################################################

#
# if ACGL should not get compiled as a library, just include the following file 
# into the projects CMakeList.txt
#
INCLUDE(${CMAKE_CURRENT_SOURCE_DIR}/CMakeListsStaticInclude.txt)

################################################################################
# Defines
################################################################################

# INCLUDE(${CMAKE_CURRENT_SOURCE_DIR}/cmake/DefinesACGL.txt)
ADD_DEFINITIONS(${ACGL_DEFINES})

################################################################################
# External Libraries
################################################################################

# Qt
IF(ACGL_COMPILE_WITH_QT)
    MESSAGE("QT")
    SET(QT_USE_QTOPENGL TRUE)
    FIND_PACKAGE(Qt4 REQUIRED)
    INCLUDE(${QT_USE_FILE})
    SET(LIBRARIES ${LIBRARIES} ${QT_LIBRARIES})
ENDIF()

# static linking for MSVC
IF(ACGL_STATIC_RUNTIME_ENV)
if(MSVC)
        foreach(flag
                CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE
                CMAKE_CXX_FLAGS_MINSIZEREL CMAKE_CXX_FLAGS_RELWITHDEBINFO)
            if(${flag} MATCHES "/MD")
                string(REGEX REPLACE "/MD" "/MT" ${flag} "${${flag}}")
            endif()
        endforeach()
    endif()
ENDIF()

################################################################################
# Final Definition
################################################################################

if("\"${CMAKE_CXX_COMPILER_ID}\"" STREQUAL "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -stdlib=libc++")
elseif(CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
elseif(MSVC)
endif()

# Define the Project, add all required sources and dependencies to it
ADD_LIBRARY(ACGL${COMPILE_POSTFIX} STATIC ${SOURCE_FILES} ${HEADER_FILES})
TARGET_LINK_LIBRARIES(ACGL${COMPILE_POSTFIX} ${LIBRARIES})
#MESSAGE(${LIBRARY_NAME})
