#ifndef VECTOR_STORE_HPP
#define VECTOR_STORE_HPP

#include <vector>
#include <memory>
#include <cmath>
#include <stdexcept>
#include <utility>  // for std::pair
#include <mutex>
#include <spdlog/spdlog.h>

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
    double euclideanDistance(const Vector& other) const {
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


    double cosineSimilarity(const Vector& other) const {
        if (dimension != other.dimension) {
            throw std::runtime_error("Vectors must have same dimension");
        }
        
        double dotProduct = 0.0;
        double magnitude1 = 0.0;
        double magnitude2 = 0.0;

        for (size_t i = 0; i < dimension; ++i) {
            dotProduct += data[i] * other.data[i];
            magnitude1 += data[i] * data[i];
            magnitude2 += other.data[i] * other.data[i];
        }

        double magnitude = std::sqrt(magnitude1 * magnitude2);
        if (magnitude == 0) {
            return 0.0;
        }
        return dotProduct / magnitude;
    }

    double manhattanDistance(const Vector& other) const {
        if (dimension != other.dimension) {
            throw std::runtime_error("Vectors must have same dimension");
        }
        double sum = 0.0;
        for (size_t i = 0; i < dimension; ++i) {
            sum += std::abs(data[i] - other.data[i]);
        }
        return sum;
    }
};


class Keyspace {
private:
    std::vector<Vector> vectors;
    size_t dimension;
    std::mutex mtx;
    std::string keyspace_name;
public:
    // Constructor
    Keyspace(size_t dim, std::string name) : dimension(dim), keyspace_name(name) {
        spdlog::info("Created keyspace: {}", name);
    }

    // Destructor
    ~Keyspace() {
        spdlog::info("Destroyed keyspace: {}", keyspace_name);
    }

    std::string getName() const {
        return keyspace_name;
    }

    // Get number of vectors
    size_t size() const {
        return vectors.size();
    }

    // Get the dimension of vectors in the store
    size_t getDimension() const { return dimension; }

    // Add a vector to the store
    void addVector(const Vector& vec) {
        mtx.lock();
        if (vec.getDimension() != dimension) {
            throw std::runtime_error("Vector dimension does not match store dimension");
        }
        vectors.push_back(vec);
        mtx.unlock();
    }

    void batchAddVectors(const std::vector<Vector>& vectors){
        mtx.lock();
        for(const Vector& vec : vectors){
            if(vec.getDimension() != dimension){
                throw std::runtime_error("Vector dimension does not match store dimension");
            }
            this->vectors.push_back(vec);
        }
        mtx.unlock();
    }

    // Remove a vector by index
    void removeVector(size_t index) {
        mtx.lock();
        if (index >= vectors.size()) {
            throw std::out_of_range("Index out of bounds");
        }
        vectors.erase(vectors.begin() + index);
        mtx.unlock();
    }
    
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
        double min_distance = query.euclideanDistance(vectors[0]);

        for (size_t i = 1; i < vectors.size(); ++i) {
            double dist = query.euclideanDistance(vectors[i]);
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
            double dist = query.euclideanDistance(vectors[i]);
            // Convert euclideanDistance to similarity (1 / (1 + euclideanDistance))
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

class VectorStore {
private:
    std::vector<std::shared_ptr<Keyspace>> keyspaces;
    std::mutex mtx;
    std::string vector_store_name;
public:
    VectorStore(std::string name) : vector_store_name(name) {
        spdlog::info("Initializing VectorStore: {}", name);
    }

    void addKeyspace(const std::shared_ptr<Keyspace>& keyspace) {
        mtx.lock();
        keyspaces.push_back(keyspace);
        spdlog::info("Added keyspace: {}", keyspace->getName());
        mtx.unlock();
    }

    void removeKeyspace(const std::string& name) {
        mtx.lock();
        keyspaces.erase(
            std::remove_if(keyspaces.begin(), keyspaces.end(),
                [&](const std::shared_ptr<Keyspace>& k) { 
                    return k->getName() == name; 
                }
            ),
            keyspaces.end()
        );
        spdlog::info("Removed keyspace: {}, from VectorStore: {}", name, vector_store_name);
        mtx.unlock();
    }

    std::shared_ptr<Keyspace> getKeyspace(const std::string& name) const {
        for(const auto& keyspace: keyspaces) {
            if(keyspace->getName() == name) {
                return keyspace;
            }
        }
        spdlog::error("Keyspace not found: {} in VectorStore: {}", name, vector_store_name);
        throw std::runtime_error("Keyspace not found");
    }
};

#endif // VECTOR_STORE_HPP 