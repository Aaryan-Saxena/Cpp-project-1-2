#include "raylib.h"
#include <cstdlib>
#include <ctime>
#include <cmath>

const int screenWidth = 800;
const int screenHeight = 450;

enum GameState { MENU, PLAYER_VS_PLAYER, PLAYER_VS_COMPUTER, GAME_OVER };

struct Paddle {
    Rectangle rect;
    int speed;
    int score;
};

struct Ball {
    Vector2 position;
    Vector2 speed;
    float radius;
};

float Clamp(float value, float min, float max) {
    return (value < min) ? min : ((value > max) ? max : value);
}

// Improved computer paddle AI
void UpdateComputerPaddle(Paddle &computerPaddle, Ball ball, int difficulty) {
    // Calculate target position with prediction
    float targetY = computerPaddle.rect.y + computerPaddle.rect.height/2;
    
    // When ball is moving toward computer, use prediction
    if (ball.speed.x > 0) {
        // Calculate time for ball to reach paddle's x position
        float distanceX = computerPaddle.rect.x - ball.position.x;
        float timeToReach = distanceX / ball.speed.x;
        
        // Predict ball's y position when it reaches paddle
        float predictedY = ball.position.y + (ball.speed.y * timeToReach);
        
        // Handle bounces off top and bottom walls
        float bounceY = predictedY;
        while (bounceY < 0 || bounceY > screenHeight) {
            if (bounceY < 0) bounceY = -bounceY;
            if (bounceY > screenHeight) bounceY = 2*screenHeight - bounceY;
        }
        
        // Set target with some error margin based on difficulty
        float errorMargin = 30.0f - (difficulty * 5.0f);
        if (errorMargin < 0) errorMargin = 0;
        
        targetY = bounceY + GetRandomValue(-errorMargin, errorMargin);
    }
    else {
        // When ball moving away, return to center with some offset
        targetY = screenHeight/2 + GetRandomValue(-20, 20);
    }
    
    // Calculate AI speed based on difficulty
    float aiSpeed = computerPaddle.speed * (0.8f + (difficulty * 0.1f));
    if (difficulty > 3) aiSpeed *= 1.2f; // Extra boost at higher difficulty
    
    // Move paddle toward target
    if (computerPaddle.rect.y + computerPaddle.rect.height/2 < targetY) {
        computerPaddle.rect.y += aiSpeed;
    } else if (computerPaddle.rect.y + computerPaddle.rect.height/2 > targetY) {
        computerPaddle.rect.y -= aiSpeed;
    }
    
    // Keep paddle on screen
    computerPaddle.rect.y = Clamp(computerPaddle.rect.y, 0, screenHeight - computerPaddle.rect.height);
}

void UpdateGame(Paddle &leftPaddle, Paddle &rightPaddle, Ball &ball, Sound bounceSound, Sound scoreSound, 
                GameState &currentState, int &difficulty, bool vsComputer, int &currentBackground) {
    // Move paddles
    if (IsKeyDown(KEY_W)) leftPaddle.rect.y -= leftPaddle.speed;
    if (IsKeyDown(KEY_S)) leftPaddle.rect.y += leftPaddle.speed;
    
    if (vsComputer) {
        UpdateComputerPaddle(rightPaddle, ball, difficulty);
    } else {
        if (IsKeyDown(KEY_UP)) rightPaddle.rect.y -= rightPaddle.speed;
        if (IsKeyDown(KEY_DOWN)) rightPaddle.rect.y += rightPaddle.speed;
    }
    
    // Keep paddles on screen
    leftPaddle.rect.y = Clamp(leftPaddle.rect.y, 0, screenHeight - leftPaddle.rect.height);
    rightPaddle.rect.y = Clamp(rightPaddle.rect.y, 0, screenHeight - rightPaddle.rect.height);

    // Move ball
    ball.position.x += ball.speed.x;
    ball.position.y += ball.speed.y;

    // Ball collision with top and bottom walls
    if (ball.position.y - ball.radius <= 0 || ball.position.y + ball.radius >= screenHeight) {
        ball.speed.y *= -1.0f;
        PlaySound(bounceSound);
    }

    // Ball collision with left paddle
    if (ball.speed.x < 0 && 
        ball.position.x - ball.radius <= leftPaddle.rect.x + leftPaddle.rect.width &&
        ball.position.x - ball.radius > leftPaddle.rect.x &&
        ball.position.y + ball.radius >= leftPaddle.rect.y &&
        ball.position.y - ball.radius <= leftPaddle.rect.y + leftPaddle.rect.height) {
        
        // Basic reflection
        ball.speed.x = fabs(ball.speed.x);
        
        // Add some vertical speed based on where the ball hit the paddle
        float relativeIntersectY = (ball.position.y - (leftPaddle.rect.y + leftPaddle.rect.height/2)) / (leftPaddle.rect.height/2);
        ball.speed.y = relativeIntersectY * 5.0f;
        
        // Ensure minimum horizontal speed
        if (ball.speed.x < 4.0f) ball.speed.x = 4.0f;
        
        // Slight speed increase
        ball.speed.x *= 1.05f;
        if (ball.speed.x > 10.0f) ball.speed.x = 10.0f;
        
        PlaySound(bounceSound);
    }

    // Ball collision with right paddle
    if (ball.speed.x > 0 && 
        ball.position.x + ball.radius >= rightPaddle.rect.x &&
        ball.position.x + ball.radius < rightPaddle.rect.x + rightPaddle.rect.width &&
        ball.position.y + ball.radius >= rightPaddle.rect.y &&
        ball.position.y - ball.radius <= rightPaddle.rect.y + rightPaddle.rect.height) {
        
        // Basic reflection
        ball.speed.x = -fabs(ball.speed.x);
        
        // Add some vertical speed based on where the ball hit the paddle
        float relativeIntersectY = (ball.position.y - (rightPaddle.rect.y + rightPaddle.rect.height/2)) / (rightPaddle.rect.height/2);
        ball.speed.y = relativeIntersectY * 5.0f;
        
        // Ensure minimum horizontal speed
        if (fabs(ball.speed.x) < 4.0f) ball.speed.x = -4.0f;
        
        // Slight speed increase
        ball.speed.x *= 1.05f;
        if (fabs(ball.speed.x) > 10.0f) ball.speed.x = -10.0f;
        
        PlaySound(bounceSound);
    }

    // Scoring
    if (ball.position.x < 0) {
        rightPaddle.score++;
        ball.position = (Vector2){screenWidth/2, screenHeight/2};
        ball.speed = (Vector2){5.0f, (GetRandomValue(0, 1) ? 2.0f : -2.0f)};
        PlaySound(scoreSound);
        
        if (vsComputer) difficulty++;
        currentBackground = GetRandomValue(0, 2);
    }
    
    if (ball.position.x > screenWidth) {
        leftPaddle.score++;
        ball.position = (Vector2){screenWidth/2, screenHeight/2};
        ball.speed = (Vector2){-5.0f, (GetRandomValue(0, 1) ? 2.0f : -2.0f)};
        PlaySound(scoreSound);
        
        currentBackground = GetRandomValue(0, 2);
    }
    
    // Check for game over
    if (leftPaddle.score >= 5 || rightPaddle.score >= 5) {
        currentState = GAME_OVER;
    }
}

void DrawGame(Paddle leftPaddle, Paddle rightPaddle, Ball ball, GameState currentState, bool vsComputer, 
              Texture2D backgrounds[], int currentBackground, RenderTexture2D dottedLineTexture) {
    BeginDrawing();
    
    if (currentState == PLAYER_VS_PLAYER || currentState == PLAYER_VS_COMPUTER) {
        Rectangle source = {0, 0, (float)backgrounds[currentBackground].width, (float)backgrounds[currentBackground].height};
        Rectangle dest = {0, 0, (float)screenWidth, (float)screenHeight};
        DrawTexturePro(backgrounds[currentBackground], source, dest, (Vector2){0, 0}, 0.0f, WHITE);
        DrawRectangle(0, 0, screenWidth, screenHeight, ColorAlpha(BLACK, 0.3f));
        
        DrawRectangleRec(leftPaddle.rect, WHITE);
        DrawRectangleRec(rightPaddle.rect, WHITE);
        DrawCircleV(ball.position, ball.radius, WHITE);
        DrawTexture(dottedLineTexture.texture, screenWidth/2 - 1, 0, WHITE);
        
        DrawText(TextFormat("%d", leftPaddle.score), screenWidth/4, 20, 40, WHITE);
        DrawText(TextFormat("%d", rightPaddle.score), 3*screenWidth/4, 20, 40, WHITE);
        
        DrawText(vsComputer ? "PLAYER VS COMPUTER" : "PLAYER VS PLAYER", 10, screenHeight - 30, 20, GRAY);
    } else {
        ClearBackground(BLACK);
        if (currentState == MENU) {
            DrawText("PONG GAME", screenWidth/2 - MeasureText("PONG GAME", 40)/2, 100, 40, WHITE);
            DrawText("1 - Player vs Player", screenWidth/2 - MeasureText("1 - Player vs Player", 20)/2, 200, 20, WHITE);
            DrawText("2 - Player vs Computer", screenWidth/2 - MeasureText("2 - Player vs Computer", 20)/2, 240, 20, WHITE);
            DrawText("Press the corresponding key to start", screenWidth/2 - MeasureText("Press the corresponding key to start", 20)/2, 300, 20, WHITE);
        } else if (currentState == GAME_OVER) {
            const char* winner = leftPaddle.score >= 5 ? "LEFT PLAYER WINS!" : "RIGHT PLAYER WINS!";
            if (vsComputer) winner = (rightPaddle.score >= 5) ? "COMPUTER WINS!" : "PLAYER WINS!";
            
            DrawText(winner, screenWidth/2 - MeasureText(winner, 40)/2, 180, 40, WHITE);
            DrawText("PRESS ENTER TO RETURN TO MENU", screenWidth/2 - MeasureText("PRESS ENTER TO RETURN TO MENU", 20)/2, 250, 20, WHITE);
            DrawText(TextFormat("FINAL SCORE: %d - %d", leftPaddle.score, rightPaddle.score), 
                     screenWidth/2 - MeasureText(TextFormat("FINAL SCORE: %d - %d", leftPaddle.score, rightPaddle.score), 30)/2, 320, 30, WHITE);
        }
    }
    
    EndDrawing();
}

int main() {
    srand(static_cast<unsigned int>(time(nullptr)));
    
    InitWindow(screenWidth, screenHeight, "Pong Game");
    InitAudioDevice();
    SetTargetFPS(60);

    Paddle leftPaddle = {{50, screenHeight/2 - 50, 10, 100}, 6, 0};
    Paddle rightPaddle = {{screenWidth - 60, screenHeight/2 - 50, 10, 100}, 6, 0};
    Ball ball = {{screenWidth/2, screenHeight/2}, {5, 0}, 7};
    
    Texture2D backgrounds[3] = {
        LoadTexture("assets/background1.jpg"),
        LoadTexture("assets/background2.jpg"),
        LoadTexture("assets/background3.jpg")
    };
    
    RenderTexture2D dottedLineTexture = LoadRenderTexture(2, screenHeight);
    BeginTextureMode(dottedLineTexture);
        ClearBackground(BLANK);
        for (int i = 0; i < screenHeight; i += 15) {
            DrawRectangle(0, i, 2, 10, WHITE);
        }
    EndTextureMode();
    
    int currentBackground = GetRandomValue(0, 2);
    
    Sound bounceSound = LoadSound("assets/bounce.wav");
    Sound scoreSound = LoadSound("assets/score.wav");
    
    GameState currentState = MENU;
    bool vsComputer = false;
    int difficulty = 1;

    while (!WindowShouldClose()) {
        switch (currentState) {
            case MENU:
                if (IsKeyPressed(KEY_ONE)) {
                    leftPaddle.score = rightPaddle.score = 0;
                    ball.position = (Vector2){screenWidth/2, screenHeight/2};
                    ball.speed = (Vector2){5.0f, 0.0f};
                    vsComputer = false;
                    currentBackground = GetRandomValue(0, 2);
                    currentState = PLAYER_VS_PLAYER;
                }
                else if (IsKeyPressed(KEY_TWO)) {
                    leftPaddle.score = rightPaddle.score = 0;
                    ball.position = (Vector2){screenWidth/2, screenHeight/2};
                    ball.speed = (Vector2){5.0f, 0.0f};
                    vsComputer = true;
                    difficulty = 1;
                    currentBackground = GetRandomValue(0, 2);
                    currentState = PLAYER_VS_COMPUTER;
                }
                break;
            case PLAYER_VS_PLAYER:
            case PLAYER_VS_COMPUTER:
                UpdateGame(leftPaddle, rightPaddle, ball, bounceSound, scoreSound, currentState, difficulty, vsComputer, currentBackground);
                break;
            case GAME_OVER:
                if (IsKeyPressed(KEY_ENTER)) {
                    currentState = MENU;
                }
                break;
        }
        
        DrawGame(leftPaddle, rightPaddle, ball, currentState, vsComputer, backgrounds, currentBackground, dottedLineTexture);
    }

    UnloadSound(bounceSound);
    UnloadSound(scoreSound);
    UnloadTexture(backgrounds[0]);
    UnloadTexture(backgrounds[1]);
    UnloadTexture(backgrounds[2]);
    UnloadRenderTexture(dottedLineTexture);
    CloseAudioDevice();
    CloseWindow();
    return 0;
}