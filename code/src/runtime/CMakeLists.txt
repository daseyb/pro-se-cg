cmake_minimum_required (VERSION 3.0)
project(ORION C CXX)

include_directories(${DEPENDENCY_DIRS} ${CMAKE_CURRENT_SOURCE_DIR}/include)

MACRO (RECURSE_GROUPS dirname)
    FILE(GLOB_RECURSE allfiles "${dirname}/*.*")
    SET(ALL_FILES ${ALL_FILES} ${allfiles})
    STRING(REGEX REPLACE ".*/([^/]*)" "\\1" maindir "${dirname}")

    FOREACH(file ${allfiles})
        STRING(REGEX REPLACE "${dirname}/(.*)/[^/]*" "\\1" loopdirname "${file}")
        STRING(REGEX REPLACE "/" "\\\\" loopdirname "${loopdirname}")

        IF(NOT "${file}" MATCHES "${dirname}/(.*)/[^/]*")
            source_group("${maindir}" FILES  ${file})
        ELSE()
            source_group("${maindir}\\${loopdirname}" FILES  ${file})
        ENDIF()
    ENDFOREACH()
ENDMACRO (RECURSE_GROUPS)

SET(RUNTIME_BASE_DIR "${CMAKE_CURRENT_LIST_DIR}")
# Find all used files of certain file-types
FILE(GLOB_RECURSE SOURCE_FILES_C   "${RUNTIME_BASE_DIR}/src/*.c")
FILE(GLOB_RECURSE SOURCE_FILES_CC  "${RUNTIME_BASE_DIR}/src/*.cc")
FILE(GLOB_RECURSE SOURCE_FILES_CPP "${RUNTIME_BASE_DIR}/src/*.cpp")
FILE(GLOB_RECURSE HEADER_FILES_H   "${RUNTIME_BASE_DIR}/include/*.h")
FILE(GLOB_RECURSE HEADER_FILES_HH  "${RUNTIME_BASE_DIR}/include/*.hh")
FILE(GLOB_RECURSE HEADER_FILES_HPP "${RUNTIME_BASE_DIR}/include/*.hpp")
#group files in msvc
RECURSE_GROUPS ("${RUNTIME_BASE_DIR}/include")
RECURSE_GROUPS ("${RUNTIME_BASE_DIR}/src")

SET(HEADER_FILES ${HEADER_FILES} ${HEADER_FILES_H} ${HEADER_FILES_HH} ${HEADER_FILES_HPP})
SET(SOURCE_FILES ${SOURCE_FILES} ${SOURCE_FILES_C} ${SOURCE_FILES_CC} ${SOURCE_FILES_CPP})

add_executable(orion ${SOURCE_FILES} ${HEADER_FILES})
target_link_libraries(orion ${LIBRARIES})

add_custom_command(TARGET orion POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different  ${DLLS} $<TARGET_FILE_DIR:orion>)
