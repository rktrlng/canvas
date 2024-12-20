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

class MyApp : public cnv::Application
{
public:
	MyApp(uint16_t width, uint16_t height, uint8_t bitdepth, uint8_t factor) : cnv::Application(width, height, bitdepth, factor) 
	{
		std::srand(std::time(nullptr));

		// write to rules/rule000.pbf
		for (size_t i = 0; i < 256; i++)
		{
			rule(i, true);
		}
	}

	// MyApp(rt::PixelBuffer& pixelbuffer, uint8_t factor) : cnv::Application(pixelbuffer, factor)
	// {
	//
	// }

	virtual ~MyApp()
	{

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
	const std::vector<bool> nextRow(const std::vector<bool>& in_row, int rule_num) const
	{
		std::vector<bool> out_row(in_row.size(), 0);
		std::vector<bool> ruleset = ruleBits(rule_num);

		for (size_t x = 1; x < in_row.size()-1; x++) {
			int index = 0;
			if (in_row[x-1] == 1) { index += 4; }
			if (in_row[x+0] == 1) { index += 2; }
			if (in_row[x+1] == 1) { index += 1; }

			uint8_t result = ruleset[index];
			out_row[x] = result;
		}

		return out_row;
	}

	const std::vector<bool> ruleBits(uint8_t rule_num) const
	{
		// the number as an array of bits
		std::vector<bool> ruleset(8, 0);
		int counter = 0;
		while (rule_num > 0)
		{
			ruleset[counter] = rule_num & 1;
			rule_num >>= 1;
			counter++;
		}
		return ruleset;
	}

	void rule(uint8_t num, bool wr = false)
	{
		auto& pixelbuffer = layers[0]->pixelbuffer;
		const size_t rows = pixelbuffer.height();
		const size_t cols = pixelbuffer.width();

		// initialize first row
		std::vector<bool> row(cols, 0);
		row[cols/2-1] = 1; // first pixel half way
		for (size_t i = 0; i < cols; i++)
		{
			row[i] = rand()%2; // random pixels on first row
		}

		// draw all the rows
		for (size_t y = 0; y < rows; y++) {
			for (size_t x = 0; x < cols; x++) {
				rt::RGBAColor color = WHITE;
				if (row[x]) {
					color = BLACK;
				}
				pixelbuffer.setPixel(x, y, color);
			}
			// update row
			row = nextRow(row, num);
		}

		if (wr)
		{
			std::string name = pixelbuffer.createFilename("rules/rule", num, 2);
			std::cout << "writing: " << name << std::endl;
			pixelbuffer.write(name);
		}
	}

	void handleInput() {
		if (input.getKeyDown(cnv::KeyCode::Space)) {
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
