#include "vector_store.hpp"
#include "vector_visualizer.hpp"
#include <cmath>

int main() {
    // Create a vector store for 2D vectors
    VectorStore store(2);  // Specify dimension as 2 for 2D vectors

    // Add some sample 2D vectors
    // Create a circle of vectors
    const int numVectors = 8;
    const float radius = 2.0f;
    for (int i = 0; i < numVectors; ++i) {
        float angle = 2.0f * M_PI * i / numVectors;
        Vector vec(2);  // 2D vector
        vec[0] = radius * std::cos(angle);
        vec[1] = radius * std::sin(angle);
        store.addVector(vec);
    }

    // Add some random vectors
    for (int i = 0; i < 5; ++i) {
        Vector vec(2);  // 2D vector
        vec[0] = (rand() % 100) / 50.0f - 1.0f;  // Random value between -1 and 1
        vec[1] = (rand() % 100) / 50.0f - 1.0f;  // Random value between -1 and 1
        store.addVector(vec);
    }

    // Create and run the visualizer
    VectorVisualizer visualizer(store);
    visualizer.run();

    return 0;
} 