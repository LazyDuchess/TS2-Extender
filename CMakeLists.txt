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

file(GLOB_RECURSE SOURCES_SHARED CONFIGURE_DEPENDS "src/*.cpp")
file(GLOB_RECURSE HEADERS_SHARED CONFIGURE_DEPENDS "include/*.h")

file(GLOB_RECURSE SOURCES_LC CONFIGURE_DEPENDS "src_lc/*.cpp")
file(GLOB_RECURSE SOURCES_UC CONFIGURE_DEPENDS "src_uc/*.cpp")

file(GLOB_RECURSE HEADERS_LC CONFIGURE_DEPENDS "include_lc/*.h")
file(GLOB_RECURSE HEADERS_UC CONFIGURE_DEPENDS "include_uc/*.h")

function(setup_ts2extender_target target_name compile_defs sources headers include_dir)
    add_library(${target_name} SHARED ${SOURCES_SHARED} ${sources} ${HEADERS_SHARED} ${headers})

    target_include_directories(${target_name} PRIVATE 
	include
	"${include_dir}"
	minhook/include
	thirdparty/mINI/src/mini
    )

    target_link_libraries(${target_name} PRIVATE minhook)

    set_target_properties(${target_name} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
        SUFFIX ".asi"
    )

    target_compile_definitions(${target_name} PRIVATE ${compile_defs})
endfunction()

# Legacy Collection build (Standard)
setup_ts2extender_target(TS2Extender "TS2_LC" "${SOURCES_LC}" "${HEADERS_LC}" include_lc)

# Ultimate Collection build
setup_ts2extender_target(TS2ExtenderUC "TS2_UC" "${SOURCES_UC}" "${HEADERS_UC}" include_uc)