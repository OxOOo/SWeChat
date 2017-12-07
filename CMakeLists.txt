project(s_we_chat)
cmake_minimum_required(VERSION 2.8.11)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

# Qt5
find_package(Qt5Core REQUIRED)
include_directories(${Qt5Widgets_INCLUDE_DIRS})
find_package(Qt5Widgets REQUIRED)
include_directories(${Qt5Widgets_INCLUDE_DIRS})

# pthread
find_package(Threads)

add_subdirectory(json11)
include_directories(plog/include)
include_directories(json11)
add_compile_options(--std=c++11 -fPIC)

set(MODULES tcp chat)
set(CPP_SOURCES )

foreach(M ${MODULES})
file(GLOB_RECURSE SOURCES src/${M}/*.cpp)
set(CPP_SOURCES ${CPP_SOURCES} ${SOURCES})
endforeach()
include_directories(src)

add_executable(server src/server.cpp ${CPP_SOURCES})
target_link_libraries(server json11 ${CMAKE_THREAD_LIBS_INIT})

file(GLOB_RECURSE GUI_SOURCES src/gui/*.cpp)
add_executable(client src/client.cpp ${CPP_SOURCES} ${GUI_SOURCES})
target_link_libraries(client json11 Qt5::Core Qt5::Widgets ${CMAKE_THREAD_LIBS_INIT})