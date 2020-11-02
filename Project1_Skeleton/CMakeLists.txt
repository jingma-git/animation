cmake_minimum_required(VERSION 3.10.0)
project(skeleton)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules/")

if (POLICY CMP0072)
  set(OpenGL_GL_PREFERENCE LEGACY)
endif()

# find the required packages
find_package(Eigen3 REQUIRED)
message(STATUS "EIGEN3 included at ${EIGEN3_INCLUDE_DIR}")
find_package(GLM REQUIRED)
message(STATUS "GLM included at ${GLM_INCLUDE_DIR}")
find_package(GLFW3 REQUIRED)
message("-- GLFW3 LIB @ ${GLFW3_LIBRARY}")
message(STATUS "Found GLFW3 in ${GLFW3_INCLUDE_DIR}")
# find_package(GLEW REQUIRED)
# message("-- GLEW LIB @ ${GLEW_LIBRARIES}")
message(STATUS "Found GLEW in ${GLEW_INCLUDE_DIR}")
find_package(OpenMP)

add_library(GLAD "src/glad.c")

include_directories(${CMAKE_SOURCE_DIR}/includes)
include_directories(./src/)

file(GLOB SRC main.cpp src/*.cpp src/*.h)
add_executable(main ${SRC})
target_link_libraries(main ${GLFW3_LIBRARY} glut GL GLAD OpenMP::OpenMP_CXX dl) # make sure dl is added at last



