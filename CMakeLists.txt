cmake_minimum_required(VERSION 3.14)
project(VectorStore)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Fetch spdlog
include(FetchContent)
FetchContent_Declare(
    spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG v1.12.0
)
FetchContent_MakeAvailable(spdlog)

# Find SFML
find_package(SFML 3.0 COMPONENTS Window Graphics System REQUIRED)

# If using Homebrew on Apple Silicon, SFML may be in /opt/homebrew
if(APPLE)
    include_directories(/opt/homebrew/include)
    link_directories(/opt/homebrew/lib)
endif()

# Main vector store executable
add_executable(vector_store main.cpp)
target_link_libraries(vector_store PRIVATE spdlog::spdlog sfml-window sfml-graphics sfml-system)

# Test visualization executable
add_executable(test_visualization test_visualization.cpp)
target_link_libraries(test_visualization PRIVATE spdlog::spdlog sfml-window sfml-graphics sfml-system)

# Add the 3D test visualization executable
add_executable(test_3d_visualization test_3d_visualization.cpp)
target_link_libraries(test_3d_visualization PRIVATE spdlog::spdlog sfml-window sfml-graphics sfml-system)

# Add the benchmark test executable
add_executable(test_benchmark test_benchmark.cpp)
target_link_libraries(test_benchmark PRIVATE spdlog::spdlog) 