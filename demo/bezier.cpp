/**
 * @file start.cpp
 *
 * @brief start
 *
 * Copyright 2021-2022 @rktrlng
 * https://github.com/rktrlng/canvas
 */

#include <ctime>

#include <canvas/application.h>

class MyApp : public rt::Application
{
public:
	MyApp(uint16_t width, uint16_t height, uint8_t bitdepth, uint8_t factor) : rt::Application(width, height, bitdepth, factor)
	{
		std::srand(std::time(nullptr));

		init();
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
	pb::vec2f a;
	pb::vec2f b;
	pb::vec2f c;
	pb::vec2f d;
	pb::vec2f prev;

	void init()
	{
		a = pb::vec2f(30, 90);
		b = pb::vec2f(80, 30);
		c = pb::vec2f(240, 150);
		d = pb::vec2f(290, 90);
		prev = pb::vec2f(30, 30);
	}

	void updatePixels()
	{
		auto& pixelbuffer = layers[0]->pixelbuffer;
		// size_t rows = pixelbuffer.header().height;
		size_t cols = pixelbuffer.header().width;

		pixelbuffer.fill(TRANSPARENT);

		for (float x = 0; x < cols; x++) {
			pb::vec2 bezier = pb::vec2::lerp_cubic(a, b, c, d, x/cols);
			if (x != 0) {
				pixelbuffer.drawLine(prev.x, prev.y, bezier.x, bezier.y, RED);
			}
			prev = bezier;
		}

		pixelbuffer.drawCircle(b.x, b.y, 3, GREEN);
		pixelbuffer.drawCircle(c.x, c.y, 3, GREEN);
		pixelbuffer.drawLine(a.x, a.y, b.x, b.y, YELLOW);
		pixelbuffer.drawLine(c.x, c.y, d.x, d.y, YELLOW);

		
		layers[0]->lock();
	}

	void handleInput()
	{
		if (input.getKeyDown(rt::KeyCode::Space)) {
			// std::cout << "spacebar pressed down." << std::endl;
			// layers[0]->pixelbuffer.printInfo();
			init();
		}

		if (input.getMouse(0)) {
			b.x = input.getMouseX();
			b.y = input.getMouseY();
			// std::cout << "click " << (int) input.getMouseX() << "," << (int) input.getMouseY() << std::endl;
		}

		if (input.getMouse(1)) {
			c.x = input.getMouseX();
			c.y = input.getMouseY();
			// std::cout << "click " << (int) input.getMouseX() << "," << (int) input.getMouseY() << std::endl;
		}

		int scrolly = input.getScrollY();
		if (scrolly != 0) {
			std::cout << "scroll: " << scrolly << std::endl;
		}
	}

};


int main( void )
{
	MyApp application(320, 180, 24, 4);

	while (!application.quit())
	{
		application.run();
	}

	return 0;
}
