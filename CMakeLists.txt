# Include the local .env file
include(./.env.cmake OPTIONAL RESULT_VARIABLE LOCAL_ENV)
message(STATUS "Local .env file included successfully.")

# Set the minumum required version
cmake_minimum_required(VERSION ${CMAKE_MIN_VERSION})
message(STATUS "CMake minimum required version set to ${CMAKE_MIN_VERSION}.")

# Create the project
project(${PROJECT_NAME} VERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}.${PROJECT_VERSION_TWEAK})
message(STATUS "Project name set to \"${PROJECT_NAME}\".")
message(STATUS "Project version set to ${PROJECT_VERSION}.")

# Enable testing
include(CTest)
enable_testing()
message(STATUS "Testing enabled successfilly")

# Find the source files
file(GLOB_RECURSE SOURCES ${PROJECT_SOURCE_DIR}/src/*.c)
list(LENGTH SOURCES SOURCE_COUNT)

if(SOURCE_COUNT EQUAL 0)
    message(FATAL_ERROR "ERROR: No sources found!")
elseif(SOURCE_COUNT EQUAL 1)
    message(STATUS "1 source found.")
else()
    message(STATUS "${SOURCE_COUNT} sources found.")
endif()

# Create the library
add_library(${PROJECT_NAME} ${SOURCES})
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_20)
message(STATUS "Library created successfully.")

# Add the include directories
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include ${PROJECT_SOURCE_DIR}/src ${PROJECT_SOURCE_DIR}/external/Vulkan/include)
message(STATUS "Include directories added.")

# Add the link directories and link the libraries
target_link_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/external/Vulkan/lib)
message(STATUS "Link directories added.")
target_link_libraries(${PROJECT_NAME} vulkan-x86_64)
message(STATUS "Link libraries added.")

# Add the precompiled header
target_precompile_headers(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include/WireframeAllocator.h)
message(STATUS "Precompiled header added.")

# Find the test sources
file(GLOB_RECURSE TESTS ${PROJECT_SOURCE_DIR}/test/*.c)
list(LENGTH TESTS TEST_COUNT)

if(TEST_COUNT EQUAL 0)
    message(FATAL_ERROR "ERROR: No tests found!")
elseif(TEST_COUNT EQUAL 1)
    message(STATUS "1 test found.")
else()
    message(STATUS "${SOURCE_COUNT} tests found.")
endif()

# Create the test executable
add_executable(Test ${TESTS})
target_compile_features(Test PUBLIC cxx_std_20)
message(STATUS "Test executable created successfully.")

# Add the include directories
target_include_directories(Test PUBLIC ${PROJECT_SOURCE_DIR}/include ${PROJECT_SOURCE_DIR}/test ${PROJECT_SOURCE_DIR}/external/Vulkan/include)
message(STATUS "Test include directories added.")

# Add the link directories and link the libraries
target_link_directories(Test PUBLIC ${PROJECT_SOURCE_DIR}/external/Vulkan/lib)
message(STATUS "Test link directories added.")
target_link_libraries(Test vulkan-x86_64 ${PROJECT_NAME})
message(STATUS "Test link libraries added.")

set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)