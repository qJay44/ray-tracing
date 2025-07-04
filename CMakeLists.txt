include(./.env.cmake OPTIONAL RESULT_VARIABLE LOCAL_ENV)
message(STATUS "Local .env.cmake: ${LOCAL_ENV}")

cmake_minimum_required(VERSION 3.21)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Run)

project(MyProject VERSION 1.0)

file(GLOB_RECURSE SOURCES
  ${PROJECT_SOURCE_DIR}/src/*.cpp
  ${UTILS_PATH}/src/*.cpp)

# Using mannually built imgui with GLFW (copying dll)
set(GLFW_IMGUI_LIB_FILE libGLFW_IMGUI.dll)
add_custom_command(
  OUTPUT ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${GLFW_IMGUI_LIB_FILE}
  COMMAND ${CMAKE_COMMAND} -E copy
    ${IMGUI_PATH}/Build/Release/${GLFW_IMGUI_LIB_FILE}
    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${GLFW_IMGUI_LIB_FILE}
  DEPENDS ${IMGUI_PATH}/Build/Release/${GLFW_IMGUI_LIB_FILE}
)

# Using mannually built GLAD (copying dll)
set(GLAD_LIB_FILE libGLAD.dll)
add_custom_command(
  OUTPUT ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${GLAD_LIB_FILE}
  COMMAND ${CMAKE_COMMAND} -E copy
    ${GLAD_PATH}/Build/Release/${GLAD_LIB_FILE}
    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${GLAD_LIB_FILE}
  DEPENDS ${GLAD_PATH}/Build/Release/${GLAD_LIB_FILE}
)

# Using mannually built libtiff (copying dll)
set(LIBTIFF_LIB_FILE libtiff.dll)
add_custom_command(
  OUTPUT ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${LIBTIFF_LIB_FILE}
  COMMAND ${CMAKE_COMMAND} -E copy
    ${LIBTIFF_PATH}/bin/${LIBTIFF_LIB_FILE}
    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${LIBTIFF_LIB_FILE}
  DEPENDS ${LIBTIFF_PATH}/bin/${LIBTIFF_LIB_FILE}
)

add_executable(${PROJECT_NAME}
  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${GLFW_IMGUI_LIB_FILE}
  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${GLAD_LIB_FILE}
  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${LIBTIFF_LIB_FILE}
  ${SOURCES})

target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Werror)
target_compile_options(${PROJECT_NAME} PRIVATE -isystem ${GENERAL_INCLUDES}/stb)

target_precompile_headers(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/src/pch.hpp)

target_include_directories(${PROJECT_NAME} PUBLIC
  ${GENERAL_INCLUDES}
  ${GLAD_PATH}/include
  ${GLFW_PATH}/include
  ${GLM_PATH}/include
  ${IMGUI_PATH}/include
  ${UTILS_PATH}/include
  ${LIBTIFF_PATH}/include
)

target_link_directories(${PROJECT_NAME} PUBLIC
  ${GLM_PATH}/bin
  ${GLFW_PATH}/lib-mingw-w64
  ${GLM_PATH}/lib
  ${IMGUI_PATH}/Build/Release
  ${GLAD_PATH}/Build/Release
  ${LIBTIFF_PATH}/lib
)

target_link_libraries(${PROJECT_NAME} libGLFW_IMGUI libGLAD libtiff glm)

