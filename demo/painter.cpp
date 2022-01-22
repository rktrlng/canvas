/**
 * @file painter.cpp
 *
 * @brief Paint App
 *
 * Copyright 2022 @rktrlng
 * https://github.com/rktrlng/canvas
 */

#include <canvas/application.h>

class MyApp : public rt::Application
{
private:
	bool showMenu = false;
	pb::RGBAColor fcolor = {0,0,0,0};
public:
	MyApp(uint16_t width, uint16_t height, uint8_t bitdepth, uint8_t factor) : rt::Application(width, height, bitdepth, factor)
	{
		layers.push_back(new rt::Canvas(width, height, bitdepth, factor));
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
			clearUI();
			if (showMenu) {
				drawPalette();
			}
			drawCursor();
			frametime = 0.0f;
		}
	}

private:
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

		pb::RGBAColor color = RED;
		for (size_t x = 0; x < 10; x++) {
			pixelbuffer.drawSquareFilled( x * size + offset, size + offset, size, size, color);
			color = pb::Color::rotate(color, 0.05f);
		}
		for (size_t x = 0; x < 10; x++) {
			pixelbuffer.drawSquareFilled( x * size + offset, (size*2) + offset, size, size, color);
			color = pb::Color::rotate(color, 0.05f);
		}

		layers[1]->lock();
	}

	void drawCursor()
	{
		int x = (int) input.getMouseX();
		int y = (int) input.getMouseY();
		pb::vec2i pos = pb::vec2i(x, y);
		std::vector<pb::vec2i> points = { {-3,0}, {-2,0}, {3,0}, {2,0}, {0,-3}, {0,-2}, {0,3}, {0,2} };
		auto& pixelbuffer = layers[1]->pixelbuffer;
		for (size_t i = 0; i < points.size(); i++) {
			pb::vec2i dpos = pos + points[i];
			pixelbuffer.setPixel(dpos.x, dpos.y, {0, 0, 0, 255});
		}

		layers[1]->lock();
	}

	void handleInput()
	{
		if (input.getKeyDown(rt::KeyCode::Q)) { showMenu = !showMenu; }

		if (input.getKeyDown(rt::KeyCode::Minus)) { layers[0]->scale /= 2; }
		if (input.getKeyDown(rt::KeyCode::Equal)) { layers[0]->scale *= 2; }
		if (input.getKeyDown(rt::KeyCode::Left)) { layers[0]->position.x -= layers[0]->scale; }
		if (input.getKeyDown(rt::KeyCode::Right)) { layers[0]->position.x += layers[0]->scale; }
		if (input.getKeyDown(rt::KeyCode::Up)) { layers[0]->position.y -= layers[0]->scale; }
		if (input.getKeyDown(rt::KeyCode::Down)) { layers[0]->position.y += layers[0]->scale; }
		if (input.getKeyDown(rt::KeyCode::C)) { layers[0]->position = {0, 0}; }

		if (layers[0]->scale <= 0) { layers[0]->scale = 1; }
		if (layers[0]->scale >= 64) { layers[0]->scale = 64; }

		layers[1]->scale = layers[0]->scale;

		if (input.getKeyDown(rt::KeyCode::Space)) {
			std::cout << "spacebar pressed down." << std::endl;
			layers[0]->pixelbuffer.printInfo();
		}

		if (input.getMouse(0)) {
			int x = (int) input.getMouseX() - layers[0]->position.x / layers[0]->scale;
			int y = (int) input.getMouseY() - layers[0]->position.y / layers[0]->scale;
			if (showMenu) {
				fcolor = layers[1]->pixelbuffer.getPixel(x, y);
			} else {
				layers[0]->pixelbuffer.setPixel(x, y, fcolor);
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
	MyApp application(160, 90, 32, 8);
	application.hideMouse();

	while (!application.quit())
	{
		application.run();
	}

	return 0;
}
