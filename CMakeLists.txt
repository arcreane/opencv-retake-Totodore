include(FindOpenGL)

cmake_minimum_required(VERSION 3.12)                            
project(scanner)

set(CMAKE_CXX_STANDARD 17)
set(OpenCV_DIR vendor/opencv/build)

# OpenCV package
find_package(OpenCV REQUIRED)
configure_file("${_OpenCV_LIB_PATH}/opencv_world470d.dll" "${CMAKE_CURRENT_BINARY_DIR}/Debug" COPYONLY)
configure_file("${_OpenCV_LIB_PATH}/opencv_world470d.dll" "${CMAKE_CURRENT_BINARY_DIR}/Release" COPYONLY)
configure_file("${_OpenCV_LIB_PATH}/opencv_world470.dll" "${CMAKE_CURRENT_BINARY_DIR}/Debug" COPYONLY)
configure_file("${_OpenCV_LIB_PATH}/opencv_world470.dll" "${CMAKE_CURRENT_BINARY_DIR}/Release" COPYONLY)

# glfw package
add_subdirectory(vendor/glfw)


# imgui config
file(GLOB IMGUI_SOURCES 
	vendor/imgui/*.cpp 
	vendor/imgui/backends/imgui_impl_glfw.cpp 
	vendor/imgui/backends/imgui_impl_opengl3.cpp
	vendor/imgui_file_dialog/ImGuiFileDialog.cpp)

file(GLOB IMGUI_HEADERS 
	vendor/imgui/*.h 
	vendor/imgui/backends/imgui_impl_glfw.h
	vendor/imgui/backends/imgui_impl_opengl3.h
	vendor/imgui_file_dialog/ImGuiFileDialog.h)

add_library(imgui STATIC ${IMGUI_SOURCES})
target_include_directories(imgui PUBLIC vendor/imgui vendor/imgui/backends vendor/imgui_file_dialog)
target_link_libraries(imgui PUBLIC glfw)

# OpenGL impl package
find_package(OpenGL REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS} vendor/glfw/include ${IMGUI_HEADERS})
link_directories(${OpenCV_LIB_DIR})
link_libraries(${OpenCV_LIBS} imgui glfw ${OPENGL_LIBRARIES})


# Assets config
file(COPY assets DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/Debug)
# file(COPY assets DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/Release)
# Executable config

file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE HEADERS "src/*.hpp")

# print runtime output directory
message(STATUS "Runtime output directory: ${EXECUTABLE_OUTPUT_PATH}")

add_executable(main ${SOURCES} ${HEADERS})