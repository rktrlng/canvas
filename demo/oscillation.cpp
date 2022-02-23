/**
 * @file oscillation.cpp
 *
 * @brief oscillation app
 *
 * Copyright 2021-2022 @rktrlng
 * https://github.com/rktrlng/canvas
 */

#include <ctime>
#include <deque>

#include <canvas/application.h>

class MyApp : public rt::Application
{
private:
	std::deque<float> values;
public:
	MyApp(uint16_t width, uint16_t height, uint8_t bitdepth, uint8_t factor) : rt::Application(width, height, bitdepth, factor)
	{
		std::srand(std::time(nullptr));
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
		float maxtime = 0.01667f - deltatime;
		frametime += deltatime;
		if (frametime >= maxtime) {
			updatePixels();
			frametime = 0.0f;
		}
	}

private:
	float square_wave(float t, int octaves = 3)
	{
		// https://www.mathworks.com/help/matlab/math/square-wave-from-sine-waves.html
		// y = sin(t) + sin(3*t)/3 + sin(5*t)/5 + sin(7*t)/7 + sin(9*t)/9;
		float y = 0.0f;
		for (int i = 1; i < octaves*2; i += 2) {
			y += sin(i*t)/i;
		}
		return y;
	}

	void updatePixels()
	{
		auto& pixelbuffer = layers[0]->pixelbuffer;
		size_t rows = pixelbuffer.header().height;
		size_t cols = pixelbuffer.header().width;

		pixelbuffer.fill(TRANSPARENT);

		// ##############################################
		// calculate wave form
		static float t = 0;
		t += 0.075f;
		float y = square_wave(t, 6);

		values.push_back(y);
		if (values.size() > cols) {
			values.pop_front();
		}

		// ##############################################
		// draw wave
		int scale = 60;

		for (size_t x = 0; x < values.size(); x++) {
			if (x > 1) {
				pixelbuffer.drawLine(x, (values[x]*scale)+rows/2, x-1, (values[x-1]*scale)+rows/2, GREEN);
			}
		}

		// ##############################################
		// draw grid
		int gridsize = 10;
		int doublegrid = 20;
		uint8_t light = 192;
		uint8_t dark = 127;
		uint8_t alpha = 63;

		for (size_t y = 0; y < rows; y += gridsize) {
			if (y%doublegrid == 0) {
				pixelbuffer.drawLine(0, y, cols, y, pb::RGBAColor(light, alpha));
			} else {
				pixelbuffer.drawLine(0, y, cols, y, pb::RGBAColor(dark, alpha));
			}
		}
		for (size_t x = 0; x < cols; x += gridsize) {
			if (x%doublegrid == 0) {
				pixelbuffer.drawLine(x, 0, x, rows, pb::RGBAColor(light, alpha));
			} else {
				pixelbuffer.drawLine(x, 0, x, rows, pb::RGBAColor(dark, alpha));
			}
		}
		pixelbuffer.drawLine(cols-1, 0, cols-1, rows-1, pb::RGBAColor(light, alpha));
		pixelbuffer.drawLine(0, rows-1, cols-1, rows-1, pb::RGBAColor(light, alpha));

		// ##############################################
		// lock()
		layers[0]->lock();
	}

	void handleInput()
	{
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
	MyApp application(640, 360, 32, 2);

	while (!application.quit())
	{
		application.run();
	}

	return 0;
}
