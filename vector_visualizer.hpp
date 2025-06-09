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
    
    // Convert vector coordinates to screen coordinates
    sf::Vector2f toScreenCoords(const Vector& vec) const {
        float x = vec[0] * scale + center.x;
        float y = -vec[1] * scale + center.y;  // Negative y because screen coordinates are inverted
        return sf::Vector2f(x, y);
    }

    // Create a point representation of a vector
    sf::CircleShape createVectorPoint(const Vector& vec, const sf::Color& color) const {
        sf::CircleShape point(5.f);
        point.setFillColor(color);
        point.setPosition(toScreenCoords(vec));
        point.setOrigin(sf::Vector2f(5.f, 5.f));  // Center the point
        return point;
    }

    // Create a line from origin to vector point
    sf::VertexArray createVectorLine(const Vector& vec, const sf::Color& color) const {
        sf::VertexArray line(sf::PrimitiveType::Lines, 2);
        line[0].position = center;
        line[0].color = color;
        line[1].position = toScreenCoords(vec);
        line[1].color = color;
        return line;
    }

    // Update all visual elements with current scale
    void updateVisualElements() {
        vectorPoints.clear();
        connections.clear();
        for (size_t i = 0; i < store.size(); ++i) {
            const Vector& vec = store.getVector(i);
            vectorPoints.push_back(createVectorPoint(vec, sf::Color::Green));
            connections.push_back(createVectorLine(vec, sf::Color(100, 100, 100)));
        }
        updateAxisMarkers();
    }

    // Update axis markers based on current scale
    void updateAxisMarkers() {
        xMarkers.clear();
        yMarkers.clear();

        // Calculate marker spacing based on scale
        float markerSpacing = 1.0f;  // One unit per marker
        if (scale < 20.0f) markerSpacing = 2.0f;
        if (scale < 10.0f) markerSpacing = 5.0f;

        // Create x-axis markers
        float xStart = -window.getSize().x / (2 * scale);
        float xEnd = window.getSize().x / (2 * scale);
        for (float x = std::ceil(xStart / markerSpacing) * markerSpacing; x <= xEnd; x += markerSpacing) {
            sf::Text marker(font);
            marker.setCharacterSize(12);
            marker.setFillColor(sf::Color::White);
            
            std::stringstream ss;
            ss << std::fixed << std::setprecision(1) << x;
            marker.setString(ss.str());
            
            Vector vec(2);
            vec[0] = x;
            vec[1] = 0;
            sf::Vector2f pos = toScreenCoords(vec);
            marker.setPosition(sf::Vector2f(pos.x - 10, center.y + 5));
            xMarkers.push_back(marker);
        }

        // Create y-axis markers
        float yStart = -window.getSize().y / (2 * scale);
        float yEnd = window.getSize().y / (2 * scale);
        for (float y = std::ceil(yStart / markerSpacing) * markerSpacing; y <= yEnd; y += markerSpacing) {
            sf::Text marker(font);
            marker.setCharacterSize(12);
            marker.setFillColor(sf::Color::White);
            
            std::stringstream ss;
            ss << std::fixed << std::setprecision(1) << y;
            marker.setString(ss.str());
            
            Vector vec(2);
            vec[0] = 0;
            vec[1] = y;
            sf::Vector2f pos = toScreenCoords(vec);
            marker.setPosition(sf::Vector2f(center.x + 5, pos.y - 10));
            yMarkers.push_back(marker);
        }
    }

    // Draw coordinate axes
    void drawAxes() {
        // Draw x-axis
        sf::VertexArray xAxis(sf::PrimitiveType::Lines, 2);
        xAxis[0].position = sf::Vector2f(0, center.y);
        xAxis[0].color = sf::Color(100, 100, 100);
        xAxis[1].position = sf::Vector2f(window.getSize().x, center.y);
        xAxis[1].color = sf::Color(100, 100, 100);

        // Draw y-axis
        sf::VertexArray yAxis(sf::PrimitiveType::Lines, 2);
        yAxis[0].position = sf::Vector2f(center.x, 0);
        yAxis[0].color = sf::Color(100, 100, 100);
        yAxis[1].position = sf::Vector2f(center.x, window.getSize().y);
        yAxis[1].color = sf::Color(100, 100, 100);

        window.draw(xAxis);
        window.draw(yAxis);

        // Draw axis labels
        window.draw(xAxisLabel);
        window.draw(yAxisLabel);

        // Draw markers
        for (const auto& marker : xMarkers) {
            window.draw(marker);
        }
        for (const auto& marker : yMarkers) {
            window.draw(marker);
        }
    }

    // Update info text
    void updateInfoText() {
        std::string info = "Vector Space Visualization\n";
        info += "Number of vectors: " + std::to_string(store.size()) + "\n";
        info += "Scale: " + std::to_string(scale) + "\n";
        info += "Use mouse wheel or arrow keys to zoom";
        infoText.setString(info);
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
                    // On macOS, delta is positive when scrolling up (zoom in)
                    // and negative when scrolling down (zoom out)
                    if (wheelEvent->delta > 0) {
                        scale *= 1.1f;  // Zoom in
                    } else {
                        scale /= 1.1f;  // Zoom out
                    }
                    updateVisualElements();
                }
                // Handle zoom with arrow keys
                else if (const auto* keyEvent = event->getIf<sf::Event::KeyPressed>()) {
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

            // Update display
            updateInfoText();

            // Draw everything
            window.clear(sf::Color(30, 30, 30));
            
            drawAxes();

            // Draw connections and points
            for (const auto& connection : connections) {
                window.draw(connection);
            }
            for (const auto& point : vectorPoints) {
                window.draw(point);
            }

            window.draw(infoText);
            window.display();
        }
    }
};

#endif // VECTOR_VISUALIZER_HPP 