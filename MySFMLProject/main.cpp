#include <SFML/Graphics.hpp>
#include "Board.h"

int main() {
    sf::RenderWindow window(sf::VideoMode(800, 800), "Chess in SFML");

    sf::RectangleShape tile(sf::Vector2f(100, 100));
    Board board;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
    
            // Check for mouse click
            if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    int mouseX = event.mouseButton.x;
                    int mouseY = event.mouseButton.y;
                    board.handleClick(mouseX, mouseY);
                }
            }
        }
    
        window.clear();
    
        // Draw board tiles
        for (int row = 0; row < 8; ++row) {
            for (int col = 0; col < 8; ++col) {
                tile.setPosition(col * 100, row * 100);
                tile.setFillColor((row + col) % 2 == 0 ? sf::Color::White : sf::Color(118, 150, 86));
                window.draw(tile);
            }
        }
    
        board.draw(window);
        window.display();
    }
    

    return 0;
}
