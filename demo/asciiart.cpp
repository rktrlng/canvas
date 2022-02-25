#include <iostream>
#include <pixelbuffer/pixelbuffer.h>

int main(int argc, char *argv[])
{
	std::string filename = "testpattern.pbf";

	if (argc == 1) {
		std::cout << "Usage: asciiart [path]" << std::endl;
	}
	if (argc == 2) {
		filename = argv[1];
	}

	pb::PixelBuffer pb;
	pb.read(filename);

	std::string characters = " .-:+=*%#@";

	// std::string reversed = std::string(characters.rbegin(),characters.rend());
	std::string ramp = characters;

	size_t step = ramp.size()-1;

	uint16_t cols = pb.header().width;
	uint16_t rows = pb.header().height;

	for (size_t y = 0; y < rows; y++) {
		for (size_t x = 0; x < cols; x++) {
			pb::RGBAColor color = pb.getPixel(x, y);
			int avg = (color.r + color.g + color.b) / 3;
			size_t index = pb::map(avg, 0, 255, 0, step);
			std::cout << ramp[index];
		}
		std::cout << std::endl;
	}

	return 0;
}
