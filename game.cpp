#include "game.h"
#include <cmath>
#include <algorithm>
#include <iostream>

//helper function in an unnamed namespace (static/internal)
namespace 
{
    float randomFloat(float min, float max)
    {
        return min + (max - min) * ((float)rand() / RAND_MAX);
    }
}

// Brick constructor setup
Brick::Brick(BRICKTYPE bt, float xx, float yy, float ww, float hh, float rr, float gg, float bb, int hp)
{
	brick_type = bt;
	x = xx;
	y = yy;
	width = ww;
    height = hh;
	red = rr;
	green = gg;
	blue = bb;
	onoff = ON;
	health = hp;
}

// draw brick structure

void Brick::DrawBrick() const
{
    if (onoff == ON) 
    {
        double halfW = width * 0.5;
        double halfH = height * 0.5;
        //darken color based on damage
        float colorMulti = 1.0f;
        if (brick_type == DESTRUCTABLE && health < 3)
            colorMulti = 0.4f + (0.3f * health);

        glColor3d(red * colorMulti, green * colorMulti, blue * colorMulti);
        glBegin(GL_POLYGON);
        glVertex2d(x + halfW, y + halfH);
        glVertex2d(x + halfW, y - halfH);
        glVertex2d(x - halfW, y - halfH);
        glVertex2d(x - halfW, y + halfH);
        glEnd();
    }
}

// Circle constructor
Circle::Circle(float xx, float yy, float rad, float vx, float vy, float r, float g, float b)
{
	x = xx;
	y = yy;
	radius = rad;
	velocityX = vx;
	velocityY = vy;
	red = r;
	green = g;
	blue = b;
    deleteCircle = false;
}
// check collision between circle & brick
void Circle::CheckCollision(Brick* brk)
{
    if (brk->onoff == OFF) return;
    // collision detection to check if edge of circle is touching brick
    float halfW = brk->width * 0.5f;
    float halfH = brk->height * 0.5f;
    // check if circle overlaps with brick
    if ((x + radius > brk->x - halfW && x - radius < brk->x + halfW) &&
        (y + radius > brk->y - halfH && y - radius < brk->y + halfH))
    {
        // find the side its hit from
        float dx = x - brk->x;
        float dy = y - brk->y;

        // bounce based on which side is closer
        if (std::abs(dx) / halfW > std::abs(dy) / halfH)
        {
            velocityX = -velocityX;
            if (dx > 0)
            {
                x = brk->x + halfW + radius;
            }
            else
            {
                x = brk->x - halfW - radius;
            }
        }
        else
        {
            velocityY = -velocityY;
            // prevent sticking
            if (dy > 0)
            {
                y = brk->y + halfH + radius;
            }
            else
            {
                y = brk->y - halfH - radius;
            }
        }

        // brick state changes
        if (brk->brick_type == DESTRUCTABLE) {
            brk->health--; 
            if (brk->health <= 0)
                brk->onoff = OFF;
        }

        // paddle
        if (brk->brick_type == PADDLE) {
            // calculate position from -1 (left edge) to +1 (right edge)
            float padPos = (x - brk->x) / halfW;
            // clamp to valid range
            padPos = std::max(-1.0f, std::min(1.0f, padPos));
            // adjust x velocity based on paddle hit position
            velocityX = padPos * 0.03f; // max horizontal speed
            // ensure the ball goes up
            velocityY = std::abs(velocityY);
        }
    }
}


void Circle::CheckCircleCollision(Circle* other)
{
    if (this == other || deleteCircle || other->deleteCircle) return; // this == other unnecessary

    //float dist = sqrt(pow(x - other->x, 2) + pow(y - other->y, 2));
    float dx = x - other->x;
    float dy = y - other->y;
    float dist = std::sqrt(dx * dx + dy * dy);

    if (dist < radius + other->radius)
    {
	    // merge circles, bigger one absorbs smaller ones
        if (radius >= other->radius)
        {
            //calculate new radius from combined area
            float combinedCircle = (radius * radius) + (other->radius * other->radius);
            radius = std::sqrt(combinedCircle);
            // limiting the max size
            //if (radius > 0.15f) radius = 0.15f;
            radius = std::min(radius, 0.15f); //same thing
            // blending colors
            red = (red + other->red) * 0.5f;
            green = (green + other->green) * 0.5f;
            blue = (blue + other->blue) * 0.5f;
            //Average velocities weighted by size
            float weight = other->radius / (radius + other->radius);
            velocityX = velocityX * (1 - weight) + other->velocityX * weight;
            velocityY = velocityY * (1 - weight) + other->velocityY * weight;
            //delete smaller circle
            other->deleteCircle = true;
        }
    }
}

// move the circle one frame & slow down velocity
void Circle::MoveOneStep()
{
	x += velocityX;
	y += velocityY;
	// bounce off right wall
    if (x > 1 - radius) {
        x = 1 - radius;
        velocityX = -std::abs(velocityX) * 0.85f; // go left
    }
    // bounce off left wall
    if (x < -1 + radius) {
        x = -1 + radius;
        velocityX = std::abs(velocityX) * 0.85f; // go right
    }
    // bounce off top wall
    if (y > 1 - radius) {
        y = 1 - radius;
        velocityY = -std::abs(velocityY) * 0.85f; // go down
    }
    // bounce off bottom wall
    if (y < -1 + radius) {
        y = -1 + radius;
        velocityY = std::abs(velocityY) * 0.85f; // go up
    }
}

// draw the circle
void Circle::DrawCircle() const
{
    glColor3f(red, green, blue);
    glBegin(GL_POLYGON);
    for (int i = 0; i < 360; i++)
    {
        float degInRad = i * DEG2RAD;
        glVertex2f((cos(degInRad) * radius) + x, (sin(degInRad) * radius) + y);
    }
    glEnd();
}

// gamemanager default constructor
GameManager::GameManager()
{
    spaceKeyPress = false;
    rPressed = false;
    cPressed = false;
    paddleIndex = -1;
    paddleSpeed = 0.025f;
}

/* Clear the balls & bricks
 * Setup the bricks for the screen (their position)
 * Spawnball starts with one ball.
 */
void GameManager::Initialize()
{
    balls.clear(); 
    bricks.clear();
    SetupBricks();
    SpawnBall(); 
}


void GameManager::SetupBricks()
{
	// breakout brick layout
    for (int row = 0; row < 6; row++) {
        for (int col = 0; col < 10; col++) {
            float x = -0.85f + col * 0.19f;
            float y = 0.7f - row * 0.1f;

            // set color and health based on row
            float colors[3][3] = { {0.9f, 0.2f, 0.2f}, {0.2f, 0.9f, 0.2f}, {0.2f,0.5f,0.9f} };
            int colorIndex = row / 2; // 0,0,1,1,2,2
            int hp = 3 - colorIndex; // 3,3,2,2,1,1

            //bricks.push_back(Brick(DESTRUCTABLE, x, y, brickWidth, brickHeight, r, g, b, hp));
            bricks.emplace_back(DESTRUCTABLE, x, y, 0.17f, 0.08f,colors[colorIndex][0], colors[colorIndex][1],
                colors[colorIndex][2], hp);
        }
    }

    // paddle at bottom
    //bricks.push_back(Brick(PADDLE, 0.0f, -0.85f, 0.3f, 0.05f, 1.0f, 1.0f, 1.0f, 1));
    bricks.emplace_back(PADDLE, 0.0f, -0.85f, 0.3f, 0.05f, 1.0f, 1.0f, 1.0f, 1);
    paddleIndex = bricks.size() - 1;
}

// update game state
void GameManager::Update()
{
    //move all balls & check collisions
    for (size_t i = 0; i < balls.size(); i++) {
        balls[i].MoveOneStep();

        //check brick collisions
        for (size_t j = 0; j < bricks.size(); j++) {
            balls[i].CheckCollision(&bricks[j]);
        }

        //check ball collisions
        for (size_t j = i + 1; j < balls.size(); j++) {
            balls[i].CheckCircleCollision(&balls[j]);
        }
    }
    //Remove balls that were merged/absorbed in collisions
    RemoveDeletedBalls();
}

// Remove balls that have merged
void GameManager::RemoveDeletedBalls()
{
    // backwards iterator to remove elements
    for (int i = static_cast<int>(balls.size()) - 1; i >= 0; i--) 
    {
	    if (balls[i].deleteCircle)
	    {
            balls.erase(balls.begin() + i);
	    }
    }
}

// render all game objects
void GameManager::Render() const
{
	// drawing all bricks
    for (size_t i = 0; i < bricks.size(); i++)
        bricks[i].DrawBrick();
    // drawing balls
    for (size_t i = 0; i < balls.size(); i++)
        balls[i].DrawCircle();
}
// new ball spawned at center of screen
void GameManager::SpawnBall()
{
    // random velocities for the balls
    float vx = randomFloat(-0.02f, 0.02f);
    float vy = randomFloat(0.01f, 0.02f); // always starts going up direction.
    // minimum speed, if x velocity is too small
    if (std::abs(vx) < 0.01f)
    {
        if (vx >= 0)
        {
            vx = 0.01f;
        }
        else
        {
            vx = -0.01f;
        }
    }
    // random colors
    float r = randomFloat(0.3f, 1.0f);
    float g = randomFloat(0.3f, 1.0f);
    float b = randomFloat(0.3f, 1.0f);

    //balls.push_back(Circle(0, 0, 0.04f, vx, vy, r, g, b));
    balls.emplace_back(0, 0, 0.04f, vx, vy, r, g, b);
}

//keyboard input
void GameManager::HandleInput(GLFWwindow* window)
{
    // exit game with escape key
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    // spawn ball with space key
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
    {
        if (!spaceKeyPress) {
            SpawnBall();
            spaceKeyPress = true;
        }
    }
    else
        spaceKeyPress = false;

    // reset game with R
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
    {
	    if (!rPressed)
	    {
            Initialize();
            rPressed = true;
	    }
    } else
        rPressed = false;
    // clear balls with C
    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
    {
	    if (!cPressed)
	    {
            balls.clear();
            cPressed = true;
	    }
    } else
        cPressed = false;

    // paddle movement (hold to move)
    if (paddleIndex >= 0 && paddleIndex < bricks.size())
    {
        Brick& paddle = bricks[paddleIndex];
        // left arrow key or A to decrease x (go left)
        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        {
            paddle.x -= paddleSpeed;
        }
        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        {
            paddle.x += paddleSpeed;
        }
        // clamp paddle inside walls
        float limit = 1.0f - paddle.width * 0.5f;
        paddle.x = std::max(-limit, std::min(limit, paddle.x));
    }
}