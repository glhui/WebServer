project(libserver_base)

file(GLOB_RECURSE SOURCES "src/*.cpp" "src/*.h")

add_library(libserver_base ${SOURCES})

target_link_libraries(libserver_base)
set_target_properties(libserver_base PROPERTIES OUTPUT_NAME "server_base")

target_include_directories(libserver_base
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

option(ENABLE_TESTS "Enable building tests" ON)

if (ENABLE_TESTS)
    add_subdirectory(tests)
endif()