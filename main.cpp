#include <iostream>
#include "vector_store.hpp"

int main() {
    // Create a vector store for 3-dimensional vectors
    VectorStore store(3);

    // Create and add some vectors
    Vector v1(3);
    v1[0] = 1.0; v1[1] = 2.0; v1[2] = 3.0;
    store.addVector(v1);

    Vector v2(3);
    v2[0] = 4.0; v2[1] = 5.0; v2[2] = 6.0;
    store.addVector(v2);

    Vector v3(3);
    v3[0] = 7.0; v3[1] = 8.0; v3[2] = 9.0;
    store.addVector(v3);

    // Create a query vector
    Vector query(3);
    query[0] = 2.0; query[1] = 3.0; query[2] = 4.0;

    // Find the nearest neighbor
    size_t nearest_idx = store.findNearestNeighbor(query);
    const Vector& nearest = store.getVector(nearest_idx);

    std::cout << "Number of vectors in store: " << store.size() << std::endl;
    std::cout << "Nearest neighbor index: " << nearest_idx << std::endl;
    std::cout << "Nearest neighbor vector: [" 
              << nearest[0] << ", " 
              << nearest[1] << ", " 
              << nearest[2] << "]" << std::endl;
    std::cout << "Distance to query: " << query.distance(nearest) << std::endl;

    return 0;
} 