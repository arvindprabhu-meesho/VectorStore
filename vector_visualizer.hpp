#ifndef VECTOR_VISUALIZER_HPP
#define VECTOR_VISUALIZER_HPP

#include <SFML/Graphics.hpp>
#include "vector_store.hpp"
#include <vector>
#include <memory>
#include <cmath>
#include <optional>
#include <string>
#include <sstream>
#include <iomanip>

class VectorVisualizer {
private:
    sf::RenderWindow window;
    const VectorStore& store;
    float scale;
    sf::Vector2f center;
    std::vector<sf::CircleShape> vectorPoints;
    std::vector<sf::VertexArray> connections;
    sf::Font font;
    sf::Text infoText;
    sf::Text xAxisLabel;
    sf::Text yAxisLabel;
    std::vector<sf::Text> xMarkers;
    std::vector<sf::Text> yMarkers;
    // 3D visualization state
    struct Quaternion {
        float w, x, y, z;
        Quaternion() : w(1), x(0), y(0), z(0) {}
        Quaternion(float w, float x, float y, float z) : w(w), x(x), y(y), z(z) {}
    };
    Quaternion rotation;
    bool is3D = false;

    // Normalize quaternion
    void normalizeQuaternion(Quaternion& q) {
        float len = std::sqrt(q.w * q.w + q.x * q.x + q.y * q.y + q.z * q.z);
        if (len > 0) {
            q.w /= len;
            q.x /= len;
            q.y /= len;
            q.z /= len;
        }
    }

    // Multiply two quaternions
    Quaternion multiplyQuaternions(const Quaternion& q1, const Quaternion& q2) const {
        return Quaternion(
            q1.w * q2.w - q1.x * q2.x - q1.y * q2.y - q1.z * q2.z,
            q1.w * q2.x + q1.x * q2.w + q1.y * q2.z - q1.z * q2.y,
            q1.w * q2.y - q1.x * q2.z + q1.y * q2.w + q1.z * q2.x,
            q1.w * q2.z + q1.x * q2.y - q1.y * q2.x + q1.z * q2.w
        );
    }

    // Create rotation quaternion from axis and angle
    Quaternion createRotationQuaternion(float angle, float x, float y, float z) {
        float halfAngle = angle * 0.5f;
        float s = std::sin(halfAngle);
        return Quaternion(std::cos(halfAngle), x * s, y * s, z * s);
    }

    // Rotate a vector using quaternion
    std::vector<float> rotateVector(const std::vector<float>& v, const Quaternion& q) const {
        // Convert vector to quaternion
        Quaternion vq(0, v[0], v[1], v[2]);
        // Create conjugate of rotation quaternion
        Quaternion qc(q.w, -q.x, -q.y, -q.z);
        // Perform rotation: q * vq * qc
        Quaternion result = multiplyQuaternions(q, multiplyQuaternions(vq, qc));
        return {result.x, result.y, result.z};
    }

    // Project a 3D point to 2D screen coordinates (perspective)
    sf::Vector2f project3D(const std::vector<float>& v) const {
        // Apply rotation
        std::vector<float> rotated = rotateVector(v, rotation);
        float x = rotated[0], y = rotated[1], z = rotated[2];
        
        // Perspective projection
        float fov = 500.0f;
        float perspective = fov / (fov + z);
        float sx = x * scale * perspective + center.x;
        float sy = -y * scale * perspective + center.y;
        return sf::Vector2f(sx, sy);
    }

    // Draw 3D axes
    void draw3DAxes() {
        // X axis: red, Y axis: green, Z axis: blue
        std::vector<std::pair<std::vector<float>, sf::Color>> axes = {
            {{0,0,0}, sf::Color::White}, {{1,0,0}, sf::Color::Red},
            {{0,0,0}, sf::Color::White}, {{0,1,0}, sf::Color::Green},
            {{0,0,0}, sf::Color::White}, {{0,0,1}, sf::Color::Blue}
        };
        for (size_t i = 0; i < axes.size(); i += 2) {
            sf::VertexArray axis(sf::PrimitiveType::Lines, 2);
            axis[0].position = project3D(axes[i].first);
            axis[0].color = axes[i+1].second;
            axis[1].position = project3D(axes[i+1].first);
            axis[1].color = axes[i+1].second;
            window.draw(axis);
        }
        // Axis labels
        sf::Text xLabel(font);
        xLabel.setString("x");
        xLabel.setCharacterSize(16);
        xLabel.setFillColor(sf::Color::Red);
        xLabel.setPosition(project3D({1.1f, 0, 0}));
        window.draw(xLabel);

        sf::Text yLabel(font);
        yLabel.setString("y");
        yLabel.setCharacterSize(16);
        yLabel.setFillColor(sf::Color::Green);
        yLabel.setPosition(project3D({0, 1.1f, 0}));
        window.draw(yLabel);

        sf::Text zLabel(font);
        zLabel.setString("z");
        zLabel.setCharacterSize(16);
        zLabel.setFillColor(sf::Color::Blue);
        zLabel.setPosition(project3D({0, 0, 1.1f}));
        window.draw(zLabel);
    }

    // Draw 3D vectors
    void draw3DVectors() {
        for (size_t i = 0; i < store.size(); ++i) {
            const Vector& vec = store.getVector(i);
            // Draw line from origin to vector
            sf::VertexArray line(sf::PrimitiveType::Lines, 2);
            line[0].position = project3D({0,0,0});
            line[0].color = sf::Color(100, 100, 100);
            line[1].position = project3D({static_cast<float>(vec[0]), 
                                        static_cast<float>(vec[1]), 
                                        static_cast<float>(vec[2])});
            line[1].color = sf::Color::Yellow;
            window.draw(line);
            // Draw point at vector tip
            sf::CircleShape point(5.f);
            point.setFillColor(sf::Color::Yellow);
            sf::Vector2f tip = project3D({static_cast<float>(vec[0]), 
                                        static_cast<float>(vec[1]), 
                                        static_cast<float>(vec[2])});
            point.setPosition(tip);
            point.setOrigin(sf::Vector2f(5.f, 5.f));
            window.draw(point);
        }
    }

    // Convert vector coordinates to screen coordinates (2D)
    sf::Vector2f toScreenCoords(const Vector& vec) const {
        float x = static_cast<float>(vec[0]) * scale + center.x;
        float y = -static_cast<float>(vec[1]) * scale + center.y;  // Negative y because screen coordinates are inverted
        return sf::Vector2f(x, y);
    }

    // Create a point representation of a vector (2D)
    sf::CircleShape createVectorPoint(const Vector& vec, const sf::Color& color) const {
        sf::CircleShape point(5.f);
        point.setFillColor(color);
        point.setPosition(toScreenCoords(vec));
        point.setOrigin(sf::Vector2f(5.f, 5.f));  // Center the point
        return point;
    }

    // Create a line from origin to vector point (2D)
    sf::VertexArray createVectorLine(const Vector& vec, const sf::Color& color) const {
        sf::VertexArray line(sf::PrimitiveType::Lines, 2);
        line[0].position = center;
        line[0].color = color;
        line[1].position = toScreenCoords(vec);
        line[1].color = color;
        return line;
    }

    // Update all visual elements with current scale (2D)
    void updateVisualElements() {
        vectorPoints.clear();
        connections.clear();
        if (!is3D) {
            for (size_t i = 0; i < store.size(); ++i) {
                const Vector& vec = store.getVector(i);
                vectorPoints.push_back(createVectorPoint(vec, sf::Color::Green));
                connections.push_back(createVectorLine(vec, sf::Color(100, 100, 100)));
            }
            updateAxisMarkers();
        }
    }

    // Update axis markers based on current scale (2D)
    void updateAxisMarkers() {
        xMarkers.clear();
        yMarkers.clear();
        if (is3D) return; // No axis markers for 3D for now
        float markerSpacing = 1.0f;
        if (scale < 20.0f) markerSpacing = 2.0f;
        if (scale < 10.0f) markerSpacing = 5.0f;
        float xStart = -window.getSize().x / (2 * scale);
        float xEnd = window.getSize().x / (2 * scale);
        for (float x = std::ceil(xStart / markerSpacing) * markerSpacing; x <= xEnd; x += markerSpacing) {
            sf::Text marker(font);
            marker.setCharacterSize(12);
            marker.setFillColor(sf::Color::White);
            std::stringstream ss;
            ss << std::fixed << std::setprecision(1) << x;
            marker.setString(ss.str());
            Vector vec(2); vec[0] = x; vec[1] = 0;
            sf::Vector2f pos = toScreenCoords(vec);
            marker.setPosition(sf::Vector2f(pos.x - 10, center.y + 5));
            xMarkers.push_back(marker);
        }
        float yStart = -window.getSize().y / (2 * scale);
        float yEnd = window.getSize().y / (2 * scale);
        for (float y = std::ceil(yStart / markerSpacing) * markerSpacing; y <= yEnd; y += markerSpacing) {
            sf::Text marker(font);
            marker.setCharacterSize(12);
            marker.setFillColor(sf::Color::White);
            std::stringstream ss;
            ss << std::fixed << std::setprecision(1) << y;
            marker.setString(ss.str());
            Vector vec(2); vec[0] = 0; vec[1] = y;
            sf::Vector2f pos = toScreenCoords(vec);
            marker.setPosition(sf::Vector2f(center.x + 5, pos.y - 10));
            yMarkers.push_back(marker);
        }
    }

    // Draw coordinate axes (2D)
    void drawAxes() {
        if (is3D) {
            draw3DAxes();
            return;
        }
        sf::VertexArray xAxis(sf::PrimitiveType::Lines, 2);
        xAxis[0].position = sf::Vector2f(0, center.y);
        xAxis[0].color = sf::Color(100, 100, 100);
        xAxis[1].position = sf::Vector2f(window.getSize().x, center.y);
        xAxis[1].color = sf::Color(100, 100, 100);
        sf::VertexArray yAxis(sf::PrimitiveType::Lines, 2);
        yAxis[0].position = sf::Vector2f(center.x, 0);
        yAxis[0].color = sf::Color(100, 100, 100);
        yAxis[1].position = sf::Vector2f(center.x, window.getSize().y);
        yAxis[1].color = sf::Color(100, 100, 100);
        window.draw(xAxis);
        window.draw(yAxis);
        window.draw(xAxisLabel);
        window.draw(yAxisLabel);
        for (const auto& marker : xMarkers) window.draw(marker);
        for (const auto& marker : yMarkers) window.draw(marker);
    }

    // Update info text
    void updateInfoText() {
        std::string info = is3D ? "3D Vector Space Visualization\n" : "2D Vector Space Visualization\n";
        info += "Number of vectors: " + std::to_string(store.size()) + "\n";
        info += "Scale: " + std::to_string(scale) + "\n";
        info += is3D ? "Use mouse wheel or arrow keys to zoom, arrow keys to rotate" : "Use mouse wheel or arrow keys to zoom";
        infoText.setString(info);
    }

    // Draw orientation indicator in subwindow
    void drawOrientationIndicator() {
        if (!is3D) return;

        // Create a small view for the orientation indicator
        sf::View indicatorView;
        float indicatorSize = 100.f;
        indicatorView.setViewport(sf::FloatRect(sf::Vector2f(0.8f, 0.8f), sf::Vector2f(0.2f, 0.2f)));
        indicatorView.setSize(sf::Vector2f(indicatorSize, indicatorSize));
        indicatorView.setCenter(sf::Vector2f(indicatorSize/2, indicatorSize/2));
        window.setView(indicatorView);

        // Draw background
        sf::RectangleShape background(sf::Vector2f(indicatorSize, indicatorSize));
        background.setFillColor(sf::Color(20, 20, 20, 200));
        window.draw(background);

        // Draw axes in the indicator
        float axisLength = 40.f;
        std::vector<std::pair<std::vector<float>, sf::Color>> axes = {
            {{0,0,0}, sf::Color::White}, {{1,0,0}, sf::Color::Red},
            {{0,0,0}, sf::Color::White}, {{0,1,0}, sf::Color::Green},
            {{0,0,0}, sf::Color::White}, {{0,0,1}, sf::Color::Blue}
        };

        // Create a temporary rotation for the indicator
        Quaternion indicatorRotation = rotation;
        // Add a fixed rotation to make the indicator more intuitive
        Quaternion fixedRotation = createRotationQuaternion(0.5f, 1, 0, 0);
        indicatorRotation = multiplyQuaternions(fixedRotation, indicatorRotation);
        normalizeQuaternion(indicatorRotation);

        for (size_t i = 0; i < axes.size(); i += 2) {
            sf::VertexArray axis(sf::PrimitiveType::Lines, 2);
            std::vector<float> start = rotateVector(axes[i].first, indicatorRotation);
            std::vector<float> end = rotateVector(axes[i+1].first, indicatorRotation);
            
            // Scale and center the axes
            float scale = 20.f;
            axis[0].position = sf::Vector2f(
                start[0] * scale + indicatorSize/2,
                -start[1] * scale + indicatorSize/2
            );
            axis[0].color = axes[i+1].second;
            axis[1].position = sf::Vector2f(
                end[0] * scale + indicatorSize/2,
                -end[1] * scale + indicatorSize/2
            );
            axis[1].color = axes[i+1].second;
            window.draw(axis);
        }

        // Draw axis labels
        sf::Text xLabel(font);
        xLabel.setString("x");
        xLabel.setCharacterSize(12);
        xLabel.setFillColor(sf::Color::Red);
        std::vector<float> xEnd = rotateVector({1.1f, 0, 0}, indicatorRotation);
        xLabel.setPosition(sf::Vector2f(
            xEnd[0] * 20.f + indicatorSize/2,
            -xEnd[1] * 20.f + indicatorSize/2
        ));
        window.draw(xLabel);

        sf::Text yLabel(font);
        yLabel.setString("y");
        yLabel.setCharacterSize(12);
        yLabel.setFillColor(sf::Color::Green);
        std::vector<float> yEnd = rotateVector({0, 1.1f, 0}, indicatorRotation);
        yLabel.setPosition(sf::Vector2f(
            yEnd[0] * 20.f + indicatorSize/2,
            -yEnd[1] * 20.f + indicatorSize/2
        ));
        window.draw(yLabel);

        sf::Text zLabel(font);
        zLabel.setString("z");
        zLabel.setCharacterSize(12);
        zLabel.setFillColor(sf::Color::Blue);
        std::vector<float> zEnd = rotateVector({0, 0, 1.1f}, indicatorRotation);
        zLabel.setPosition(sf::Vector2f(
            zEnd[0] * 20.f + indicatorSize/2,
            -zEnd[1] * 20.f + indicatorSize/2
        ));
        window.draw(zLabel);

        // Reset the view
        window.setView(window.getDefaultView());
    }

public:
    VectorVisualizer(const VectorStore& vectorStore, 
                    unsigned int width = 800, 
                    unsigned int height = 600)
        : window(sf::VideoMode(sf::Vector2u(width, height)), "Vector Space Visualization")
        , store(vectorStore)
        , scale(50.f)
        , center(width/2.f, height/2.f)
        , infoText(font)
        , xAxisLabel(font)
        , yAxisLabel(font)
    {
        // Load font
        if (!font.openFromFile("/System/Library/Fonts/Helvetica.ttc")) {
            throw std::runtime_error("Could not load font");
        }
        // Detect if 3D
        is3D = (store.getDimension() == 3);
        // Setup info text
        infoText.setCharacterSize(14);
        infoText.setFillColor(sf::Color::White);
        infoText.setPosition(sf::Vector2f(10, 10));
        // Setup axis labels
        xAxisLabel.setString("x");
        xAxisLabel.setCharacterSize(16);
        xAxisLabel.setFillColor(sf::Color::White);
        xAxisLabel.setPosition(sf::Vector2f(window.getSize().x - 20, center.y + 5));
        yAxisLabel.setString("y");
        yAxisLabel.setCharacterSize(16);
        yAxisLabel.setFillColor(sf::Color::White);
        yAxisLabel.setPosition(sf::Vector2f(center.x + 5, 10));
        // Create initial visual elements
        updateVisualElements();
    }

    void run() {
        while (window.isOpen()) {
            // Handle events
            while (const std::optional event = window.pollEvent()) {
                // Window closed or escape key pressed: exit
                if (event->is<sf::Event::Closed>() ||
                    (event->is<sf::Event::KeyPressed>() &&
                     event->getIf<sf::Event::KeyPressed>()->code == sf::Keyboard::Key::Escape)) {
                    window.close();
                }
                // Handle zoom with mouse wheel
                else if (const auto* wheelEvent = event->getIf<sf::Event::MouseWheelScrolled>()) {
                    if (wheelEvent->delta > 0) {
                        scale *= 1.1f;  // Zoom in
                    } else {
                        scale /= 1.1f;  // Zoom out
                    }
                    updateVisualElements();
                }
                // Handle rotation and zoom with arrow keys
                else if (const auto* keyEvent = event->getIf<sf::Event::KeyPressed>()) {
                    if (is3D) {
                        float rotationSpeed = 0.1f;
                        Quaternion deltaRotation;
                        
                        if (keyEvent->code == sf::Keyboard::Key::Up) {
                            deltaRotation = createRotationQuaternion(rotationSpeed, 1, 0, 0);
                        } else if (keyEvent->code == sf::Keyboard::Key::Down) {
                            deltaRotation = createRotationQuaternion(-rotationSpeed, 1, 0, 0);
                        } else if (keyEvent->code == sf::Keyboard::Key::Left) {
                            deltaRotation = createRotationQuaternion(rotationSpeed, 0, 1, 0);
                        } else if (keyEvent->code == sf::Keyboard::Key::Right) {
                            deltaRotation = createRotationQuaternion(-rotationSpeed, 0, 1, 0);
                        } else if (keyEvent->code == sf::Keyboard::Key::Q) {
                            deltaRotation = createRotationQuaternion(rotationSpeed, 0, 0, 1);
                        } else if (keyEvent->code == sf::Keyboard::Key::E) {
                            deltaRotation = createRotationQuaternion(-rotationSpeed, 0, 0, 1);
                        } else if (keyEvent->code == sf::Keyboard::Key::Add || keyEvent->code == sf::Keyboard::Key::Equal) {
                            scale *= 1.1f;
                        } else if (keyEvent->code == sf::Keyboard::Key::Subtract || keyEvent->code == sf::Keyboard::Key::Hyphen) {
                            scale /= 1.1f;
                        }
                        
                        // Apply rotation
                        rotation = multiplyQuaternions(deltaRotation, rotation);
                        normalizeQuaternion(rotation);
                    } else {
                        if (keyEvent->code == sf::Keyboard::Key::Up || 
                            keyEvent->code == sf::Keyboard::Key::Right) {
                            scale *= 1.1f;  // Zoom in
                            updateVisualElements();
                        }
                        else if (keyEvent->code == sf::Keyboard::Key::Down || 
                                 keyEvent->code == sf::Keyboard::Key::Left) {
                            scale /= 1.1f;  // Zoom out
                            updateVisualElements();
                        }
                    }
                }
            }
            // Update display
            updateInfoText();
            window.clear(sf::Color(30, 30, 30));
            drawAxes();
            if (is3D) {
                draw3DVectors();
                drawOrientationIndicator();  // Add the orientation indicator
            } else {
                for (const auto& connection : connections) window.draw(connection);
                for (const auto& point : vectorPoints) window.draw(point);
            }
            window.draw(infoText);
            window.display();
        }
    }
};

#endif // VECTOR_VISUALIZER_HPP 