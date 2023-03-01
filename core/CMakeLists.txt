cmake_minimum_required(VERSION 3.12)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(scan_simulator C CXX)

#get json lib
include(FetchContent)
FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.2/json.tar.xz)
FetchContent_MakeAvailable(json)

#get progress bar lib
FetchContent_Declare(indicators
	GIT_REPOSITORY https://github.com/p-ranav/indicators.git
	GIT_TAG 6432875ef4c866d25e324c19a787b5bb90b0ab12)
FetchContent_MakeAvailable(indicators)

#get command line parsing lib
include(FetchContent)
FetchContent_Declare(cli11
	GIT_REPOSITORY https://github.com/CLIUtils/CLI11.git
	GIT_TAG 291c58789c031208f08f4f261a858b5b7083e8e2)
FetchContent_MakeAvailable(cli11)

#create interface library for parsing lib
add_library(cli11 INTERFACE)
target_include_directories(cli11 INTERFACE ${cli11_SOURCE_DIR}/include)

set(CORE_SOURCES
	src/main.cpp
	src/geom.cpp
	src/simulator.cpp
	src/loader.cpp
)

add_executable(core ${CORE_SOURCES})

set_target_properties(core PROPERTIES
	C_STANDARD 99
	CXX_STANDARD 20
	CXX_EXTENSIONS OFF
)

target_compile_options(core PRIVATE -fopenmp -Wall -Wextra -Wpedantic -O3)
target_link_options(core PRIVATE -fopenmp)
target_link_libraries(core PRIVATE nlohmann_json::nlohmann_json indicators::indicators cli11)
target_include_directories(core PRIVATE ./include)
