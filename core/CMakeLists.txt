cmake_minimum_required(VERSION 3.12)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_GENERATOR Ninja CACHE INTERNAL "" FORCE)
set(CMAKE_COLOR_DIAGNOSTICS ON)

project(projector C CXX)

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

#get Catch2 for tests
FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v3.3.2
)
FetchContent_MakeAvailable(Catch2)


set(PROJECTOR_LIB_SRC
	src/xcom_loader.cpp
	src/utils.cpp
	src/cross_sections.cpp
	src/geometry.cpp
)

set(PROJECTOR_TESTS_SRC
)


#
# PROJECTOR CORE
#
add_executable(projector_core ${PROJECTOR_LIB_SRC} src/main.cpp)

set_target_properties(projector_core PROPERTIES
	C_STANDARD 99
	CXX_STANDARD 20
	CXX_EXTENSIONS OFF
)

target_compile_options(projector_core PRIVATE -Wall -Wextra -pedantic -O0)

target_link_options(projector_core PRIVATE)

target_link_libraries(projector_core PRIVATE
	nlohmann_json::nlohmann_json
	#indicators::indicators
	CLI11::CLI11
	Eigen3::Eigen)

target_include_directories(projector_core PRIVATE ./include)


#
# TESTS
#
add_executable(projector_tests ${PROJECTOR_LIB_SRC} ${PROJECTOR_TESTS_SRC})

set_target_properties(projector_tests PROPERTIES
	C_STANDARD 99
	CXX_STANDARD 20
	CXX_EXTENSIONS OFF
)

target_compile_options(projector_tests PRIVATE -fopenmp -Wall -Wextra -pedantic -g -gdwarf)

target_link_options(projector_tests PRIVATE -fopenmp)

target_link_libraries(projector_tests PRIVATE
	nlohmann_json::nlohmann_json
	indicators::indicators
	CLI11::CLI11
	Eigen3::Eigen
	Catch2::Catch2WithMain)

target_include_directories(projector_tests PRIVATE ./include)