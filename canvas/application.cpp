/**
 * @file application.cpp
 *
 * @brief rt::Application implementation
 *
 * Copyright 2021-2022 @rktrlng
 * https://github.com/rktrlng/canvas
 */

#include <canvas/application.h>

namespace rt {

Application::Application(uint16_t width, uint16_t height, uint8_t bitdepth, uint8_t factor) :
	renderer(width*factor, height*factor),
	input(renderer.window())
{
	layers.push_back( new rt::Canvas(width, height, bitdepth, factor) );
}

Application::Application(pb::PixelBuffer& pixelbuffer, uint8_t factor, bool setlocked /* false */) :
	renderer(pixelbuffer.header().width * factor, pixelbuffer.header().height * factor),
	input(renderer.window())
{
	uint16_t cols = pixelbuffer.header().width;
	uint16_t rows = pixelbuffer.header().height;
	layers.push_back( new rt::Canvas(cols, rows, pixelbuffer.header().bitdepth, factor) );
	layers[0]->pixelbuffer = pixelbuffer;
	if (setlocked)
	{
		layers[0]->lock();
	}
}

Application::~Application()
{
	for (auto canvas : layers) {
		delete canvas;
	}
	layers.clear();

	std::cout << "Application done. Thank you." << std::endl;
}

int Application::quit()
{
	if (glfwGetKey(renderer.window(), GLFW_KEY_ESCAPE ) == GLFW_PRESS ||
		glfwWindowShouldClose(renderer.window()) )
	{
		glfwTerminate();
		return 1;
	}

	return 0;
}

int Application::run()
{
	// Update deltaTime
	float deltaTime = renderer.updateDeltaTime();

	size_t cols = 0;
	size_t rows = 0;
	if (layers.size() > 0) {
		cols = layers[0]->width();
		rows = layers[0]->height();
	}

	// #########################################
	// update window title every second
	static float frametime = 0.0f;
	static int frames = 0;
	float maxtime = 1.0f;
	frametime += deltaTime;
	if (frametime > maxtime)
	{
		std::string str = "";
		if (layers.size() > 0) {
			str = "Canvas: " + std::to_string(cols) + "x" + std::to_string(rows);
			str += " pixels    |    " + std::to_string(layers[0]->scale);
			str + "x    |    " + std::to_string(frames) + " FPS";
		} else {
			str = std::string(std::to_string(frames) + " FPS");
		}
		glfwSetWindowTitle(renderer.window(), str.c_str());
		frametime = 0.0f;
		frames = 0;
	}
	frames++;
	// #########################################

	input.updateInput(cols, rows);
	// update user application
	this->update(deltaTime);

	// Clear the screen
	glClear(GL_COLOR_BUFFER_BIT);

	// Render all layers (Canvas*, xpos, ypos, xscale, yscale, rotation)
	for (auto& canvas : layers)
	{
		// float factor = renderer.width() / canvas->width();
		// float factor_y = renderer->height() / canvas->height(); // don't factor in height; keep pixels square.
		// renderer.renderCanvas(canvas, renderer.width()/2, renderer.height()/2, factor, factor, 0.0f);
		renderer.renderCanvas(canvas, renderer.width()/2 + canvas->position.x, renderer.height()/2 + canvas->position.y, canvas->scale, canvas->scale, 0.0f);
	}
	
	// Swap buffers
	glfwSwapBuffers(renderer.window());
	// glfwPollEvents(); // we do this in input.updateInput()

	return 1;
}



} // namespace rt
