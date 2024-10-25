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

# add_link_options(/DEBUG)
add_compile_definitions(
    NOMINMAX
    UNICODE
    KobeBryant_EXPORTS
)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

file(GLOB_RECURSE SOURCES "src/*.cpp")

set(RESOURCE_FILE ${PROJECT_SOURCE_DIR}/src/resource.rc)

include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/include/third)
include_directories(${PROJECT_SOURCE_DIR}/src)
include_directories(${PROJECT_SOURCE_DIR}/third)

add_executable(KobeBryant ${SOURCES} ${RESOURCE_FILE})

target_link_libraries(KobeBryant PRIVATE
    ${PROJECT_SOURCE_DIR}/lib/fmt/fmt.lib
    ${PROJECT_SOURCE_DIR}/lib/LightWebSocketClient/LightWSClient.lib
)