cmake_minimum_required(VERSION 2.8)
PROJECT(GPGPU_TD1)

#OpenGL
find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIR})
link_directories(${OpenGL_LIBRARY_DIRS})
add_definitions(${OpenGL_DEFINITIONS})

#GLUT
find_package(GLUT REQUIRED)
include_directories(${GLUT_INCLUDE_DIRS})
link_directories(${GLUT_LIBRARY_DIRS})
add_definitions(${GLUT_DEFINITIONS})
if(NOT GLUT_FOUND)
    message(ERROR " GLUT not found!")
endif(NOT GLUT_FOUND)

#GLEW
include_directories(glew-1.10.0/include)
add_definitions(-DGLEW_STATIC=1)
add_subdirectory(glew-1.10.0)

#Application (TD)
add_definitions(-Wall)
set(CMAKE_CXX_FLAGS_DEBUG "-DDEBUG ${CMAKE_CXX_FLAGS_DEBUG}")

set(ALL_LIBRARIES glew ${OPENGL_LIBRARIES} ${GLUT_glut_LIBRARY} jpeg rt)

add_subdirectory(src)

