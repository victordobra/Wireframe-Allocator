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
file(GLOB_RECURSE SOURCES ${PROJECT_SOURCE_DIR}/src/*.cpp)
list(LENGTH SOURCES SOURCE_COUNT)

if(SOURCE_COUNT EQUAL 0)
    message(FATAL_ERROR "ERROR: No sources found!")
elseif(ENGINE_SOURCE_COUNT EQUAL 1)
    message(STATUS "1 source found.")
else()
    message(STATUS "${SOURCE_COUNT} sources found.")
endif()

# Create the executable
add_library(${PROJECT_NAME} ${SOURCES})
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_20)
message(STATUS "Library created successfully.")

# Add the include directories
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include ${PROJECT_SOURCE_DIR}/src ${PROJECT_SOURCE_DIR}/external/Vulkan/include)
message(STATUS "Include directories added.")
message(STATUS "Link libraries added.")

# Add the link directories and link the libraries
target_link_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/external/Core/lib ${PROJECT_SOURCE_DIR}/external/ImGui/lib ${PROJECT_SOURCE_DIR}/external/Vulkan/lib)
message(STATUS "Link directories added.")
target_link_libraries(${PROJECT_NAME} ImGui-x86_64 Wireframe-Core-x86_64 vulkan-x86_64)
message(STATUS "Link libraries added.")

# Add the precompiled header
target_precompile_headers(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include/WireframeAllocator.hpp)
message(STATUS "Precompiled header added.")

set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)