cmake_minimum_required(VERSION 3.8)

set(CMAKE_CUDA_ARCHITECTURES 61)
include(CheckLanguage)

project(screen-share)
set(PROJECT_VERSION 0.0.2)

find_package(SDL2 REQUIRED)
include_directories(screen-share ${SDL2_INCLUDE_DIRS})

set(CMAKE_C_STANDARD 11)

include_directories(screen-share ${CMAKE_CURRENT_SOURCE_DIR}/include)

if (CMAKE_CUDA_COMPILER)
	enable_language(CUDA)
	aux_source_directory(cuda CU_SOURCES)
else()
	message(WARNING "No CUDA compiler found, CUDA support disabled")
	aux_source_directory(cpu CU_SOURCES)
endif()

# Add the agnostic sources
aux_source_directory(src SOURCES)

if (WIN32)
	aux_source_directory(windows OS_SOURCES)
elseif (UNIX)
	aux_source_directory(linux OS_SOURCES)
	find_package(X11 REQUIRED)
	include_directories(screen-share ${X11_INCLUDE_DIR})
	link_libraries(${X11_LIBRARIES})
	set(CMAKE_C_COMPILER gcc)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c11 -Wall -Wextra -Wpedantic -Wno-comment -fno-trapping-math -fno-math-errno -fno-signed-zeros -march=native -falign-functions=16")
	set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O3")
	set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -O0 -g")
else()
	message(FATAL_ERROR "Unsupported platform")
endif()

link_libraries(${SDL2_LIBRARIES})

add_executable(client client.c ${SOURCES} ${CU_SOURCES} ${OS_SOURCES})

aux_source_directory(server SERVER_SOURCES)
# add_executable(server server.c src/crypto.c src/protocol.c linux/net.c ${SERVER_SOURCES})
