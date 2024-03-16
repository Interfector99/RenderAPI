cmake_minimum_required(VERSION 3.10)
project(RenderAPI VERSION 1.0 DESCRIPTION "A simple rendering API written in C17, using SDL2 for window management. Inspired by Pikuma's comuter graphics course.")

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED True)

include_directories(include)

# Set output directory based on OS and architecture
if(WIN32)
  set(OS_FOLDER "Windows")
else()
  if(APPLE)
    set(OS_FOLDER "macOS")
  else()
    set(OS_FOLDER "Linux")
  endif()
endif()

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
  set(ARCH_FOLDER "x64")
else()
  set(ARCH_FOLDER "x86")
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build/${OS_FOLDER}/${ARCH_FOLDER}/${CMAKE_BUILD_TYPE})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build/${OS_FOLDER}/${ARCH_FOLDER}/${CMAKE_BUILD_TYPE})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build/${OS_FOLDER}/${ARCH_FOLDER}/${CMAKE_BUILD_TYPE})

# Intermediate directory
set(CMAKE_INTERMEDIATE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build/${OS_FOLDER}/${ARCH_FOLDER}/${CMAKE_BUILD_TYPE}/intermediate)

# Add the executable
file(GLOB SOURCE_FILES src/*.c)
add_executable(RenderAPI ${SOURCE_FILES})

# If on Windows, set SDL2 paths manually
if(WIN32)
    set(SDL2_DIR "${CMAKE_SOURCE_DIR}/lib/SDL2")
    set(SDL2_INCLUDE_DIRS "${SDL2_DIR}/include")
    set(SDL2_LIBRARIES "${SDL2_DIR}/lib/${ARCH_FOLDER}/SDL2.lib" "${SDL2_DIR}/lib/${ARCH_FOLDER}/SDL2main.lib")
	
	include_directories(${SDL2_INCLUDE_DIRS})
    # SDL2.dll copy command for x64 configuration
    add_custom_command(TARGET RenderAPI POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${SDL2_DIR}/lib/${ARCH_FOLDER}/SDL2.dll" $<TARGET_FILE_DIR:RenderAPI>)
	
else()
    # For Linux and macOS, use pkg-config or find_package
    find_package(SDL2 REQUIRED)
    include_directories(${SDL2_INCLUDE_DIRS})
endif()

# Link the executable to the libraries
target_link_libraries(RenderAPI ${SDL2_LIBRARIES})
if(UNIX AND NOT APPLE)
    target_link_libraries(RenderAPI m)
endif()