#include <iostream>
#include "vector_store.hpp"
#include "vector_visualizer.hpp"
#include <random>
#include <cmath>

int main() {
    // Create a vector store
    VectorStore store("test_store");
    
    // Create and add a keyspace
    auto keyspace = std::make_shared<Keyspace>(3, "test_keyspace");
    store.addKeyspace(keyspace);

    // Add some sample 3D vectors
    // Create a sphere of vectors
    const int numVectors = 20;
    const float radius = 2.0f;
    for (int i = 0; i < numVectors; ++i) {
        float theta = 2.0f * M_PI * i / numVectors;
        float phi = M_PI * i / numVectors;
        Vector vec(3);  // 3D vector
        vec[0] = radius * std::sin(phi) * std::cos(theta);
        vec[1] = radius * std::sin(phi) * std::sin(theta);
        vec[2] = radius * std::cos(phi);
        keyspace->addVector(vec);
    }

    // Add some random vectors
    for (int i = 0; i < 5; ++i) {
        Vector vec(3);  // 3D vector
        vec[0] = (rand() % 100) / 50.0f - 1.0f;  // Random value between -1 and 1
        vec[1] = (rand() % 100) / 50.0f - 1.0f;
        vec[2] = (rand() % 100) / 50.0f - 1.0f;
        keyspace->addVector(vec);
    }

    // Create and run the visualizer
    VectorVisualizer visualizer(store);
    visualizer.run();

    return 0;
} 