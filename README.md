# Vector Store

A simple and efficient vector store implementation in C++ that allows storing and retrieving vectors with nearest neighbor search capabilities.

## Features

- Store vectors of any dimension
- Add and remove vectors
- Find nearest neighbors using Euclidean distance
- Efficient memory management using STL containers
- Exception handling for error cases

## Building the Project

This project uses CMake as its build system. To build the project:

1. Create a build directory:
```bash
mkdir build
cd build
```

2. Generate the build files:
```bash
cmake ..
```

3. Build the project:
```bash
make
```

## Usage

The vector store provides two main classes:

1. `Vector`: Represents a single vector with a fixed dimension
   - Create vectors with specified dimension
   - Access elements using operator[]
   - Calculate distances between vectors

2. `VectorStore`: Manages a collection of vectors
   - Add and remove vectors
   - Find nearest neighbors
   - Get vector count and access vectors by index

Example usage can be found in `main.cpp`.

## Requirements

- C++17 or later
- CMake 3.10 or later
- A C++ compiler with C++17 support

## Future Improvements

- Add support for different distance metrics
- Implement k-nearest neighbors search
- Add vector serialization/deserialization
- Implement more efficient nearest neighbor search algorithms (e.g., k-d trees)
- Add support for parallel processing 