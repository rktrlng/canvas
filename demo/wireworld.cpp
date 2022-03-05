/**
 * @file wireworld.cpp
 *
 * @brief Wireworld implementation
 *
 * Copyright 2021-2022 @rktrlng
 * https://github.com/rktrlng/canvas
 */

#include <ctime>

#include <canvas/application.h>

class MyApp : public cnv::Application
{
private:
	const uint8_t EMPTY = 0; // BLACK
	const uint8_t CONDUCTOR = 1; // YELLOW
	const uint8_t HEAD = 2; // BLUE
	const uint8_t TAIL = 3; // CYAN

	// internal data to work with (value are 0,1,2,3)
	std::vector<uint8_t> m_field;

public:
	// MyApp(uint16_t width, uint16_t height, uint8_t bitdepth, uint8_t factor) : cnv::Application(width, height, bitdepth, factor)
	// {
	// 	init();
	// }

	MyApp(rt::PixelBuffer& pixelbuffer, uint8_t factor) : cnv::Application(pixelbuffer, factor)
	{
		init();
	}

	virtual ~MyApp()
	{

	}

	void init()
	{
		auto& pixelbuffer = layers[0]->pixelbuffer;
		uint16_t cols = pixelbuffer.header().width;
		uint16_t rows = pixelbuffer.header().height;
		
		// fill m_field for wireworld
		m_field = std::vector<uint8_t>(rows*cols, 0);
		int counter = 0;
		for (size_t y = 0; y < rows; y++) {
			for (size_t x = 0; x < cols; x++) {
				rt::RGBAColor color = pixelbuffer.getPixel(x, y);
				if (color == BLACK) { m_field[counter] = EMPTY; }
				if (color == YELLOW) { m_field[counter] = CONDUCTOR; }
				if (color == BLUE) { m_field[counter] = HEAD;}
				if (color == CYAN) { m_field[counter] = TAIL; }

				counter++;
			}
		}
	}


	void update(float deltatime) override
	{
		handleInput();

		static float frametime = 0.0f;
		float maxtime = 0.1f;
		frametime += deltatime;
		if (frametime >= maxtime)
		{
			wireworld();
			layers[0]->lock();

			frametime = 0.0f;
		}
	}

private:
	void wireworld()
	{
		// get pixelbuffer, rows and cols
		auto& pixelbuffer = layers[0]->pixelbuffer;
		size_t rows = pixelbuffer.header().height;
		size_t cols = pixelbuffer.header().width;

		// set the next state
		std::vector<uint8_t> next = std::vector<uint8_t>(cols*rows, 0);
		for (size_t y = 0; y < rows; y++) {
			for (size_t x = 0; x < cols; x++) {
				// Apply rules for each pixel:
				//- EMPTY -> EMPTY (do nothing)
				//- HEAD -> TAIL
				//- TAIL -> CONDUCTOR
				//- CONDUCTOR: if 1 or 2 neighbours are HEAD -> HEAD
				uint8_t current = m_field[rt::index(x,y,cols)];
				if (current == EMPTY) {
					continue; // nothing to do, continue to next pixel
				} else if (current == HEAD) {
					current = TAIL;
				} else if (current == TAIL) {
					current = CONDUCTOR;
				} else if (current == CONDUCTOR) {
					// check 8 neighbours and count the ones that are a HEAD
					int nc = 0; // number of neighbour cells
					for (int r = -1; r < 2; r++) {
						for (int c = -1; c < 2; c++) {
							if (r == 0 && c == 0) {
								// this is us
							} else {
								rt::vec2i n = rt::wrap(rt::vec2i(x+c, y+r), cols, rows);
								if (m_field[rt::index(n.x,n.y,cols)] == HEAD) { nc++; }
							}
						}
					}
					if (nc == 1 || nc == 2) { current = HEAD; }
				}
				next[rt::index(x,y,cols)] = current;

				// update pixelbuffer from (current) m_field
				rt::RGBAColor color = BLACK;
				int index = rt::index(x,y,cols);
				if (m_field[index] == CONDUCTOR) {
					color = YELLOW;
				} else if (m_field[index] == HEAD) {
					color = BLUE;
				} else { // TAIL
					color = CYAN;
				}
				pixelbuffer.setPixel(x, y, color);
			}
		}

		// update m_field to next state
		m_field = next;
	}

	void handleInput() {
		if (input.getKeyDown(cnv::KeyCode::Space)) {
			std::cout << "spacebar pressed down." << std::endl;
			layers[0]->pixelbuffer.printInfo();
			layers[0]->pixelbuffer.write("wire.pbf");
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
	rt::PixelBuffer pixelbuffer("assets/wire01.pbf");
	MyApp application(pixelbuffer, 8);

	while (!application.quit())
	{
		application.run();
	}

	return 0;
}
