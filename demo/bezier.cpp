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
#include <pixelbuffer/math/geom.h>

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
	pb::BezierCubic curve;

	void init()
	{
		curve.start = pb::vec2f(30, 90);
		curve.control_start = pb::vec2f(80, 30);
		curve.control_end = pb::vec2f(240, 150);
		curve.end = pb::vec2f(290, 90);
	}

	void updatePixels()
	{
		drawBezier(curve);

		layers[0]->lock();
	}

	void drawBezier(const pb::BezierCubic& bezier)
	{
		auto& pixelbuffer = layers[0]->pixelbuffer;
		// size_t rows = pixelbuffer.height();
		// size_t cols = pixelbuffer.width();

		pixelbuffer.fill(TRANSPARENT);

		pb::vec2f delta = bezier.end - bezier.start;
		int segmentlength = 3;
		float steps = delta.mag()/segmentlength;

		pb::vec2f prev = pb::vec2f(0, 0);
		for (float t = 0; t <= 1.01f; t += 1.0f / steps) {
			pb::vec2f point = bezier.point(t);
			if (t != 0) {
				pixelbuffer.drawLine(prev.x, prev.y, point.x, point.y, RED);
			}
			prev = point;
		}

		// draw control points and lines
		pixelbuffer.drawCircle(bezier.control_start.x, bezier.control_start.y, 3, GREEN);
		pixelbuffer.drawCircle(bezier.control_end.x, bezier.control_end.y, 3, GREEN);
		pixelbuffer.drawLine(bezier.start.x, bezier.start.y, bezier.control_start.x, bezier.control_start.y, YELLOW);
		pixelbuffer.drawLine(bezier.control_end.x, bezier.control_end.y, bezier.end.x, bezier.end.y, YELLOW);
		// draw start and end points
		pixelbuffer.drawCircle(bezier.start.x, bezier.start.y, 3, GREEN);
		pixelbuffer.drawCircle(bezier.end.x, bezier.end.y, 3, GREEN);

		// pixelbuffer.drawLine(bezier.control_start.x, bezier.control_start.y, bezier.control_end.x, bezier.control_end.y, {255, 127, 0, 240});
	}

	void handleInput()
	{
		if (input.getKeyDown(rt::KeyCode::Space)) {
			init();
		}

		if (input.getMouse(0)) {
			pb::vec2f mousepos  = pb::vec2f(input.getMouseX(), input.getMouseY());

			pb::Circlef start   = pb::Circlef(curve.start.x, curve.start.y, 3);
			pb::Circlef start_c = pb::Circlef(curve.control_start.x, curve.control_start.y, 3);
			pb::Circlef end     = pb::Circlef(curve.end.x, curve.end.y, 3);
			pb::Circlef end_c   = pb::Circlef(curve.control_end.x, curve.control_end.y, 3);

			if (pb::point2circle(mousepos, start)) {
				curve.start.x = mousepos.x;
				curve.start.y = mousepos.y;
			}
			if (pb::point2circle(mousepos, start_c)) {
				curve.control_start.x = mousepos.x;
				curve.control_start.y = mousepos.y;
			}
			if (pb::point2circle(mousepos, end)) {
				curve.end.x = mousepos.x;
				curve.end.y = mousepos.y;
			}
			if (pb::point2circle(mousepos, end_c)) {
				curve.control_end.x = mousepos.x;
				curve.control_end.y = mousepos.y;
			}
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
