#include <common/application.h>

namespace rt {

Application::Application(uint16_t width, uint16_t height, uint8_t factor) :
	factor(factor),
	renderer({width*factor, height*factor}),
	input({renderer.window()})
{

}

Application::Application(PixelBuffer& pixelbuffer, uint8_t factor) :
	factor(factor),
	renderer({pixelbuffer.header().width * factor, pixelbuffer.header().height * factor}),
	input({renderer.window()})
{

}

Application::~Application()
{
	std::cout << "Application done. Thank you." << std::endl;
}

int Application::run()
{
	if (glfwGetKey(renderer.window(), GLFW_KEY_ESCAPE ) == GLFW_PRESS ||
		glfwWindowShouldClose(renderer.window()) )
	{
		glfwTerminate();
		return 0;
	}

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
			str = "Canvas: " + std::to_string(cols) + "x" + std::to_string(rows) + " pixels    |    " + std::to_string(factor) + "x    |    " + std::to_string(frames) + " FPS";
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
	// Compute the ViewMatrix from keyboard and mouse input
	renderer.computeViewMatrixFromInputs(deltaTime);

	// Clear the screen
	glClear(GL_COLOR_BUFFER_BIT);

	// Render all layers (Canvas*, xpos, ypos, xscale, yscale, rotation)
	for (auto canvas : layers)
	{
		float factor = renderer.width() / canvas->width();
		// float factor_y = renderer->height() / canvas->height(); // don't factor in height; keep pixels square.
		renderer.renderCanvas(canvas, renderer.width()/2, renderer.height()/2, factor, factor, 0.0f);
	}
	
	// Swap buffers
	glfwSwapBuffers(renderer.window());
	// glfwPollEvents(); // we do this in input.updateInput()

	return 1;
}



} // namespace rt
