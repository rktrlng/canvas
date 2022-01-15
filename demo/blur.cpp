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

class MyApp : public rt::Application
{
public:
	MyApp(uint16_t width, uint16_t height, uint8_t bitdepth, uint8_t factor) : rt::Application(width, height, bitdepth, factor)
	{
		std::srand(std::time(nullptr));
		layers[0]->pixelbuffer.fill(BLACK);
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
	void updatePixels()
	{
		auto& pixelbuffer = layers[0]->pixelbuffer;
		size_t cols = pixelbuffer.header().width;
		size_t rows = pixelbuffer.header().height;

		static float angle = 0.0f;
		pb::vec2f pos = pb::vec2f(16, 0);
		pos.rotate(angle);
		angle += 0.05f;

		pixelbuffer.setPixel(cols/2 + pos.x, rows/2 + pos.y, WHITE);
		pixelbuffer.setPixel(cols/2 + pos.x + 1, rows/2 + pos.y, WHITE);
		pixelbuffer.setPixel(cols/2 + pos.x - 1, rows/2 + pos.y, WHITE);
		pixelbuffer.setPixel(cols/2 + pos.x, rows/2 + pos.y + 1, WHITE);
		pixelbuffer.setPixel(cols/2 + pos.x, rows/2 + pos.y - 1, WHITE);

		pixelbuffer.blur();
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
	MyApp application(160, 90, 24, 4);

	while (!application.quit())
	{
		application.run();
	}

	return 0;
}
