cmake_minimum_required(VERSION 3.12)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(projector_core C CXX)

#get json lib
include(FetchContent)
FetchContent_Declare(json 
	URL https://github.com/nlohmann/json/releases/download/v3.11.2/json.tar.xz)
FetchContent_MakeAvailable(json)

#get progress bar lib
FetchContent_Declare(indicators
	GIT_REPOSITORY https://github.com/p-ranav/indicators.git
	GIT_TAG v2.3
	GIT_SHALLOW TRUE)
FetchContent_MakeAvailable(indicators)

#get command line parsing lib
FetchContent_Declare(cli11
	GIT_REPOSITORY https://github.com/CLIUtils/CLI11.git
	GIT_TAG  v2.3.2
	GIT_SHALLOW TRUE)
FetchContent_MakeAvailable(cli11)

#get Eigen lib (to remove system dependency)
FetchContent_Declare(Eigen
	URL https://gitlab.com/libeigen/eigen/-/archive/3.4.0/eigen-3.4.0.tar.gz)
set(EIGEN_BUILD_DOC OFF)
set(EIGEN_BUILD_PKGCONFIG OFF)
set(EIGEN_BUILD_TESTING OFF)
FetchContent_MakeAvailable(Eigen)

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
target_link_libraries(${PROJECT_NAME} PRIVATE
	nlohmann_json::nlohmann_json
	indicators::indicators
	CLI11::CLI11
	Eigen3::Eigen)

target_include_directories(${PROJECT_NAME} PRIVATE ./include)
