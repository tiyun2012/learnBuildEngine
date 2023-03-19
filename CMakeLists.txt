cmake_minimum_required(VERSION 3.10)
project(python_interpreter)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /NODEFAULTLIB:\"MSVCRT\"")


find_package(PythonLibs REQUIRED)
find_package(OpenGL REQUIRED)
set(GLFW3_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/glfw/include) 
set(GLFW3_LIBRARY  ${PROJECT_SOURCE_DIR}/glfw/lib-vc2022/glfw3.lib)

# Set the Python root directory to the specific version
set(Python_ROOT_DIR "C:/PythonVersion/python3.10.10")

find_package(Python 3.10.10 COMPONENTS Interpreter Development REQUIRED)

if(${Python_FOUND})
  message(STATUS "Python found: ${Python_EXECUTABLE}, version ${Python_VERSION}")
else()
  message(FATAL_ERROR "Python not found")
endif()

include_directories(
    ${PROJECT_SOURCE_DIR}/imgui
    ${PROJECT_SOURCE_DIR}/include
    ${PYTHON_INCLUDE_DIRS}
    ${PROJECT_SOURCE_DIR}/include/glad
    ${PYTHON_INCLUDE_DIRS}/backends
    ${GLFW3_INCLUDE_DIR}
    ${Python_INCLUDE_DIRS}
)

file(GLOB IMGUI_SOURCE
    ${PROJECT_SOURCE_DIR}/imgui/*.cpp
    ${PROJECT_SOURCE_DIR}/imgui/backends/imgui_impl_glfw.cpp
    ${PROJECT_SOURCE_DIR}/imgui/backends/imgui_impl_opengl3.cpp
)

add_executable(python_interpreter
    src/main.cpp
    src/glad.c
    src/python_interpreter.cpp
    ${IMGUI_SOURCE}
)

target_link_libraries(python_interpreter
    ${OPENGL_gl_LIBRARY}
    ${GLFW3_LIBRARY}
    ${Python_LIBRARIES}
)
