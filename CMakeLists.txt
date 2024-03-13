cmake_minimum_required(VERSION 3.22.1)
project(CG)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_STANDARD_REQUIRED true)


if (WIN32)

	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}")

	cmake_policy(SET CMP0072 NEW)

else (WIN32)
	set(CMAKE_CXX_FLAGS_DEBUG "${GCC_COVERAGE_COMPILE_FLAGS} -Wno-deprecated ${CMAKE_CXX_FLAGS_DEBUG} -O0 -g3")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS} -Wno-deprecated -O2 -Wall -Wextra -pedantic")
endif(WIN32)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

find_package(OpenGL REQUIRED)

if (WIN32)

	message(STATUS "Toolkits_DIR set to: " ${TOOLKITS_FOLDER})
	set(TOOLKITS_FOLDER "" CACHE PATH "Path to Toolkits folder")

	set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})

	if (NOT EXISTS "${TOOLKITS_FOLDER}/glut/GL/glut.h" OR NOT EXISTS "${TOOLKITS_FOLDER}/glut/glut32.lib")
		message(ERROR ": GLUT not found")
	endif (NOT EXISTS "${TOOLKITS_FOLDER}/glut/GL/glut.h" OR NOT EXISTS "${TOOLKITS_FOLDER}/glut/glut32.lib")

	if (NOT EXISTS "${TOOLKITS_FOLDER}/glew/GL/glew.h" OR NOT EXISTS "${TOOLKITS_FOLDER}/glew/glew32.lib")
		message(ERROR ": GLEW not found")
	endif (NOT EXISTS "${TOOLKITS_FOLDER}/glew/GL/glew.h" OR NOT EXISTS "${TOOLKITS_FOLDER}/glew/glew32.lib")	


	include_directories(${TOOLKITS_FOLDER}/glut ${TOOLKITS_FOLDER}/glew)

	if (EXISTS "${TOOLKITS_FOLDER}/glut/glut32.dll"  AND EXISTS "${TOOLKITS_FOLDER}/glew/glew32.dll")
		file(COPY ${TOOLKITS_FOLDER}/glut/glut32.dll DESTINATION ${CMAKE_BINARY_DIR})
		file(COPY ${TOOLKITS_FOLDER}/glew/glew32.dll DESTINATION ${CMAKE_BINARY_DIR})
	endif(EXISTS "${TOOLKITS_FOLDER}/glut/glut32.dll"  AND EXISTS "${TOOLKITS_FOLDER}/glew/glew32.dll")	

else (WIN32)
	find_package(GLEW REQUIRED)
endif (WIN32)


add_subdirectory(external/glm)
include_directories(external/glm)

add_subdirectory(external/glfw)
include_directories(external/glfw/include)

include_directories(external/imgui)
include_directories(external/imgui/backends)

add_executable(Engine
		src/Engine/Engine.cpp
		src/Engine/Renderer.cpp
		src/Engine/Camera.cpp
		src/Engine/InputHandler.cpp
		src/Engine/XmlParser.cpp
		src/Engine/Engine_Object.cpp
		src/Engine/Translate.cpp
		src/Engine/Rotate.cpp
		src/Engine/Scale.cpp
		src/Engine/Transformation.cpp
		src/Engine/GLErrors.cpp
		src/TinyXML2/tinyxml2.cpp
		src/common/Vertex.cpp
		src/common/Consts.cpp
		src/windows/windows_unistd.cpp

		external/imgui/imgui.cpp
		external/imgui/imgui_demo.cpp
		external/imgui/imgui_draw.cpp
		external/imgui/imgui_tables.cpp
		external/imgui/imgui_widgets.cpp
		external/imgui/backends/imgui_impl_glfw.cpp
		external/imgui/backends/imgui_impl_opengl3.cpp
)

target_include_directories(Engine
		PUBLIC includes/Engine
		PUBLIC includes/TinyXML2
		PUBLIC includes/common
		PUBLIC includes/windows
)

add_executable(Generator
		src/generator/Generator.cpp
		src/generator/Plane.cpp
		src/generator/Box.cpp
		src/generator/Cylinder.cpp
		src/generator/Sphere.cpp
		src/generator/Cone.cpp
		src/generator/Cylinder.cpp
		src/generator/Torus.cpp
		src/common/Vertex.cpp
		src/common/Consts.cpp
)

target_include_directories(Generator
		PUBLIC includes/generator
		PUBLIC includes/common
)

if (WIN32) # windows glfw, glew, glut, opengl mas de maneira manhosa
	target_link_libraries(Engine
		glfw
		# GLEW::GLEW
		# OpenGL::GL
		${OPENGL_LIBRARIES} 
		${TOOLKITS_FOLDER}/glut/glut32.lib 
		${TOOLKITS_FOLDER}/glew/glew32.lib
	)
else (WIN32)
	target_link_libraries(Engine
			glfw
			GLEW::GLEW
			OpenGL::GL
	)
endif (WIN32)
# ?????? o glm funciona por magia sem linking nenhum????? e usar so link_libraries nao funcionou, isto ja me ultrapassa
