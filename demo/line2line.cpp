/**
 * @file line2line.cpp
 *
 * @brief draw line intersection
 *
 * Copyright 2021-2022 @rktrlng
 * https://github.com/rktrlng/canvas
 */

#include <ctime>

#include <canvas/application.h>
#include <pixelbuffer/math/geom.h>

class MyApp : public cnv::Application
{
public:
	MyApp(uint16_t width, uint16_t height, uint8_t bitdepth, uint8_t factor) : cnv::Application(width, height, bitdepth, factor)
	{
		std::srand(std::time(nullptr));

		init();
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
			layers[0]->lock();

			frametime = 0.0f;
		}

		static float fliptime = 0.0f;
		fliptime += deltatime;
		if (fliptime > 1.0f) {
			fliptime = 0.0f;
			init();
		}
	}

private:
	rt::Line first;
	rt::Line second;
	rt::vec2 intersection;

	void init()
	{
		int height = layers[0]->height();
		int width = layers[0]->width();

		do {
			first.begin = rt::vec2f(0, rand()%height);
			first.end = rt::vec2f(width, rand()%height);
			second.begin = rt::vec2f(rand()%width, 0);
			second.end = rt::vec2f(rand()%width, height);

			intersection = rt::line2line(first, second);
		} while (intersection == rt::vec2(0, 0));
	}

	void updatePixels()
	{
		auto& pixelbuffer = layers[0]->pixelbuffer;
		pixelbuffer.fill({51, 51, 51, 255});

		pixelbuffer.drawLine(first.begin.x, first.begin.y, first.end.x, first.end.y, GRAY);
		pixelbuffer.drawLine(second.begin.x, second.begin.y, second.end.x, second.end.y, GRAY);

		pixelbuffer.drawCircle(intersection.x, intersection.y, 3, GREEN);
		pixelbuffer.setPixel(intersection.x, intersection.y, MAGENTA);
	}

	void handleInput()
	{
		if (input.getKeyDown(cnv::KeyCode::Space)) {
			init();
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
