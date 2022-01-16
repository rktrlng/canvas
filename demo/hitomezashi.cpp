/**
 * @file hitomezashi.cpp
 *
 * @brief Hitomezashi Stitching Patterns
 *
 * Copyright 2022 @rktrlng
 * https://github.com/rktrlng/canvas
 */

#include <ctime>
#include <deque>

#include <canvas/application.h>

class MyApp : public rt::Application
{
private:
	const uint8_t step = 5;
	size_t counter = 0;
	std::vector<bool> xstitch;
	std::vector<bool> ystitch;
public:
	MyApp(uint16_t width, uint16_t height, uint8_t bitdepth, uint8_t factor) : rt::Application(width, height, bitdepth, factor)
	{
		std::srand(std::time(nullptr));

		auto& pixelbuffer = layers[0]->pixelbuffer;
		size_t cols = pixelbuffer.header().width;
		size_t rows = pixelbuffer.header().height;
		xstitch = randomSequence(cols/step);
		ystitch = randomSequence(rows/step);

		hitomezashi();
	}

	// MyApp(rt::PixelBuffer& pixelbuffer, uint8_t factor) : rt::Application(pixelbuffer, factor)
	// {
	// 
	// }

	virtual ~MyApp()
	{
		
	}

	void update(float deltatime) override
	{
		handleInput();

		static float frametime = 0.0f;
		float maxtime = 0.0167f - deltatime;
		frametime += deltatime;
		if (frametime >= maxtime) {
			hitomezashi();
			frametime = 0.0f;
		}
	}

private:
	std::vector<bool> randomSequence(size_t amount)
	{
		std::vector<bool> sequence;
		for (size_t i = 0; i < amount; i++) {
			sequence.push_back(rand()%2);
		}
		return sequence;
	}

	void hitomezashi()
	{
		layers[0]->pixelbuffer.fill(WHITE);

		auto& pixelbuffer = layers[0]->pixelbuffer;
		size_t cols = pixelbuffer.header().width;
		size_t rows = pixelbuffer.header().height;

		// horizontal stitches
		int ypos = 0;
		for (size_t y = 0; y < ystitch.size(); y++) {
			int xpos = 0;
			if (ystitch[y]) { xpos += step; }
			for (size_t x = 0; x < cols; x+=step) {
				pixelbuffer.drawLine(xpos+1, ypos, xpos+step-1, ypos, BLACK);
				xpos += step*2;
			}
			ypos += step;
		}

		// vertical stitches
		int xpos = 0;
		for (size_t x = 0; x < xstitch.size(); x++) {
			int ypos = 0;
			if (xstitch[x]) { ypos += step; }
			for (size_t y = 0; y < rows; y+=step) {
				pixelbuffer.drawLine(xpos, ypos+1, xpos, ypos+step-1, BLACK);
				ypos += step*2;
			}
			xpos += step;
		}

		// draw mouse cursor
		int x = (int) input.getMouseX();
		int y = (int) input.getMouseY();

		pixelbuffer.setPixel(x-1, y+0, RED);
		pixelbuffer.setPixel(x+1, y+0, RED);
		pixelbuffer.setPixel(x+0, y-1, RED);
		pixelbuffer.setPixel(x+0, y+1, RED);

		layers[0]->lock();
	}

	void handleInput()
	{
		if (input.getKeyDown(rt::KeyCode::Space)) {
			std::string filename = layers[0]->pixelbuffer.createFilename("hitomezashi", counter);
			std::cout << filename << std::endl;
			layers[0]->pixelbuffer.write(filename);
			counter++;
		}

		if (input.getKeyDown(rt::KeyCode::R)) {
			xstitch = randomSequence(layers[0]->pixelbuffer.header().width / step);
			ystitch = randomSequence(layers[0]->pixelbuffer.header().height / step);
		}

		if (input.getMouseDown(0)) {
			int x = (int) input.getMouseX();
			int y = (int) input.getMouseY();

			if (x%step == 0) { // column
				size_t index = x/step;
				xstitch[index] = !xstitch[index];
			}

			if (y%step == 0) { // row
				size_t index = y/step;
				ystitch[index] = !ystitch[index];
			}
		}

		int scrolly = input.getScrollY();
		if (scrolly != 0) {
			std::cout << "scroll: " << scrolly << std::endl;
		}
	}

};


int main( void )
{
	MyApp application(320, 180, 8, 4);

	while (!application.quit())
	{
		application.run();
	}

	return 0;
}
