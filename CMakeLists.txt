cmake_minimum_required(VERSION 3.15.0)
project(KobeBryant LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED 23)

add_compile_options(
    /EHa
    /utf-8
    -DNDEBUG
    /O2
)

add_compile_definitions(
    NOMINMAX
    UNICODE
    KobeBryant_EXPORTS
)

file(GLOB_RECURSE SOURCES "src/*.cpp")

set(RESOURCE_FILE ${PROJECT_SOURCE_DIR}/src/resource.rc)

include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/include/third)
include_directories(${PROJECT_SOURCE_DIR}/src)
include_directories(${PROJECT_SOURCE_DIR}/third)

add_executable(${PROJECT_NAME} ${SOURCES} ${RESOURCE_FILE})

target_link_libraries(${PROJECT_NAME} PRIVATE
    ${PROJECT_SOURCE_DIR}/lib/fmt/fmt.lib
    ${PROJECT_SOURCE_DIR}/lib/LightWebSocketClient/LightWSClient.lib
)

# copy target EXE to output(bin) directory
add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMENT "Copying and packing all build files to ${PROJECT_SOURCE_DIR}/output directory"
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${PROJECT_SOURCE_DIR}/output
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${PROJECT_NAME}> ${PROJECT_SOURCE_DIR}/output/${PROJECT_NAME}.exe
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_SOURCE_DIR}/include ${PROJECT_SOURCE_DIR}/output/sdk/include
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_SOURCE_DIR}/lib ${PROJECT_SOURCE_DIR}/output/sdk/lib
    COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/build/MinSizeRel/${PROJECT_NAME}.lib ${PROJECT_SOURCE_DIR}/output/sdk/lib/${PROJECT_NAME}.lib
    VERBATIM
)