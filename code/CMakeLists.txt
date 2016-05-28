cmake_minimum_required (VERSION 3.0)
project(ORION C CXX)

enable_testing()

if(MSVC)
  # Force to always compile with W4
  if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
    string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
  else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
  endif()  
elseif(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
  # Update if necessary
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1y -Wall -Wno-long-long -Wno-reorder -pedantic")
endif()


set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED on)

find_package(OpenGL)
if (OPENGL_FOUND)
  MESSAGE("OpenGL Correctly Found")
    include_directories(${OPENGL_INCLUDE_DIR})
  else (OPENGL_FOUND)
    MESSAGE("OpenGL environment missing")
endif (OPENGL_FOUND)

add_subdirectory(extern/glow)
include_directories(${GLOW_INCLUDE_DIRECTORIES})


set(SDL_STATIC ON)
set(SDL_SHARED OFF)
set(DIRECTX NO)
set(RENDER_D3D NO)
set(SDL_AUDIO NO)
set(DISKAUDIO NO)
set(DUMMYAUDIO NO)
add_subdirectory(extern/sdl2)
include_directories(extern/sdl2/include)

include(extern/fmod/CMakeLists.txt)
include_directories(extern/fmod/include)
MESSAGE(${FMOD_LIBRARIES})

set(DLLS ${FMOD_DLLS})

add_subdirectory(extern/portmidi)
include_directories(extern/portmidi/pm_common)

set(LIBRARIES glow SDL2-static SDL2main portmidi-static ${OPENGL_LIBRARIES} ${FMOD_LIBRARIES})

#
# MacOS X:
IF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  set(LIBRARIES ${LIBRARIES} -Wl,-framework,Cocoa -Wl,-framework,OpenGL,-framework,IOKit)
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")

#
# Linux:
IF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    set(LIBRARIES ${LIBRARIES} -lXrandr -lGL -lXi  -pthread -lm -lX11 -lXxf86vm)
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")

add_subdirectory(src/runtime)
