/**
 * @file pbfviewer.cpp
 *
 * @brief pbf viewer
 *
 * Copyright 2021-2022 @rktrlng
 * https://github.com/rktrlng/canvas
 */

#include <ctime>
#include <string>

#include <canvas/renderer.h>

int main(void)
{
	int width = 768;
	int height = 768;

	rt::Renderer renderer = rt::Renderer(width, height);
	rt::Canvas canvas = rt::Canvas("rules/rule105.pbf");

	int cols = canvas.pixelbuffer.header().width;
	int rows = canvas.pixelbuffer.header().height;
	float fx = (float) width / cols;
	float fy = (float) height / rows;

	float timer = 0.0f;
	int frames = 0;
	while (renderer.displayCanvas(&canvas, width/2, height/2, fx, fy, 0.0f)) {
		timer += renderer.updateDeltaTime();
		frames++;
		if (timer > 5.0f) {
			std::cout << frames/5 << " fps" << std::endl;
			timer = 0.0f;
			frames = 0;
		}
	}

	return 0;
}
