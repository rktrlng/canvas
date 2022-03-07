/**
 * @file painter.cpp
 *
 * @brief Paint App
 *
 * Copyright 2022 @rktrlng
 * https://github.com/rktrlng/canvas
 */

#include <canvas/application.h>

class MyApp : public cnv::Application
{
private:
	bool m_showMenu = false;
	rt::RGBAColor m_fcolor = {0,0,0,0};
public:
	MyApp(uint16_t width, uint16_t height, uint8_t bitdepth, uint8_t factor) : cnv::Application(width, height, bitdepth, factor)
	{
		cnv::Canvas* menuCanvas = new cnv::Canvas(width, height, bitdepth, factor);
		layers.push_back(menuCanvas);
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
			clearUI();
			drawPixels();
			if (m_showMenu) {
				drawPalette();
			}
			drawCursor();
			frametime = 0.0f;

			layers[0]->lock();
			layers[1]->lock();
		}
	}

private:
	void drawPixels() {
		auto& pixelbuffer = layers[0]->pixelbuffer;
		size_t cols = pixelbuffer.header().width;
		size_t rows = pixelbuffer.header().height;
		layers[0]->pixelbuffer.drawSquare(0, 0, cols-1, rows-1, {255, 255, 255, 64});
	}

	void clearUI()
	{
		layers[1]->pixelbuffer.fill({0,0,0,0});
	}

	void drawPalette()
	{
		auto& pixelbuffer = layers[1]->pixelbuffer;

		uint8_t size = 4;
		int offset = 8;

		int value = 0;
		for (size_t x = 0; x < 9; x++) {
			if (value > 255) { value = 255; }
			pixelbuffer.drawSquareFilled( x * size + offset, 0 + offset, size, size, {(uint8_t)value, (uint8_t)value, (uint8_t)value, 255});
			value += 32;
		}

		rt::RGBAColor color = RED;
		for (size_t x = 0; x < 10; x++) {
			pixelbuffer.drawSquareFilled( x * size + offset, size + offset, size, size, color);
			color = rt::rotate(color, 0.05f);
		}
		for (size_t x = 0; x < 10; x++) {
			pixelbuffer.drawSquareFilled( x * size + offset, (size*2) + offset, size, size, color);
			color = rt::rotate(color, 0.05f);
		}
	}

	void drawCursor()
	{
		int x = (int) input.getMouseX();
		int y = (int) input.getMouseY();
		rt::vec2i pos = rt::vec2i(x, y);
		std::vector<rt::vec2i> points = { {-3,0}, {-2,0}, {3,0}, {2,0}, {0,-3}, {0,-2}, {0,3}, {0,2} };
		auto& pixelbuffer = layers[1]->pixelbuffer;
		for (size_t i = 0; i < points.size(); i++) {
			rt::vec2i dpos = pos + points[i];
			pixelbuffer.setPixel(dpos.x, dpos.y, {0, 0, 0, 255});
		}
	}

	void handleInput()
	{
		if (input.getKeyDown(cnv::KeyCode::Q)) { m_showMenu = !m_showMenu; }

		if (input.getKeyDown(cnv::KeyCode::Minus)) { layers[0]->scale /= 2; }
		if (input.getKeyDown(cnv::KeyCode::Equal)) { layers[0]->scale *= 2; }
		if (input.getKeyDown(cnv::KeyCode::Left)) { layers[0]->position.x -= layers[0]->scale; }
		if (input.getKeyDown(cnv::KeyCode::Right)) { layers[0]->position.x += layers[0]->scale; }
		if (input.getKeyDown(cnv::KeyCode::Up)) { layers[0]->position.y -= layers[0]->scale; }
		if (input.getKeyDown(cnv::KeyCode::Down)) { layers[0]->position.y += layers[0]->scale; }
		if (input.getKeyDown(cnv::KeyCode::C)) { layers[0]->position = {0, 0}; }

		if (layers[0]->scale <= 0) { layers[0]->scale = 1; }
		if (layers[0]->scale >= 64) { layers[0]->scale = 64; }

		layers[1]->scale = layers[0]->scale;

		if (input.getKeyDown(cnv::KeyCode::Space)) {
			std::cout << "spacebar pressed down." << std::endl;
			layers[0]->pixelbuffer.printInfo();
		}

		if (input.getMouse(0)) {
			int x = (int) input.getMouseX() - layers[0]->position.x / layers[0]->scale;
			int y = (int) input.getMouseY() - layers[0]->position.y / layers[0]->scale;
			if (m_showMenu) {
				m_fcolor = layers[1]->pixelbuffer.getPixel(x, y);
			} else {
				layers[0]->pixelbuffer.setPixel(x, y, m_fcolor);
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
	MyApp application(128, 72, 32, 12);
	application.hideMouse();
	std::cout << "Press 'Q' to toggle palette." << std::endl;

	while (!application.quit())
	{
		application.run();
	}

	return 0;
}
