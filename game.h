#pragma once
#include <vector>
#include <GLFW/glfw3.h>

/* This handles the objects, input, collision, and helper functions. 
 * If this were to be expanded, I would probably split it up more.
 */

const float DEG2RAD = 3.14159f / 180.0f;

enum BRICKTYPE { REFLECTIVE, DESTRUCTABLE, PADDLE };
enum ONOFF { ON, OFF };

class Brick {
public:
    float red, green, blue;
    float x, y, width, height; // added height
    BRICKTYPE brick_type;
    ONOFF onoff;
    int health;

    // default constructor
    Brick(BRICKTYPE bt, float xx, float yy, float ww, float hh, float rr, float gg, float bb, int hp = 1);
    void DrawBrick() const;
};

class Circle {
public:
    float x, y, radius;
    float red, green, blue;
    float velocityX, velocityY;  // replaced direction with velocity
    bool deleteCircle; // for removing/deleting circles that have merged.

    Circle(float xx, float yy, float rad, float vx, float vy, float r, float g, float b);
    void CheckCollision(Brick* brk);
    void CheckCircleCollision(Circle* other);  // extra addition for circles colliding
    void MoveOneStep();
    void DrawCircle() const;
};

class GameManager {
public:
    std::vector<Circle> balls;
    std::vector<Brick> bricks;
    // input tracking
    bool spaceKeyPress; // only allow 1 spawn per press
    bool rPressed; //had a bug with R spawning too many balls
    bool cPressed;

    // paddle
    int paddleIndex;
    float paddleSpeed;

    //TODO: scoring & life count, lose lives for ball hitting bottom. Score for hitting or breaking bricks. 
    //int score;
    //int lives;

    GameManager();
    void Initialize();
    void Update();
    void Render() const;
    void HandleInput(GLFWwindow* window);
    void SpawnBall();
    void SetupBricks();
    void RemoveDeletedBalls();
};