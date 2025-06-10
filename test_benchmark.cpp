#include "vector_store.hpp"
#include <spdlog/spdlog.h>
#include <chrono>
#include <random>
#include <vector>
#include <memory>
#include <iomanip>

using namespace std::chrono;

// Helper function to generate random vectors
std::vector<float> generateRandomVector(size_t dimension) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dis(-1.0f, 1.0f);
    
    std::vector<float> vec(dimension);
    for (size_t i = 0; i < dimension; ++i) {
        vec[i] = dis(gen);
    }
    return vec;
}

// Helper function to measure memory usage
size_t getCurrentMemoryUsage() {
    // Note: This is a simplified version. In a real application,
    // you might want to use platform-specific methods to get accurate memory usage
    return sizeof(VectorStore) + sizeof(Keyspace);
}

void runBenchmark(size_t numVectors, size_t vectorDimension, size_t numKeyspaces) {
    spdlog::info("Starting benchmark with {} vectors of dimension {} in {} keyspaces", 
                 numVectors, vectorDimension, numKeyspaces);

    // Create store
    auto start = high_resolution_clock::now();
    VectorStore store("benchmark_store");
    auto end = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(end - start);
    spdlog::info("Store creation time: {} microseconds", duration.count());

    // Create keyspaces
    std::vector<std::shared_ptr<Keyspace>> keyspaces;
    for (size_t i = 0; i < numKeyspaces; ++i) {
        keyspaces.push_back(store.createKeyspace("keyspace_" + std::to_string(i)));
    }

    // Measure insertion time
    start = high_resolution_clock::now();
    for (size_t i = 0; i < numVectors; ++i) {
        auto vec = generateRandomVector(vectorDimension);
        keyspaces[i % numKeyspaces]->addVector("vec_" + std::to_string(i), vec);
    }
    end = high_resolution_clock::now();
    duration = duration_cast<microseconds>(end - start);
    spdlog::info("Insertion time for {} vectors: {} microseconds ({} microseconds per vector)", 
                 numVectors, duration.count(), duration.count() / numVectors);

    // Measure search time
    start = high_resolution_clock::now();
    for (size_t i = 0; i < 100; ++i) {  // Perform 100 searches
        auto queryVec = generateRandomVector(vectorDimension);
        auto results = keyspaces[0]->search(queryVec, 5);
    }
    end = high_resolution_clock::now();
    duration = duration_cast<microseconds>(end - start);
    spdlog::info("Average search time: {} microseconds per search", duration.count() / 100);

    // Measure memory usage
    size_t memoryUsage = getCurrentMemoryUsage();
    spdlog::info("Estimated memory usage: {} bytes", memoryUsage);

    // Measure deletion time
    start = high_resolution_clock::now();
    for (size_t i = 0; i < numVectors; ++i) {
        keyspaces[i % numKeyspaces]->removeVector("vec_" + std::to_string(i));
    }
    end = high_resolution_clock::now();
    duration = duration_cast<microseconds>(end - start);
    spdlog::info("Deletion time for {} vectors: {} microseconds ({} microseconds per vector)", 
                 numVectors, duration.count(), duration.count() / numVectors);
}

int main() {
    spdlog::set_level(spdlog::level::info);
    
    // Run benchmarks with different sizes
    spdlog::info("\n=== Small Scale Benchmark ===");
    runBenchmark(1000, 128, 5);
    
    spdlog::info("\n=== Medium Scale Benchmark ===");
    runBenchmark(10000, 256, 10);
    
    spdlog::info("\n=== Large Scale Benchmark ===");
    runBenchmark(100000, 512, 20);

    return 0;
} 