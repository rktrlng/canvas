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

int main(int argc, char *argv[])
{
	std::string filename;
	int factor = 1;

	if (argc == 1) {
		std::cout << "Usage: pbfviewer [path] [factor]" << std::endl;
		return 0;
	}
	if (argc == 2) {
		filename = argv[1];
	}
	if (argc == 3) {
		filename = argv[1];
		factor = atoi(argv[2]);
	}

	pb::PixelBuffer pixelbuffer;
	pixelbuffer.read(filename);

	int width = pixelbuffer.header().width;
	int height = pixelbuffer.header().height;

	rt::Renderer renderer = rt::Renderer(width*factor, height*factor);
	rt::Canvas canvas = rt::Canvas(pixelbuffer);

	float timer = 0.0f;
	int frames = 0;
	while (renderer.displayCanvas(&canvas, (width*factor)/2, (width*factor)/2, factor, factor, 0.0f)) {
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
