/**
 * @file application.h
 *
 * @brief cnv::Application header
 *
 * Copyright 2021-2022 @rktrlng
 * https://github.com/rktrlng/canvas
 */

#ifndef APPLICATION_H
#define APPLICATION_H

#include <vector>

#include <canvas/renderer.h>
#include <canvas/input.h>
#include <canvas/canvas.h>

namespace cnv {

class Application
{
public:
	Application(uint16_t width, uint16_t height, uint8_t bitdepth = 24, uint8_t factor = 1);
	Application(rt::PixelBuffer& pixelbuffer, uint8_t factor = 1, bool setlocked = false);
	virtual ~Application();

	int quit();
	int run();
	virtual void update(float deltatime) = 0;

	void hideMouse() { renderer.hideMouse(); }
	void showMouse() { renderer.showMouse(); }

private:
	Renderer renderer;

protected:
	Input input;
	std::vector<Canvas*> layers;
};

#endif // APPLICATION_H

} // namespace cnv
