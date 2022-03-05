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

class MyApp : public cnv::Application
{
private:
	std::deque<float> m_values;
public:
	MyApp(uint16_t width, uint16_t height, uint8_t bitdepth, uint8_t factor) : cnv::Application(width, height, bitdepth, factor)
	{
		std::srand(std::time(nullptr));
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

		static float frametime = 0.0f;
		float maxtime = 0.01667f - deltatime;
		frametime += deltatime;
		if (frametime >= maxtime) {
			static int harmonic = 1;
			static int harmonic_vel = 1;
			static int counter = 0;
			counter++;
			if (counter > 250) {
				harmonic += harmonic_vel;
				counter = 0;
			}
			if (harmonic > 6 || harmonic < 2) {
				harmonic_vel *= -1;
			}
			updatePixels(harmonic);
			frametime = 0.0f;
		}
	}

private:
	float square_wave(float t, int harmonics = 3)
	{
		// https://www.mathworks.com/help/matlab/math/square-wave-from-sine-waves.html
		// y = sin(t) + sin(3*t)/3 + sin(5*t)/5 + sin(7*t)/7 + sin(9*t)/9;
		float y = 0.0f;
		for (int i = 1; i < harmonics*2; i += 2) {
			y += sin(i*t)/i;
		}
		return y;
	}

	void updatePixels(int harmonics)
	{
		auto& pixelbuffer = layers[0]->pixelbuffer;
		size_t rows = pixelbuffer.header().height;
		size_t cols = pixelbuffer.header().width;

		pixelbuffer.fill(TRANSPARENT);
		size_t stop = cols-30;

		// ##############################################
		// calculate wave form
		static float t = 0;
		t += 0.075f;
		float y = square_wave(t, harmonics);

		m_values.push_front(y);
		if (m_values.size() > stop) {
			m_values.pop_back();
		}

		// ##############################################
		// draw wave
		int scale = 60;

		for (size_t i = 0; i < m_values.size(); i++) {
			if (i > 1) {
				int x = stop-i;
				pixelbuffer.drawLine(x, (m_values[i-1]*scale)+rows/2, x-1, (m_values[i]*scale)+rows/2, GREEN);
			}
		}

		// ##############################################
		// draw red circle
		pixelbuffer.drawCircleFilled(stop, (m_values[0]*scale)+rows/2, 3, RED);

		// ##############################################
		// draw grid
		int gridsize = 10;
		int doublegrid = 20;
		uint8_t light = 192;
		uint8_t dark = 127;
		uint8_t alpha = 63;

		for (size_t y = 0; y < rows; y += gridsize) {
			if (y%doublegrid == 0) {
				pixelbuffer.drawLine(0, y, cols, y, rt::RGBAColor(light, alpha));
			} else {
				pixelbuffer.drawLine(0, y, cols, y, rt::RGBAColor(dark, alpha));
			}
		}
		for (size_t x = 0; x < cols; x += gridsize) {
			if (x%doublegrid == 0) {
				pixelbuffer.drawLine(x, 0, x, rows, rt::RGBAColor(light, alpha));
			} else {
				pixelbuffer.drawLine(x, 0, x, rows, rt::RGBAColor(dark, alpha));
			}
		}
		pixelbuffer.drawLine(cols-1, 0, cols-1, rows-1, rt::RGBAColor(light, alpha));
		pixelbuffer.drawLine(0, rows-1, cols-1, rows-1, rt::RGBAColor(light, alpha));

		// ##############################################
		layers[0]->lock();
	}

	void handleInput()
	{
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
	MyApp application(640, 360, 32, 2);

	while (!application.quit())
	{
		application.run();
	}

	return 0;
}
