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

	// Simple extension check first
	if((filename.substr(filename.find_last_of(".") + 1) != "pbf")) {
		std::cout << "Unlikely a valid .pbf file: " << filename << std::endl;
		return 0;
	}

	// Try to open the file from disk
	std::ifstream file(filename, std::fstream::in|std::fstream::binary|std::fstream::ate);
	if (!file.is_open()) {
		std::cout << "Unable to open file: " << filename << std::endl;
		return 0;
	}
	char header[8];
	file.seekg(0, std::fstream::beg);
	file.read(header, 8);
	file.close();
	if (header[0] != 'p' || header[1] != 'b' || header[7] != ':') {
		std::cout << "Not a valid .pbf file: " << filename << std::endl;
		return 0;
	}

	// Sanitize factor
	if (factor < 1 || factor > 64) {
		std::cout << "Illegal factor: " << (int)factor << " (valid 1-64)" << std::endl;
		return 0;
	}

	// read .pbf
	pb::PixelBuffer pixelbuffer;
	pixelbuffer.read(filename);

	int width = pixelbuffer.header().width;
	int height = pixelbuffer.header().height;

	// Create Renderer BEFORE creating a Canvas
	rt::Renderer renderer = rt::Renderer(width*factor, height*factor);
	rt::Canvas canvas = rt::Canvas(pixelbuffer);

	std::cout << filename << " (" << width << "x" << height << ") x " << factor;
	std::cout << " = (" << (width*factor) << "x" << (height*factor) << ")" << std::endl;
	pixelbuffer.printInfo();

	float timer = 0.0f;
	int frames = 0;
	int display = 1; // update title every n seconds
	while (renderer.displayCanvas(&canvas, (width*factor)/2, (height*factor)/2, factor, factor, 0.0f)) {
		if (timer > display) {
			std::string str = "";
			str = filename + "  |  " + std::to_string(width) + "x" + std::to_string(height) + " pixels  |  ";
			str += std::to_string(factor) + "x  |  " + std::to_string(frames/display) + " FPS";
			glfwSetWindowTitle(renderer.window(), str.c_str());

			timer = 0.0f;
			frames = 0;
		}
		frames++;
		timer += renderer.updateDeltaTime();
	}

	return 0;
}
