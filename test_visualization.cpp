#include "vector_store.hpp"
#include "vector_visualizer.hpp"
#include <cmath>

int main() {
    // Create a vector store
    VectorStore store("test_store");
    
    // Create and add a keyspace
    auto keyspace = std::make_shared<Keyspace>(2, "test_keyspace");
    store.addKeyspace(keyspace);

    // Add some sample 2D vectors
    // Create a circle of vectors
    const int numVectors = 8;
    const float radius = 2.0f;
    for (int i = 0; i < numVectors; ++i) {
        float angle = 2.0f * M_PI * i / numVectors;
        Vector vec(2);  // 2D vector
        vec[0] = radius * std::cos(angle);
        vec[1] = radius * std::sin(angle);
        keyspace->addVector(vec);
    }

    // Add some random vectors
    for (int i = 0; i < 5; ++i) {
        Vector vec(2);  // 2D vector
        vec[0] = (rand() % 100) / 50.0f - 1.0f;  // Random value between -1 and 1
        vec[1] = (rand() % 100) / 50.0f - 1.0f;  // Random value between -1 and 1
        keyspace->addVector(vec);
    }

    // Create and run the visualizer
    VectorVisualizer visualizer(store);
    visualizer.run();

    return 0;
} 