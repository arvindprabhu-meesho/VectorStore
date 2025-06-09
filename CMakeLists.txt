cmake_minimum_required(VERSION 3.10)
project(VectorStore)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find SFML
find_package(SFML 3.0 COMPONENTS Window Graphics System REQUIRED)

# If using Homebrew on Apple Silicon, SFML may be in /opt/homebrew
if(APPLE)
    include_directories(/opt/homebrew/include)
    link_directories(/opt/homebrew/lib)
endif()

# Main vector store executable
add_executable(vector_store main.cpp)

# Test visualization executable
add_executable(test_visualization test_visualization.cpp)

# Add the 3D test visualization executable
add_executable(test_3d_visualization test_3d_visualization.cpp)

# Link SFML libraries
target_link_libraries(vector_store PRIVATE sfml-window sfml-graphics sfml-system)
target_link_libraries(test_visualization PRIVATE sfml-window sfml-graphics sfml-system)
target_link_libraries(test_3d_visualization PRIVATE sfml-window sfml-graphics sfml-system) 