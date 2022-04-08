/**
 * @file renderer.h
 * @brief cnv::Renderer header
 * @see https://github.com/rktrlng/pixelbuffer
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

namespace cnv {

class Renderer
{
public:
	Renderer(int w, int h);
	virtual ~Renderer();

	void renderCanvas(cnv::Canvas* canvas, float px, float py, float sx, float sy, float rot);
	bool displayCanvas(cnv::Canvas* canvas, float px, float py, float sx, float sy, float rot);
	GLFWwindow* window() { return _window; };

	int width() { return _window_width; };
	int height() { return _window_height; };

	float updateDeltaTime();

	void hideMouse();
	void showMouse();

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

} // namespace cnv

#endif /* RENDERER_H */
