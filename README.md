# Breakout-Game
This was built as part of our week 8 assignment for graphics programming. It's intended functionality will be as a breakout style game, where you move a paddle to prevent the balls from hitting the bottom, otherwise the player loses a life. And score is gained by hitting and breaking the bricks. 

<img width="400" height="400" alt="image" src="https://github.com/user-attachments/assets/92fcf15b-4d33-4112-bb78-5d3ec6dc510c" />


## Features
* **Bricks**: Represented as either reflective or destructible. Reflective bricks change the direction of the balls upon collision, while destructible bricks can take one or multiple hits, with visual damage indicators.
* **Balls**: Circular objects that move randomly within the window using physics. Their motion is influenced by collisions with bricks and window boundaries. Balls can collide and absorb one another, growing in size.
* **Input Handling**:
  * Spacebar: spawn new ball at the center.
  * ESC: exit the game.
  * R: Reset the game.
  * C: clear all balls on screen.
  * A/LEFT Arrow: Move paddle left.
  * D/Right Arrow: Move paddle right.

### TODO
* Add scoring and lives. ~3 lives & 100 score for breaking a brick.
* Everytime the ball hits the bottom, lose a life.
* Every certain amount of hits/bricks broken, spawn a new ball.
* Every certain amount of points, gain a new life?
* Add Game Features console print.
* Textures & lighting.
* imgui for game engine adaptation?
* Apply more features from learnopengl.
* Layout procedural generation for levels?
* Visual indicator for score / lives

## Dependencies
* GLFW for window creation and input handling.
* OpenGL for rendering graphics.
