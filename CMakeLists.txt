project(s_we_chat)
cmake_minimum_required(VERSION 2.7)

add_subdirectory(json11)
include_directories(plog/include)
include_directories(json11)
add_compile_options(--std=c++11 -lpthread)

set(MODULES tcp)
set(CPP_SOURCES )

foreach(M ${MODULES})
file(GLOB_RECURSE SOURCES src/${M}/*.cpp)
set(CPP_SOURCES ${CPP_SOURCES} ${SOURCES})
endforeach()

add_executable(test src/test.cpp ${CPP_SOURCES})
target_link_libraries(test json11)

add_executable(server src/server.cpp ${CPP_SOURCES})
target_link_libraries(server json11)

add_executable(client src/client.cpp ${CPP_SOURCES})
target_link_libraries(client json11)