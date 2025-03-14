cmake_minimum_required(VERSION 3.10)

project(TS2Extender LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include(FetchContent)

FetchContent_Declare(
  MinHook
  GIT_REPOSITORY https://github.com/TsudaKageyu/minhook.git
  GIT_TAG        master
)

FetchContent_MakeAvailable(MinHook)

file(GLOB_RECURSE SOURCES "src/*.cpp" "src/*/*.cpp")
file(GLOB_RECURSE HEADERS "include/*.h" "include/*/*.h")

function(setup_ts2extender_target target_name compile_defs)
    add_library(${target_name} SHARED ${SOURCES} ${HEADERS})

    target_include_directories(${target_name} PRIVATE include minhook/include)
    target_include_directories(${target_name} PRIVATE include thirdparty/mINI/src/mini)

    target_link_libraries(${target_name} PRIVATE minhook)

    set_target_properties(${target_name} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
        SUFFIX ".asi"
    )

    target_compile_definitions(${target_name} PRIVATE ${compile_defs})
endfunction()

setup_ts2extender_target(TS2Extender "TS2EXTENDER_EXPORTS")

setup_ts2extender_target(TS2ExtenderConsole "TS2EXTENDER_EXPORTS;FORCE_CONSOLE")