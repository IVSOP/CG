cmake_minimum_required(VERSION 3.5)
project(CG)

set(CMAKE_CXX_STANDARD  20)

set(CMAKE_CXX_STANDARD_REQUIRED true)

set(GCC_COVERAGE_COMPILE_FLAGS "-O2 -Wall -Wextra -std=c++17")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}")

cmake_policy(SET CMP0072 NEW)
set(OpenGL_GL_PREFERENCE "GLVND")

set(CMAKE_PREFIX_PATH "C:/Program Files (x86)/freeglut")

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

find_package(OpenGL REQUIRED)
include_directories(${OpenGL_INCLUDE_DIRS})
link_directories(${OpenGL_LIBRARY_DIRS})
add_definitions(${OpenGL_DEFINITIONS})

if(NOT OPENGL_FOUND)
    message(ERROR " OPENGL not found!")
endif(NOT OPENGL_FOUND)

add_executable(Engine
        src/engine.cpp
        src/TinyXML2/tinyxml2.cpp
)

target_include_directories(Engine
        PUBLIC includes/TinyXML2)

set( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} Wno-deprecated")
set(CMAKE_PREFIX_PATH "/usr/include/GL") #[[Added by me]]
find_package(GLUT REQUIRED)
include_directories(${GLUT_INCLUDE_DIR})
link_directories(${GLUT_LIBRARY_DIRS})
add_definitions(${GLUT_DEFINITIONS})

target_link_libraries(Engine ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES} )
if(NOT GLUT_FOUND)
    message(ERROR ": GLUT not found!")
endif(NOT GLUT_FOUND)

