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
			frametime = 0.0f;
		}
	}

private:
	rt::BezierCubic curve;

	void init()
	{
		int height = layers[0]->height();
		curve.start = rt::vec2f(30, rand()%height);
		curve.control_start = rt::vec2f(80, rand()%height);
		curve.control_end = rt::vec2f(240, rand()%height);
		curve.end = rt::vec2f(290, rand()%height);
	}

	void updatePixels()
	{
		drawBezier(curve);

		layers[0]->lock();
	}

	void drawBezier(const rt::BezierCubic& bezier)
	{
		auto& pixelbuffer = layers[0]->pixelbuffer;
		// size_t rows = pixelbuffer.height();
		// size_t cols = pixelbuffer.width();

		pixelbuffer.fill(TRANSPARENT);

		rt::vec2f delta = bezier.end - bezier.start;
		int segmentlength = 3;
		float steps = delta.mag()/segmentlength;

		rt::vec2f prev = rt::vec2f(0, 0);
		for (float t = 0; t <= 1.01f; t += 1.0f / steps) {
			rt::vec2f point = bezier.point(t);
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
		static bool stickystart  = false;
		static bool stickystartc = false;
		static bool stickyend    = false;
		static bool stickyendc   = false;

		if (input.getMouseUp(0)) {
			stickystart  = false;
			stickystartc = false;
			stickyend    = false;
			stickyendc   = false;
		}

		if (input.getMouseDown(0) || input.getMouse(0)) {
			// get mousepos and collission spheres on points
			rt::vec2f mousepos  = rt::vec2f(input.getMouseX(), input.getMouseY());

			rt::Circlef start   = rt::Circlef(curve.start.x, curve.start.y, 3);
			rt::Circlef start_c = rt::Circlef(curve.control_start.x, curve.control_start.y, 3);
			rt::Circlef end     = rt::Circlef(curve.end.x, curve.end.y, 3);
			rt::Circlef end_c   = rt::Circlef(curve.control_end.x, curve.control_end.y, 3);

			// vec from end points to their handlepoints (to move control points relative to end points)
			rt::vec2f control_vec_start = curve.control_start - curve.start;
			rt::vec2f control_vec_end   = curve.control_end - curve.end;

			if (input.getMouseDown(0)) {
				if (rt::point2circle(mousepos, start)) {
					stickystart = true;
				}
				if (rt::point2circle(mousepos, start_c)) {
					stickystartc = true;
				}
				if (rt::point2circle(mousepos, end)) {
					stickyend = true;
				}
				if (rt::point2circle(mousepos, end_c)) {
					stickyendc = true;
				}
			}

			if (input.getMouse(0)) {
				if (stickystart) {
					curve.start = mousepos;
					curve.control_start = curve.start + control_vec_start;
				}
				if (stickystartc) {
					curve.control_start = mousepos;
				}
				if (stickyend) {
					curve.end = mousepos;
					curve.control_end = curve.end + control_vec_end;
				}
				if (stickyendc) {
					curve.control_end = mousepos;
				}
			}
		}

		// ########################################################
		if (input.getKeyDown(cnv::KeyCode::Space)) {
			init();
		}

		// int scrolly = input.getScrollY();
		// if (scrolly != 0) {
		// 	std::cout << "scroll: " << scrolly << std::endl;
		// }
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
