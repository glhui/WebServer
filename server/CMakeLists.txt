add_subdirectory(base)

file(GLOB_RECURSE SOURCES "src/*.cpp" "src/*.h")

# add_executable(server ${SOURCES})

# target_include_directories(server
#         PRIVATE
#         ${CMAKE_CURRENT_SOURCE_DIR}/include
# )

# target_link_libraries(server
#         PRIVATE
#         libserver_base
# )