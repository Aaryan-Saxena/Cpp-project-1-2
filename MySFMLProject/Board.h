#ifndef BOARD_H
#define BOARD_H

#include <SFML/Graphics.hpp>
#include <string>
#include <map>

class Board {
public:
    Board();
    void loadTextures();
    void draw(sf::RenderWindow& window);
    void handleClick(int x, int y);
    bool isKingInCheck(int kingRow, int kingCol, bool isWhiteKing);


private:
    std::string board[8][8];
    std::map<std::string, sf::Texture> textures;
    std::map<std::string, sf::Sprite> sprites;
    sf::Vector2i selectedTile = {-1, -1};
    bool isTileSelected = false;
    bool isWhiteTurn = true;
    



    void setupInitialPosition();
};

#endif
