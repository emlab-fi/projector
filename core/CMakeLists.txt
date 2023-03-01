cmake_minimum_required(VERSION 3.12)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(scan_simulator C CXX)

find_package(Boost REQUIRED COMPONENTS system program_options)


#get json lib
include(FetchContent)
FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.2/json.tar.xz)
FetchContent_MakeAvailable(json)

#get progress bar lib
FetchContent_Declare(indicators
	GIT_REPOSITORY https://github.com/p-ranav/indicators.git
	GIT_TAG 6432875ef4c866d25e324c19a787b5bb90b0ab12)
FetchContent_MakeAvailable(indicators)

set(SOURCES
	src/main.cpp
	src/geom.cpp
	src/simulator.cpp
	src/loader.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES})

set_target_properties(${PROJECT_NAME} PROPERTIES
	C_STANDARD 99
	CXX_STANDARD 20
	CXX_EXTENSIONS OFF
)

target_compile_options(${PROJECT_NAME} PRIVATE -fopenmp -Wall -Wextra -Wpedantic -O3)
target_link_options(${PROJECT_NAME} PRIVATE -fopenmp)
target_link_libraries(${PROJECT_NAME} PRIVATE nlohmann_json::nlohmann_json indicators::indicators Boost::system Boost::program_options)
target_include_directories(${PROJECT_NAME} PRIVATE ./include)
