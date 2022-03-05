/**
 * @file blur.cpp
 *
 * @brief Firefly maybe
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
	rt::RGBAColor color = RED;
public:
	MyApp(uint16_t width, uint16_t height, uint8_t bitdepth, uint8_t factor) : cnv::Application(width, height, bitdepth, factor)
	{
		std::srand(std::time(nullptr));
		layers[0]->pixelbuffer.fill(BLACK);
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
			updatePixels();
			color = rt::rotate(color, 0.005f);
			frametime = 0.0f;
		}
	}

private:
	void updatePixels()
	{
		auto& pixelbuffer = layers[0]->pixelbuffer;
		size_t cols = pixelbuffer.header().width;
		size_t rows = pixelbuffer.header().height;

		static float angle = 0.0f;
		rt::vec2f pos = rt::vec2f(16, 0);
		pos.rotate(angle);
		angle += 0.05f;

		drawCross(pos.x + cols/2, pos.y + rows/2, color);

		pixelbuffer.blur();
		layers[0]->lock();
	}

	void drawCross(int x, int y, rt::RGBAColor color)
	{
		auto& pixelbuffer = layers[0]->pixelbuffer;

		pixelbuffer.setPixel(x,     y, color);
		pixelbuffer.setPixel(x + 1, y, color);
		pixelbuffer.setPixel(x - 1, y, color);
		pixelbuffer.setPixel(x,     y + 1, color);
		pixelbuffer.setPixel(x,     y - 1, color);
	}

	void handleInput()
	{
		if (input.getKeyDown(cnv::KeyCode::Space)) {
			std::cout << "spacebar pressed down." << std::endl;
			layers[0]->pixelbuffer.printInfo();
		}

		if (input.getMouse(0)) {
			int x = (int) input.getMouseX();
			int y = (int) input.getMouseY();
			drawCross(x, y, WHITE);
			// std::cout << "click " << x << "," << y << std::endl;
		}

		int scrolly = input.getScrollY();
		if (scrolly != 0) {
			std::cout << "scroll: " << scrolly << std::endl;
		}
	}

};


int main( void )
{
	MyApp application(160, 90, 24, 4);

	while (!application.quit())
	{
		application.run();
	}

	return 0;
}
