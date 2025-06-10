#include "vector_store.hpp"
#include <iostream>
#include <random>

// Helper function to create a random vector
Vector createRandomVector(size_t dimension) {
    Vector vec(dimension);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(-1.0, 1.0);
    
    for(size_t i = 0; i < dimension; ++i) {
        vec[i] = dis(gen);
    }
    return vec;
}

int main() {
    try {
        // Initialize vector store
        VectorStore store("test_store");
        
        // Create and add a keyspace
        auto keyspace = std::make_shared<Keyspace>(3, "test_keyspace");
        store.addKeyspace(keyspace);
        
        // Add some random vectors to the keyspace
        std::vector<Vector> vectors;
        for(int i = 0; i < 5; ++i) {
            vectors.push_back(createRandomVector(3));
        }
        keyspace->batchAddVectors(vectors);
        
        // Create a query vector
        Vector query = createRandomVector(3);
        
        // Find nearest neighbor
        size_t nearest_idx = keyspace->findNearestNeighbor(query);
        spdlog::info("Nearest neighbor index: {}", nearest_idx);
        
        // Find neighbors above threshold
        auto neighbors = keyspace->findNeighborsAboveThreshold(query, 0.5);
        spdlog::info("Found {} neighbors above threshold", neighbors.size());
        
        // Remove the keyspace
        store.removeKeyspace("test_keyspace");
        
        // Try to get removed keyspace (should throw)
        try {
            store.getKeyspace("test_keyspace");
        } catch(const std::runtime_error& e) {
            spdlog::info("Successfully caught error for removed keyspace");
        }
        
    } catch(const std::exception& e) {
        spdlog::error("Error: {}", e.what());
        return 1;
    }
    
    return 0;
} 