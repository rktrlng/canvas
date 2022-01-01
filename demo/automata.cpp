/**
 * @file automata.cpp
 *
 * @brief Wolfram Elementary Cellular Automaton implementation
 *
 * Copyright 2021-2022 @rktrlng
 * https://github.com/rktrlng/canvas
 */

#include <ctime>
#include <string>

#include <canvas/application.h>

class MyApp : public rt::Application
{
public:
	MyApp(uint16_t width, uint16_t height, uint8_t bitdepth, uint8_t factor) : rt::Application(width, height, bitdepth, factor) 
	{
		std::srand(std::time(nullptr));

		// write to rules/rule000.pbf
		for (size_t i = 0; i < 256; i++)
		{
			rule(i, true);
		}
	}

	// MyApp(rt::PixelBuffer& pixelbuffer, uint8_t factor) : rt::Application(pixelbuffer, factor)
	// {
	//
	// }

	virtual ~MyApp()
	{
		for (auto canvas : layers) {
			delete canvas;
		}
		layers.clear();
	}

	void update(float deltatime) override
	{
		handleInput();
		
		float maxtime = 1.5f;
		static float frametime = maxtime;
		frametime += deltatime;
		if (frametime >= maxtime)
		{
			static int r = 0;
			std::cout << "rule set: " << r << std::endl;
			rule(r);
			r++;
			r %= 256;
			layers[0]->lock();

			frametime = 0.0f;
		}
	}

private:
	void rule(int num, bool wr = false)
	{
		auto& pixelbuffer = layers[0]->pixelbuffer;

		const size_t rows = pixelbuffer.header().height;
		const size_t cols = pixelbuffer.header().width;

		std::vector<uint8_t> ruleset(8, 0);
		int counter = 0;
		int rn = num;
		while (rn > 0)
		{
			ruleset[counter] = rn & 1;
			rn >>= 1;
			counter++;
		}

		std::vector<uint8_t> row(cols, 0);
		row[cols/2-1] = 1; // first pixel half way
		for (size_t i = 0; i < cols; i++)
		{
			row[i] = rand()%2; // random pixels on first row
		}

		// draw first row
		for (size_t x = 1; x < cols-1; x++) {
			pb::RGBAColor color = WHITE;
			if (row[x] == 1) {
				color = BLACK;
			}
			pixelbuffer.setPixel(x, 0, color);
		}

		// draw the rest of the owl
		for (size_t y = 1; y < rows; y++) {
			std::vector<uint8_t> next(cols, 0);
			for (size_t x = 1; x < cols-1; x++) {
				int index = 0;
				if (row[x-1] == 1) { index += 4; }
				if (row[x+0] == 1) { index += 2; }
				if (row[x+1] == 1) { index += 1; }

				uint8_t result = ruleset[index];
				next[x] = result;

				pb::RGBAColor color = WHITE;
				if (result) {
					color = BLACK;
				}
				pixelbuffer.setPixel(x, y, color);
			}
			row = next;
		}

		if (wr)
		{
			std::string name = pixelbuffer.createFilename("rules/rule", num, 2);
			std::cout << "writing: " << name << std::endl;
			pixelbuffer.write(name);
		}
	}

	void handleInput() {
		if (input.getKeyDown(rt::KeyCode::Space)) {
			std::cout << "spacebar pressed down." << std::endl;
			layers[0]->pixelbuffer.printInfo();
		}

		if (input.getMouseDown(0)) {
			std::cout << "click " << (int) input.getMouseX() << "," << (int) input.getMouseY() << std::endl;
		}

		int scrolly = input.getScrollY();
		if (scrolly != 0) {
			std::cout << "scroll: " << scrolly << std::endl;
		}
	}

};


int main( void )
{
	MyApp application(128, 128, 8, 7);

	while (!application.quit())
	{
		application.run();
	}

	return 0;
}
