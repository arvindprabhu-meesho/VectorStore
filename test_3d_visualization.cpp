#include <iostream>
#include "vector_visualizer.hpp"
#include <random>
#include <cmath>

int main() {
    // Create a vector store for 3D vectors
    VectorStore store(3);

    // Add some sample 3D vectors
    // Unit vectors
    Vector i(3); i[0] = 1.0f; i[1] = 0.0f; i[2] = 0.0f;
    Vector j(3); j[0] = 0.0f; j[1] = 1.0f; j[2] = 0.0f;
    Vector k(3); k[0] = 0.0f; k[1] = 0.0f; k[2] = 1.0f;
    store.addVector(i);
    store.addVector(j);
    store.addVector(k);

    // Add some vectors in the XY plane
    for (float angle = 0; angle < 2 * M_PI; angle += M_PI / 4) {
        Vector v(3);
        v[0] = std::cos(angle);
        v[1] = std::sin(angle);
        v[2] = 0.0f;
        store.addVector(v);
    }

    // Add some vectors in the XZ plane
    for (float angle = 0; angle < 2 * M_PI; angle += M_PI / 4) {
        Vector v(3);
        v[0] = std::cos(angle);
        v[1] = 0.0f;
        v[2] = std::sin(angle);
        store.addVector(v);
    }

    // Add some vectors in the YZ plane
    for (float angle = 0; angle < 2 * M_PI; angle += M_PI / 4) {
        Vector v(3);
        v[0] = 0.0f;
        v[1] = std::cos(angle);
        v[2] = std::sin(angle);
        store.addVector(v);
    }

    // Add some random 3D vectors
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dist(-1.0f, 1.0f);
    
    for (int i = 0; i < 10; ++i) {
        Vector v(3);
        v[0] = dist(gen);
        v[1] = dist(gen);
        v[2] = dist(gen);
        store.addVector(v);
    }

    // Create and run the visualizer
    VectorVisualizer visualizer(store);
    visualizer.run();

    return 0;
} 