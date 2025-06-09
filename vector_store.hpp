#ifndef VECTOR_STORE_HPP
#define VECTOR_STORE_HPP

#include <vector>
#include <memory>
#include <cmath>
#include <stdexcept>
#include <utility>  // for std::pair

class Vector {
private:
    std::vector<double> data;
    size_t dimension;

public:
    // Constructor
    Vector(size_t dim) : dimension(dim) {
        data.resize(dim, 0.0);
    }

    // Copy constructor
    Vector(const Vector& other) : dimension(other.dimension), data(other.data) {}

    // Get dimension
    size_t getDimension() const { return dimension; }

    // Access element
    double& operator[](size_t index) {
        if (index >= dimension) {
            throw std::out_of_range("Index out of bounds");
        }
        return data[index];
    }

    // Const access element
    const double& operator[](size_t index) const {
        if (index >= dimension) {
            throw std::out_of_range("Index out of bounds");
        }
        return data[index];
    }

    // Calculate Euclidean distance between two vectors
    double distance(const Vector& other) const {
        if (dimension != other.dimension) {
            throw std::runtime_error("Vectors must have same dimension");
        }
        
        double sum = 0.0;
        for (size_t i = 0; i < dimension; ++i) {
            double diff = data[i] - other.data[i];
            sum += diff * diff;
        }
        return std::sqrt(sum);
    }
};

class VectorStore {
private:
    std::vector<Vector> vectors;
    size_t dimension;

public:
    // Constructor
    VectorStore(size_t dim) : dimension(dim) {}

    // Add a vector to the store
    void addVector(const Vector& vec) {
        if (vec.getDimension() != dimension) {
            throw std::runtime_error("Vector dimension does not match store dimension");
        }
        vectors.push_back(vec);
    }

    // Remove a vector by index
    void removeVector(size_t index) {
        if (index >= vectors.size()) {
            throw std::out_of_range("Index out of bounds");
        }
        vectors.erase(vectors.begin() + index);
    }

    // Get number of vectors
    size_t size() const {
        return vectors.size();
    }

    // Get the dimension of vectors in the store
    size_t getDimension() const { return dimension; }

    // Get vector by index
    const Vector& getVector(size_t index) const {
        if (index >= vectors.size()) {
            throw std::runtime_error("Vector index out of range");
        }
        return vectors[index];
    }

    // Find nearest neighbor
    size_t findNearestNeighbor(const Vector& query) const {
        if (vectors.empty()) {
            throw std::runtime_error("Vector store is empty");
        }

        size_t nearest_idx = 0;
        double min_distance = query.distance(vectors[0]);

        for (size_t i = 1; i < vectors.size(); ++i) {
            double dist = query.distance(vectors[i]);
            if (dist < min_distance) {
                min_distance = dist;
                nearest_idx = i;
            }
        }

        return nearest_idx;
    }

    // Find all neighbors above similarity threshold
    std::vector<std::pair<size_t, double>> findNeighborsAboveThreshold(
        const Vector& query,
        double threshold
    ) const {
        if (vectors.empty()) {
            throw std::runtime_error("Vector store is empty");
        }

        std::vector<std::pair<size_t, double>> results;
        
        for (size_t i = 0; i < vectors.size(); ++i) {
            double dist = query.distance(vectors[i]);
            // Convert distance to similarity (1 / (1 + distance))
            double similarity = 1.0 / (1.0 + dist);
            
            if (similarity >= threshold) {
                results.emplace_back(i, similarity);
            }
        }

        // Sort results by similarity in descending order
        std::sort(results.begin(), results.end(),
            [](const auto& a, const auto& b) {
                return a.second > b.second;
            }
        );

        return results;
    }
};

#endif // VECTOR_STORE_HPP 