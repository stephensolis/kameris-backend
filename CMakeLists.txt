cmake_minimum_required(VERSION 3.1.0)

# build types
set(CMAKE_CONFIGURATION_TYPES Debug Release ReleaseIntel Coverage
	CACHE TYPE INTERNAL FORCE)
if(NOT CMAKE_BUILD_TYPE)
	# default build type is debug
	set(CMAKE_BUILD_TYPE Debug
		CACHE STRING "" FORCE)
elseif(NOT (CMAKE_BUILD_TYPE STREQUAL "Debug" OR CMAKE_BUILD_TYPE STREQUAL "Release" OR
			CMAKE_BUILD_TYPE STREQUAL "ReleaseIntel" OR CMAKE_BUILD_TYPE STREQUAL "Coverage"))
	message(FATAL_ERROR "Unknown build type '${CMAKE_BUILD_TYPE}'")
endif()

# force use of intel's compiler for release builds
if(CMAKE_BUILD_TYPE STREQUAL "ReleaseIntel")
	if(WIN32)
		set(CMAKE_CXX_COMPILER "icl")
	else()
		set(CMAKE_CXX_COMPILER "icpc")
	endif()
endif()

project(modmap-generator-cpp
		LANGUAGES C CXX)

# note CMAKE_CXX_COMPILER_ID isn't defined until here
if(CMAKE_BUILD_TYPE STREQUAL "Coverage" AND NOT (CMAKE_CXX_COMPILER_ID STREQUAL "GNU"))
	message(FATAL_ERROR "Coverage support is only available for gcc")
endif()

# source file lists
file(GLOB_RECURSE PROJECT_SOURCES
	 benchmarks/*.[ch]pp
	 libmmg/*.[ch]pp
	 minimal-clis/*.[ch]pp
	 mmg-formats/cpp/*.[ch]pp
	 src/*.[ch]pp
	 tests/*.[ch]pp)
file(GLOB_RECURSE SOURCES_MMG_CLI
	 src/common/*.cpp
	 src/mmg-cli/*.cpp)
file(GLOB_RECURSE SOURCES_TEST_MMG
	 tests/*.cpp)

# check whether compiler uses MSVC-like options or gcc-like options
if(MSVC OR (WIN32 AND CMAKE_CXX_COMPILER_ID STREQUAL "Intel"))
	set(MSVC_LIKE_COMPILER YES)
else()
	set(MSVC_LIKE_COMPILER NO)
endif()

# flags for release mode
if(WIN32 AND CMAKE_CXX_COMPILER_ID STREQUAL "Intel")
	set(CMAKE_CXX_FLAGS_RELEASE "/EHsc /nologo /W3 /fast")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
	set(CMAKE_CXX_FLAGS_RELEASE "/EHsc /nologo /W3 /Ox")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Intel")
	set(CMAKE_CXX_FLAGS_RELEASE "-std=gnu++14 -w3 -diag-disable:remark -fast -static")
else()
	set(CMAKE_CXX_FLAGS_RELEASE "-std=gnu++14 -Wall -Wextra -Ofast -static")
endif()
set(CMAKE_CXX_FLAGS_RELEASEINTEL "${CMAKE_CXX_FLAGS_RELEASE}")

# flags for debug mode
if(MSVC_LIKE_COMPILER)
	set(CMAKE_CXX_FLAGS_DEBUG "/EHsc /nologo /W3 /Zi")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Intel")
	set(CMAKE_CXX_FLAGS_DEBUG "-std=gnu++14 -w3 -diag-disable:remark -g")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	set(CMAKE_CXX_FLAGS_DEBUG "-std=gnu++14 -Wall -Wextra -Og -ggdb")
else()
	set(CMAKE_CXX_FLAGS_DEBUG "-std=gnu++14 -Wall -Wextra -O1 -g")
endif()

# flags for coverage mode
set(CMAKE_CXX_FLAGS_COVERAGE "-std=gnu++14 -Wall -Wextra --coverage")
set(CMAKE_EXE_LINKER_FLAGS_COVERAGE "${CMAKE_EXE_LINKER_FLAGS} --coverage")

# fetch version from git tag info
execute_process(COMMAND git describe --always
				OUTPUT_VARIABLE VERSION_SHORT
				ERROR_QUIET
				OUTPUT_STRIP_TRAILING_WHITESPACE)
if(NOT VERSION_SHORT)
	set(VERSION_SHORT "version-unknown")
endif()
add_definitions(-DVERSION_SHORT="${VERSION_SHORT}")

# setup for boost
set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_STATIC_RUNTIME ON)
if(MSVC_LIKE_COMPILER)
	set(Boost_USE_DEBUG_RUNTIME OFF)
	add_definitions(-DBOOST_ALL_NO_LIB)
endif()
find_package(Boost 1.59 REQUIRED
			 COMPONENTS filesystem program_options system)
include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})

# this is needed to use <thread> on Linux
set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)
find_package(Threads REQUIRED)

# setup for other include directories
include_directories("${PROJECT_SOURCE_DIR}")
include_directories(SYSTEM "${PROJECT_SOURCE_DIR}/external")

# sets up a release target
function(add_cli_intel_release_target target_name flags_windows flags_linux build_type)
	add_executable(${target_name} ${SOURCES_MMG_CLI})
	target_link_libraries(${target_name} ${Boost_LIBRARIES} Threads::Threads)

	# platform-specific flags
	if(WIN32)
		target_compile_options(${target_name} PRIVATE "${flags_windows}")
	else()
		target_compile_options(${target_name} PRIVATE "${flags_linux}")
	endif()

	# version info
	target_compile_definitions(${target_name} PRIVATE -DBUILD_TYPE="${build_type}")
endfunction()

# add main targets
if(CMAKE_BUILD_TYPE STREQUAL "ReleaseIntel")
	add_cli_intel_release_target(mmg-cli-sse3 "/QxSSE3" "-xSSE3" "release-sse3")
	add_cli_intel_release_target(mmg-cli-sse41 "/QxSSE4.1" "-xSSE4.1" "release-sse4.1")
	add_cli_intel_release_target(mmg-cli-avx "/QxAVX" "-xAVX" "release-avx")
	add_cli_intel_release_target(mmg-cli-avx2 "/QxCORE-AVX2" "-xCORE-AVX2" "release-avx2")
	add_cli_intel_release_target(mmg-cli-avx512 "/QxCORE-AVX512" "-xCORE-AVX512" "release-avx512")
else()
	add_executable(mmg-cli ${SOURCES_MMG_CLI})
	target_link_libraries(mmg-cli ${Boost_LIBRARIES} Threads::Threads)

	string(TOLOWER "${CMAKE_BUILD_TYPE}" build_type)
	target_compile_definitions(mmg-cli PRIVATE -DBUILD_TYPE="${build_type}")
endif()

# add test target
add_executable(test-mmg EXCLUDE_FROM_ALL
			   ${SOURCES_TEST_MMG})
target_link_libraries(test-mmg ${Boost_LIBRARIES} Threads::Threads)
add_custom_target(check-tests
				  COMMAND test-mmg VERBATIM
				  DEPENDS test-mmg)

# setup for style checking targets
string(REGEX REPLACE "([^;]+)" "-I\\1" BOOST_INCLUDES_STRING "${Boost_INCLUDE_DIRS}")
set(CHECK_LINT_LOGFILE "${CMAKE_BINARY_DIR}/check_lint_log")
set(CMAKE_BACKED_TARGET_DIRECTORY "${CMAKE_BINARY_DIR}/cmake-scripts/")
file(MAKE_DIRECTORY "${CMAKE_BACKED_TARGET_DIRECTORY}")
function(add_cmake_backed_target target_name command)
	file(WRITE "${CMAKE_BACKED_TARGET_DIRECTORY}/${target_name}.cmake"
		 "set(PROJECT_SOURCES \"${PROJECT_SOURCES}\") \n\
		  set(BOOST_INCLUDES_STRING \"${BOOST_INCLUDES_STRING}\") \n\
		  set(CHECK_LINT_LOGFILE \"${CHECK_LINT_LOGFILE}\") \n\
		  ${command}")
	add_custom_target(${target_name}
					  COMMAND "${CMAKE_COMMAND}" -P "${CMAKE_BACKED_TARGET_DIRECTORY}/${target_name}.cmake" VERBATIM
					  WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}")
endfunction()

# add style checking targets
add_cmake_backed_target(check-lint [[
						foreach(SOURCE_FILE ${PROJECT_SOURCES})
							execute_process(COMMAND clang-tidy "${SOURCE_FILE}" -- -I. -Iexternal ${BOOST_INCLUDES_STRING} --std=gnu++14
											OUTPUT_FILE "${CHECK_LINT_LOGFILE}"
											ERROR_FILE "${CHECK_LINT_LOGFILE}")
							execute_process(COMMAND	grep -Ev "(warnings.*generated\\.|Error while processing|Suppressed.*warnings|display errors from all non-system headers)"
											COMMAND perl external/colorgcc.pl
											INPUT_FILE "${CHECK_LINT_LOGFILE}")
						endforeach(SOURCE_FILE)]])
add_cmake_backed_target(check-format [[
						foreach(SOURCE_FILE ${PROJECT_SOURCES})
							execute_process(COMMAND clang-format -style=file "${SOURCE_FILE}"
											COMMAND perl external/colordiff.pl -u --expand-tabs --tabsize=4 --verifymode "${SOURCE_FILE}" -)
						endforeach(SOURCE_FILE)]])

# add all-test target
add_custom_target(check-all-1 DEPENDS check-tests check-all-2)
add_custom_target(check-all-2 DEPENDS check-lint check-all-3)
add_custom_target(check-all-3 DEPENDS check-format)
add_custom_target(check-all DEPENDS check-all-1)
add_custom_target(check DEPENDS check-all)

# add coverage target
if(CMAKE_BUILD_TYPE STREQUAL "Coverage")
	set(COVERAGE_OUTPUT_FILE "${CMAKE_BINARY_DIR}/coverage.info")
	set(COVERAGE_IGNORE_PATHS "${PROJECT_SOURCE_DIR}/tests/*" "${PROJECT_SOURCE_DIR}/external/*" "/usr/*")
	add_custom_target(coverage
					  COMMAND lcov --directory "${CMAKE_BINARY_DIR}" --zerocounters VERBATIM
					  COMMAND "${CMAKE_COMMAND}" --build "${CMAKE_BINARY_DIR}" --target check-tests VERBATIM
					  COMMAND lcov --directory "${CMAKE_BINARY_DIR}" --capture --output-file "${COVERAGE_OUTPUT_FILE}" VERBATIM
					  COMMAND lcov --remove "${COVERAGE_OUTPUT_FILE}" ${COVERAGE_IGNORE_PATHS} --output-file "${COVERAGE_OUTPUT_FILE}" VERBATIM)
endif()
