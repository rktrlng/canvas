/**
 * @file cube.cpp
 *
 * @brief 3D transforms
 *
 * Copyright 2021-2022 @rktrlng
 * https://github.com/rktrlng/canvas
 */

#include <ctime>

#include <canvas/application.h>
#include <pixelbuffer/math/mat4.h>

class MyApp : public cnv::Application
{
public:
	MyApp(uint16_t width, uint16_t height, uint8_t bitdepth, uint8_t factor) : cnv::Application(width, height, bitdepth, factor) 
	{
		init();
	}

	// MyApp(rt::PixelBuffer& pixelbuffer, uint8_t factor, bool locked) : cnv::Application(pixelbuffer, factor, locked)
	// {

	// }

	virtual ~MyApp()
	{

	}

	void update(float deltatime) override
	{
		handleInput();

		drawCube(deltatime);
	}

private:
	std::vector<rt::vec4f> m_points;

	void init()
	{
		m_points.clear();
		m_points = std::vector<rt::vec4f>(8);
		m_points[0] = rt::vec4f(-0.5, -0.5, -0.5, 1.0);
		m_points[1] = rt::vec4f( 0.5, -0.5, -0.5, 1.0);
		m_points[2] = rt::vec4f( 0.5,  0.5, -0.5, 1.0);
		m_points[3] = rt::vec4f(-0.5,  0.5, -0.5, 1.0);
		m_points[4] = rt::vec4f(-0.5, -0.5,  0.5, 1.0);
		m_points[5] = rt::vec4f( 0.5, -0.5,  0.5, 1.0);
		m_points[6] = rt::vec4f( 0.5,  0.5,  0.5, 1.0);
		m_points[7] = rt::vec4f(-0.5,  0.5,  0.5, 1.0);
	}

	void drawCube(float deltaTime)
	{
		static float angle = 0.0f;
		angle += 1 * deltaTime;

		auto& pixelbuffer = layers[0]->pixelbuffer;
		float cols = (float) pixelbuffer.header().width;
		float rows = (float) pixelbuffer.header().height;

		pixelbuffer.fill(BLACK);

		// define transform for object
		rt::vec4f scale = rt::vec4f(125.0f, 125.0f, 125.0f, 0.0f);
		rt::vec4f rotation = rt::vec4f(angle, angle + M_PI/3, angle - M_PI/3, 0.0f);
		rt::vec4f translation = rt::vec4f(cols/2, rows/2, 0.0f, 0.0f);

		// create modelmatrix
		rt::mat4f model = rt::modelMatrix(translation, rotation, scale);
		rt::mat4f view = rt::mat4f(); // identity for now...
		// rt::mat4f projection = rt::perspectiveMatrix(92.0f, cols/rows, 0.01f, 1000.0f);
		rt::mat4f projection = rt::mat4f();

		// create mvp
		rt::mat4f mvp = projection * view * model;

		// transform each point in 3D object to screen coords
		rt::vec4f drawpoints[8];
		for (size_t i = 0; i < 8; i++) {
			rt::vec4f transformed = mvp * m_points[i];
			drawpoints[i] = rt::vec4(transformed.x, transformed.y, transformed.z, 1.0f);
		}

		// define edges between points
		std::vector<rt::vec2i> edges;
		edges.push_back( rt::vec2i(0, 1));
		edges.push_back( rt::vec2i(1, 2));
		edges.push_back( rt::vec2i(2, 3));
		edges.push_back( rt::vec2i(3, 0));

		edges.push_back( rt::vec2i(4, 5));
		edges.push_back( rt::vec2i(5, 6));
		edges.push_back( rt::vec2i(6, 7));
		edges.push_back( rt::vec2i(7, 4));

		edges.push_back( rt::vec2i(0, 4));
		edges.push_back( rt::vec2i(1, 5));
		edges.push_back( rt::vec2i(2, 6));
		edges.push_back( rt::vec2i(3, 7));

		// draw edges on canvas
		for (size_t i = 0; i < edges.size(); i++) {
			int a = edges[i].x;
			int b = edges[i].y;
			rt::RGBAColor color = rt::RGBAColor();
			if (i < 4) {
				color = RED;
			} else if (i > 3 && i < 8) {
				color = GREEN;
			} else {
				color = CYAN;
			}
			rt::vec2f from = rt::vec2f(drawpoints[a].x, drawpoints[a].y);
			rt::vec2f to =   rt::vec2f(drawpoints[b].x, drawpoints[b].y);
			pixelbuffer.drawLine(from.x, from.y, to.x, to.y, color);
		}

		// draw circles
		for (size_t i = 0; i < 8; i++) {
			pixelbuffer.drawCircleFilled(drawpoints[i].x, drawpoints[i].y, 2, MAGENTA);
		}

		// draw points
		// for (size_t i = 0; i < 8; i++) {
		// 	pixelbuffer.setPixel(drawpoints[i].x, drawpoints[i].y, BLACK);
		// }

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

	MyApp application(320, 320, 32, 3);

	while (!application.quit())
	{
		application.run();
	}

	return 0;
}
