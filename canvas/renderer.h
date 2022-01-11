/**
 * @file renderer.h
 *
 * @brief rt::Renderer header
 *
 * Copyright 2015-2022 @rktrlng
 * https://github.com/rktrlng/canvas
 */

#ifndef RENDERER_H
#define RENDERER_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/euler_angles.hpp>

#include <canvas/canvas.h>

namespace rt {

class Renderer
{
public:
	Renderer(int w, int h);
	virtual ~Renderer();

	void renderCanvas(rt::Canvas* canvas, float px, float py, float sx, float sy, float rot);
	bool displayCanvas(rt::Canvas* canvas, float px, float py, float sx, float sy, float rot);
	GLFWwindow* window() { return _window; };

	int width() { return _window_width; };
	int height() { return _window_height; };

	float updateDeltaTime();

private:
	int init();

	int _window_width;
	int _window_height;
	GLFWwindow* _window;
	
	GLuint loadShaders();
	GLuint _programID;

	glm::mat4 _projectionMatrix;
	glm::mat4 _viewMatrix;
};

} // namespace rt

#endif /* RENDERER_H */
