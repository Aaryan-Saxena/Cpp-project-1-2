#include "Board.h"
#include <iostream>
#include <SFML/Graphics.hpp>

Board::Board() {
    setupInitialPosition();
    loadTextures();
}

void Board::setupInitialPosition() {
    // Empty board
    for (int r = 0; r < 8; ++r)
        for (int c = 0; c < 8; ++c)
            board[r][c] = "";

    // Pawns
    for (int i = 0; i < 8; ++i) {
        board[1][i] = "bp";
        board[6][i] = "wp";
    }

    // Rooks, Knights, Bishops
    std::string pieces[8] = {"br", "bn", "bb", "bq", "bk", "bb", "bn", "br"};
    for (int i = 0; i < 8; ++i) {
        board[0][i] = pieces[i];
        board[7][i] = "w" + pieces[i].substr(1);
    }
}

void Board::loadTextures() {
    std::string names[] = {"wp", "wr", "wn", "wb", "wq", "wk", "bp", "br", "bn", "bb", "bq", "bk"};

    for (const auto& name : names) {
        sf::Texture tex;
        if (!tex.loadFromFile("assets/" + name + ".png")) {
            std::cerr << "Failed to load texture: " << name << std::endl;
        }
        textures[name] = tex;

        sf::Sprite sprite;
        sprite.setTexture(textures[name]);
        sprite.setScale(100.0f / tex.getSize().x, 100.0f / tex.getSize().y);
        sprites[name] = sprite;
    }
}

void Board::draw(sf::RenderWindow& window) {
    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            std::string piece = board[row][col];
            if (piece != "") {
                sf::Sprite sprite = sprites[piece];
                sprite.setPosition(col * 100, row * 100);
                window.draw(sprite);
            }
        }
    }
}

void Board::handleClick(int x, int y) {
    int col = x / 100;
    int row = y / 100;

    if (!isTileSelected) {
        // Select a piece if it's your turn
        std::string piece = board[row][col];
        if (piece != "") {
            bool isWhitePiece = piece[0] == 'w';
            if ((isWhiteTurn && isWhitePiece) || (!isWhiteTurn && !isWhitePiece)) {
                selectedTile = {col, row};
                isTileSelected = true;
            }
        }
    } else {
        int fromCol = selectedTile.x;
        int fromRow = selectedTile.y;
        std::string piece = board[fromRow][fromCol];

        // Check if it's a pawn
        if (piece[1] == 'p') {
            bool isWhitePiece = piece[0] == 'w';
            int direction = isWhitePiece ? -1 : 1;
        
            // Move one square forward
            if (col == fromCol && row == fromRow + direction && board[row][col] == "") {
                // Perform the move
                board[row][col] = piece;
                board[fromRow][fromCol] = "";
        
                // Show "check" message if the opponent's king is in check (no blocking)
                bool isOpponentWhite = !isWhitePiece;
                if (isKingInCheck(-1, -1, isOpponentWhite)) {
                    std::cout << (isOpponentWhite ? "White" : "Black") << " king is in check!" << std::endl;
                }
        
                // Proceed with turn change without blocking
                isTileSelected = false;
                isWhiteTurn = !isWhiteTurn;
                return;
            }
        
            // Move two squares forward from starting row
            if (col == fromCol && row == fromRow + 2 * direction &&
                ((isWhitePiece && fromRow == 6) || (!isWhitePiece && fromRow == 1)) &&
                board[fromRow + direction][fromCol] == "" && board[row][col] == "") {
        
                // Perform the move
                board[row][col] = piece;
                board[fromRow][fromCol] = "";
        
                // Show "check" message if the opponent's king is in check (no blocking)
                bool isOpponentWhite = !isWhitePiece;
                if (isKingInCheck(-1, -1, isOpponentWhite)) {
                    std::cout << (isOpponentWhite ? "White" : "Black") << " king is in check!" << std::endl;
                }
        
                // Proceed with turn change without blocking
                isTileSelected = false;
                isWhiteTurn = !isWhiteTurn;

                return;
            }
        
            // Diagonal capture (one step diagonally if enemy piece)
            if ((col == fromCol + 1 || col == fromCol - 1) && row == fromRow + direction) {
                std::string target = board[row][col];
                if (target != "" && target[0] != piece[0]) {
                    // Perform the move
                    board[row][col] = piece;
                    board[fromRow][fromCol] = "";
        
                    // Show "check" message if the opponent's king is in check (no blocking)
                    bool isOpponentWhite = !isWhitePiece;
                    if (isKingInCheck(-1, -1, isOpponentWhite)) {
                        std::cout << (isOpponentWhite ? "White" : "Black") << " king is in check!" << std::endl;
                    }
        
                    // Proceed with turn change without blocking
                    isTileSelected = false;
                    isWhiteTurn = !isWhiteTurn;

                    return;
                }
            }
        
            // Invalid move, cancel selection
            isTileSelected = false;
            return;
        }
        
        // Knight Movement
    else if (piece[1] == 'n') {
    bool isWhitePiece = piece[0] == 'w';
    
    int dx = std::abs(col - fromCol);
    int dy = std::abs(row - fromRow);
    
    // Check L-shape move
    if ((dx == 2 && dy == 1) || (dx == 1 && dy == 2)) {
        std::string target = board[row][col];

        // Allow move if target is empty or opponent's piece
        if (target == "" || target[0] != piece[0]) {
            board[row][col] = piece;
            board[fromRow][fromCol] = "";
            
            // Show "check" message if the opponent's king is in check (no blocking)
            if (isKingInCheck(-1, -1, !isWhitePiece)) {
                std::cout << (isWhitePiece ? "Black" : "White") << " king is in check!" << std::endl;
            }
            
            isTileSelected = false;
            isWhiteTurn = !isWhiteTurn;  // Toggle turns
            return;
        }
    }
    isTileSelected = false;
    return;
}

        // Rook Movement
        else if (piece[1] == 'r') {
            bool isWhitePiece = piece[0] == 'w';
            
            // Moving in straight line
            if (fromRow == row || fromCol == col) {
                int rowDir = (row - fromRow) == 0 ? 0 : (row - fromRow) / std::abs(row - fromRow);
                int colDir = (col - fromCol) == 0 ? 0 : (col - fromCol) / std::abs(col - fromCol);
            
                int r = fromRow + rowDir;
                int c = fromCol + colDir;
            
                // Check for obstacles
                while (r != row || c != col) {
                    if (board[r][c] != "") {
                        isTileSelected = false;
                        return; // Blocked!
                    }
                    r += rowDir;
                    c += colDir;
                }
            
                std::string target = board[row][col];
                if (target == "" || target[0] != piece[0]) {
                    board[row][col] = piece;
                    board[fromRow][fromCol] = "";
                    
                    // Show "check" message if the opponent's king is in check (no blocking)
                    if (isKingInCheck(-1, -1, !isWhitePiece)) {
                        std::cout << (isWhitePiece ? "Black" : "White") << " king is in check!" << std::endl;
                    }
                    
                    isTileSelected = false;
                    isWhiteTurn = !isWhiteTurn;  // Toggle turns
                    return;
                }
            }
            isTileSelected = false;
            return;
        }

        // Bishop Movement
        else if (piece[1] == 'b') {
            bool isWhitePiece = piece[0] == 'w';
            
            int dx = col - fromCol;
            int dy = row - fromRow;
            
            // Must move diagonally (equal steps)
            if (std::abs(dx) == std::abs(dy)) {
                int rowDir = dy / std::abs(dy);
                int colDir = dx / std::abs(dx);
            
                int r = fromRow + rowDir;
                int c = fromCol + colDir;
            
                // Check for obstacles
                while (r != row && c != col) {
                    if (board[r][c] != "") {
                        isTileSelected = false;
                        return; // Blocked!
                    }
                    r += rowDir;
                    c += colDir;
                }
            
                std::string target = board[row][col];
                if (target == "" || target[0] != piece[0]) {
                    board[row][col] = piece;
                    board[fromRow][fromCol] = "";
                    
                    // Show "check" message if the opponent's king is in check (no blocking)
                    if (isKingInCheck(-1, -1, !isWhitePiece)) {
                        std::cout << (isWhitePiece ? "Black" : "White") << " king is in check!" << std::endl;
                    }
                    
                    isTileSelected = false;
                    isWhiteTurn = !isWhiteTurn;  // Toggle turns
                    return;
                }
            }
            isTileSelected = false;
            return;
        }
        
        // Queen Movement
        else if (piece[1] == 'q') {
            bool isWhitePiece = piece[0] == 'w';
            
            int dx = col - fromCol;
            int dy = row - fromRow;
            
            int rowDir = (dy == 0) ? 0 : dy / std::abs(dy);
            int colDir = (dx == 0) ? 0 : dx / std::abs(dx);
            
            // Must be straight or diagonal
            if ((fromRow == row || fromCol == col) || (std::abs(dx) == std::abs(dy))) {
                int r = fromRow + rowDir;
                int c = fromCol + colDir;
            
                // Check path
                while (r != row || c != col) {
                    if (board[r][c] != "") {
                        isTileSelected = false;
                        return; // Blocked
                    }
                    r += rowDir;
                    c += colDir;
                }
            
                std::string target = board[row][col];
                if (target == "" || target[0] != piece[0]) {
                    board[row][col] = piece;
                    board[fromRow][fromCol] = "";
                    
                    // Show "check" message if the opponent's king is in check (no blocking)
                    if (isKingInCheck(-1, -1, !isWhitePiece)) {
                        std::cout << (isWhitePiece ? "Black" : "White") << " king is in check!" << std::endl;
                    }
                    
                    isTileSelected = false;
                    isWhiteTurn = !isWhiteTurn;  // Toggle turns
                    return;
                }
            }
            isTileSelected = false;
            return;
        }
        

        // King Movement
        else if (piece[1] == 'k') {
            bool isWhitePiece = piece[0] == 'w';
            
            int dx = std::abs(col - fromCol);
            int dy = std::abs(row - fromRow);
            
            // King moves only 1 square in any direction
            if (dx <= 1 && dy <= 1) {
                std::string target = board[row][col];
                if (target == "" || target[0] != piece[0]) {
                    board[row][col] = piece;
                    board[fromRow][fromCol] = "";
                    
                    // Show "check" message if the opponent's king is in check (no blocking)
                    if (isKingInCheck(-1, -1, !isWhitePiece)) {
                        std::cout << (isWhitePiece ? "Black" : "White") << " king is in check!" << std::endl;
                    }
                    
                    isTileSelected = false;
                    isWhiteTurn = !isWhiteTurn;  // Toggle turns
                    return;
                }
            }
            isTileSelected = false;
            return;
        }
        
        

        // We haven't added logic for other pieces yet
        isTileSelected = false;
    }
}


bool Board::isKingInCheck(int kingRow, int kingCol, bool isWhiteKing) {
    std::string opponentPrefix = isWhiteKing ? "b" : "w";

    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            std::string piece = board[row][col];
            if (piece != "" && piece[0] == opponentPrefix[0]) {
                // Check if this piece can attack the king
                if (piece[1] == 'p') {
                    // Check pawn captures
                    int direction = piece[0] == 'w' ? -1 : 1;
                    if (std::abs(col - kingCol) == 1 && row == kingRow + direction) {
                        return true;
                    }
                } else if (piece[1] == 'n') {
                    // Check knight
                    int dx = std::abs(col - kingCol);
                    int dy = std::abs(row - kingRow);
                    if ((dx == 2 && dy == 1) || (dx == 1 && dy == 2)) {
                        return true;
                    }
                } else if (piece[1] == 'r') {
                    // Check rook
                    if (row == kingRow || col == kingCol) {
                        int rDir = (row == kingRow) ? 0 : (row - kingRow) / std::abs(row - kingRow);
                        int cDir = (col == kingCol) ? 0 : (col - kingCol) / std::abs(col - kingCol);

                        int r = kingRow + rDir;
                        int c = kingCol + cDir;

                        bool isBlocked = false;
                        while (r != row || c != col) {
                            if (board[r][c] != "") {
                                isBlocked = true;
                                break;
                            }
                            r += rDir;
                            c += cDir;
                        }
                        if (!isBlocked) {
                            return true;
                        }
                    }
                } else if (piece[1] == 'b') {
                    // Check bishop
                    if (std::abs(col - kingCol) == std::abs(row - kingRow)) {
                        int rDir = (row - kingRow) / std::abs(row - kingRow);
                        int cDir = (col - kingCol) / std::abs(col - kingCol);

                        int r = kingRow + rDir;
                        int c = kingCol + cDir;

                        bool isBlocked = false;
                        while (r != row || c != col) {
                            if (board[r][c] != "") {
                                isBlocked = true;
                                break;
                            }
                            r += rDir;
                            c += cDir;
                        }
                        if (!isBlocked) {
                            return true;
                        }
                    }
                } else if (piece[1] == 'q') {
                    // Check queen (same as rook + bishop combined)
                    if (row == kingRow || col == kingCol || std::abs(col - kingCol) == std::abs(row - kingRow)) {
                        int rDir = (row == kingRow) ? 0 : (row - kingRow) / std::abs(row - kingRow);
                        int cDir = (col == kingCol) ? 0 : (col - kingCol) / std::abs(col - kingCol);

                        int r = kingRow + rDir;
                        int c = kingCol + cDir;

                        bool isBlocked = false;
                        while (r != row || c != col) {
                            if (board[r][c] != "") {
                                isBlocked = true;
                                break;
                            }
                            r += rDir;
                            c += cDir;
                        }
                        if (!isBlocked) {
                            return true;
                        }
                    }
                }
            }
        }
    }

    return false;
}



