///////////////////////////////////////////////////////////////////////////////
// 2D Game Program
// ===============
// This program implements a simple 2D game using OpenGL and GLFW. The game
// involves circular objects (referred to as "balls") that move within a window 
// and interact with static rectangular bricks. The primary features of the game
// include:
// 
// - **Bricks**: Represented as either reflective or destructible. Reflective 
//   bricks change the direction of the balls upon collision, while destructible
//   bricks disappear when hit.
// - **Balls**: Circular objects that move randomly within the window. Their 
//   motion is influenced by collisions with bricks and window boundaries.
// - **Input Handling**: Players can spawn new balls by pressing the spacebar or 
//   close the game window by pressing the ESC key.
//
// Key Features:
// -------------
// - **Brick Types**:
//   - Reflective bricks alter the direction of the balls.
//   - Destructible bricks disappear upon collision.
// - **Ball Motion**:
//   - Balls move in random directions and bounce upon hitting boundaries.
//   - Collisions with bricks are handled dynamically.
// - **User Input**:
//   - Spacebar spawns a new ball at the center with random colors.
//   - ESC closes the application.
// 
// Dependencies:
// -------------
// - GLFW for window creation and input handling.
// - OpenGL for rendering graphics.
//
// Enhancements:
// -------------
// - Multi hit destructible bricks with visual damage indicators
// - Reflective bricks that bounce balls
// - Physics based ball movement
// - Ball collisions with merging behavior
// - Organized brick layouts to mimic breakout style game
//
// Controls:
// - SPACE: Spawn a new ball at the center
// - ESC: Exit the game
// - R: Reset the game
// - C: Clear all balls
// - A/LEFT: Move paddle left
// - D/RIGHT: Move paddle right
// 
// Author: Matthew Hall
// Date: 8/23/25
// Assignment: 8-2 Assignment - CS-330 Computational Graphics and Visualization
///////////////////////////////////////////////////////////////////////////////

#include <GLFW\glfw3.h>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include "../game.h"

using namespace std;

// global game manager
GameManager game;

// prototype for framebuffer function
void FramebufferSizeCallback(GLFWwindow* window, int width, int height);

int main(void)
{
	srand(time(NULL));
	//srand(static_cast<unsigned int>(time(nullptr)));

	if (!glfwInit())
		exit(EXIT_FAILURE); // not thread safe

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	glfwWindowHint(GLFW_SAMPLES, 4); // testing out antialiasing 

	GLFWwindow* window = glfwCreateWindow(800, 800, "Matthew's 8-2 Assignment", nullptr, nullptr);
	if (!window)
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);
	// framebuffer size callback
	glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);
	// anti-aliasing
	glEnable(GL_LINE_SMOOTH); //smoother lines with antialiasing
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST); //target, mode
	//background color rgba
	glClearColor(0.1f,0.1f,0.2f,1.0f);
	//initialize the game
	game.Initialize();
	//print game controls & info
	cout << "========== BREAKOUT GAME ==========\n";
	cout << "******* Game Controls: *******\n";
	cout << "  SPACE - Spawn new ball\n";
	cout << "  R - Reset the game\n";
	cout << "  C - Clear all balls\n";
	cout << "  A/LEFT - Move paddle left\n";
	cout << "  D/RIGHT - Move paddle right\n";
	cout << "  ESC - Exit game\n";
	cout << "===================================\n";
	//cout << "******* Game Features: *******\n";
	// add info about scoring, add functionality for hitting the bottom = points loss, destroying a cube = points?

	//clear screen, handle input, update game state, render game
	while (!glfwWindowShouldClose(window))
	{
		glClear(GL_COLOR_BUFFER_BIT);
		game.HandleInput(window);
		game.Update();
		game.Render();
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	//cleanup
	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);
}

//window resizing
void FramebufferSizeCallback(GLFWwindow* window, int width, int height)
{
	// this updates the viewport to match the new screen size
	glViewport(0,0,width,height);
	// maintain aspect ratio
	float aspect = static_cast<float>(width) / static_cast<float>(height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if (aspect > 1.0f)
	{
		// window is wider than it is tall
		glOrtho(-aspect, aspect, -1.0,1.0,-1.0,1.0);
	}
	else
	{
		//window is taller than it is wide
		glOrtho(-1.0, 1.0, -1.0 / aspect, 1.0 / aspect, -1.0, 1.0);
	}
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}
