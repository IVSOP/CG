cmake_minimum_required(VERSION 3.5)
project(CG)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_STANDARD_REQUIRED true)


if (WIN32)

	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}")

    cmake_policy(SET CMP0072 NEW)

    set(CMAKE_PREFIX_PATH "C:/Program Files (x86)/freeglut")

else (WIN32)
	set(CMAKE_CXX_FLAGS_DEBUG "${GCC_COVERAGE_COMPILE_FLAGS} -Wno-deprecated ${CMAKE_CXX_FLAGS_DEBUG} -O0 -g3")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS} -Wno-deprecated -O2 -Wall -Wextra -pedantic")
endif(WIN32)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

find_package(OpenGL REQUIRED)
include_directories(${OpenGL_INCLUDE_DIRS})
link_directories(${OpenGL_LIBRARY_DIRS})
add_definitions(${OpenGL_DEFINITIONS})

if(NOT OPENGL_FOUND)
    message(ERROR " OPENGL not found!")
endif(NOT OPENGL_FOUND)

if  (WIN32)

    message(STATUS "Toolkits_DIR set to: " ${TOOLKITS_FOLDER})
    set(TOOLKITS_FOLDER "" CACHE PATH "Path to Toolkits folder")

    if (NOT EXISTS "${TOOLKITS_FOLDER}/glut/GL/glut.h" OR NOT EXISTS "${TOOLKITS_FOLDER}/glut/glut32.lib")
        message(ERROR ": GLUT not found")
    endif (NOT EXISTS "${TOOLKITS_FOLDER}/glut/GL/glut.h" OR NOT EXISTS "${TOOLKITS_FOLDER}/glut/glut32.lib")


    include_directories(${TOOLKITS_FOLDER}/glut )
    link_libraries(${OPENGL_LIBRARIES}
            ${TOOLKITS_FOLDER}/glut/glut32.lib)

    if (EXISTS "${TOOLKITS_FOLDER}/glut/glut32.dll" )
        file(COPY ${TOOLKITS_FOLDER}/glut/glut32.dll DESTINATION ${CMAKE_BINARY_DIR})
    endif(EXISTS "${TOOLKITS_FOLDER}/glut/glut32.dll" )

    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})

else (WIN32) #Linux and Mac

    find_package(GLUT REQUIRED)
    include_directories(${GLUT_INCLUDE_DIR})
    link_directories(${GLUT_LIBRARY_DIRS})
    add_definitions(${GLUT_DEFINITIONS})

    link_libraries(${OPENGL_LIBRARIES} ${GLUT_LIBRARIES} )
    if(NOT GLUT_FOUND)
        message(ERROR ": GLUT not found!")
    endif(NOT GLUT_FOUND)

endif(WIN32)

add_subdirectory(external/glm)
include_directories(external/glm)
# link_libraries(glm::glm)

add_executable(Engine
        src/Engine/Engine.cpp
		src/Engine/Renderer.cpp
		src/Engine/Camera.cpp
        src/TinyXML2/tinyxml2.cpp
        src/common/Vertex.cpp)

target_include_directories(Engine
		PUBLIC includes/Engine
        PUBLIC includes/TinyXML2
        PUBLIC includes/common)

add_executable(Generator
        src/Generator.cpp
        src/generator/Plane.cpp
        src/generator/Box.cpp
        src/generator/Sphere.cpp
        src/generator/Cone.cpp
        src/common/Vertex.cpp
        src/common/Consts.cpp)

target_include_directories(Generator
        PUBLIC includes/generator
        PUBLIC includes/common)

