cmake_minimum_required(VERSION 3.31)

project(robots_vs_aliens)

set(TEMPLATE_HTML "${CMAKE_SOURCE_DIR}/template.html")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -s USE_GLFW=3 -s ASSERTIONS=1 -s WASM=1 -s ASYNCIFY -s GL_ENABLE_GET_PROC_ADDRESS=1")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -s USE_GLFW=3 -s ASSERTIONS=1 -s WASM=1 -s ASYNCIFY -s GL_ENABLE_GET_PROC_ADDRESS=1 -s EXPORTED_RUNTIME_METHODS=['HEAPF32','requestFullscreen'] --preload-file assets --shell-file ${TEMPLATE_HTML}")
set(CMAKE_EXECUTABLE_SUFFIX ".html") # This line is used to set your executable to build with the emscripten html template so that you can directly open it.

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_DEPFILE_FLAGS_CXX "")
  
add_custom_target(copy_resources ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/assets ${CMAKE_BINARY_DIR}/assets
    COMMENT "Copying resource folders to build directory"
)

include_directories(
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/deps
    ${CMAKE_SOURCE_DIR}/deps/libtextureatlas/include
    ${CMAKE_SOURCE_DIR}/deps/raygui
    ${CMAKE_SOURCE_DIR}/deps/raylib/include
)

file(GLOB_RECURSE SOURCES 
    "${CMAKE_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_SOURCE_DIR}/deps/*.c"
)

add_executable(robots_vs_aliens ${SOURCES})
add_dependencies(robots_vs_aliens copy_resources)

target_link_libraries(robots_vs_aliens PRIVATE ${CMAKE_SOURCE_DIR}/deps/raylib/lib/libraylib.a)
