#include <ctime>

#include <canvas/application.h>

class MyApp : public rt::Application
{
public:
	// MyApp(uint16_t width, uint16_t height, uint8_t factor) : rt::Application(width, height, factor) 
	// {
	// 	init();
	// }

	MyApp(rt::PixelBuffer& pixelbuffer, uint8_t factor) : rt::Application(pixelbuffer, factor)
	{
		init();
	}

	virtual ~MyApp()
	{
		for (auto canvas : layers) {
			delete canvas;
		}
		layers.clear();
	}

	void init()
	{
		auto& pixelbuffer = layers[0]->pixelbuffer;
		uint16_t cols = pixelbuffer.header().width;
		uint16_t rows = pixelbuffer.header().height;
		
		// fill field for wireworld
		field = std::vector<uint8_t>(rows*cols, 0);
		int counter = 0;
		for (size_t y = 0; y < rows; y++) {
			for (size_t x = 0; x < cols; x++) {
				rt::RGBAColor color = pixelbuffer.getPixel(x, y);
				if (color == rt::BLACK) { field[counter] = EMPTY; }
				if (color == rt::YELLOW) { field[counter] = CONDUCTOR; }
				if (color == rt::BLUE) { field[counter] = HEAD;}
				if (color == rt::CYAN) { field[counter] = TAIL; }

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

			frametime = 0.0f;
		}
	}

private:
	const uint8_t EMPTY = 0; // BLACK
	const uint8_t CONDUCTOR = 1; // YELLOW
	const uint8_t HEAD = 2; // BLUE
	const uint8_t TAIL = 3; // CYAN

	// internal data to work with (value are 0,1,2,3)
	std::vector<uint8_t> field;

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
				uint8_t current = field[rt::idFromPos(x,y,cols)];
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
								if (field[rt::idFromPos(n.x,n.y,cols)] == HEAD) { nc++; }
							}
						}
					}
					if (nc == 1 || nc == 2) { current = HEAD; }
				}
				next[rt::idFromPos(x,y,cols)] = current;

				// update pixelbuffer from (current) field
				rt::RGBAColor color;
				int index = rt::idFromPos(x,y,cols);
				if (field[index] == CONDUCTOR) {
					color = rt::YELLOW;
				} else if (field[index] == HEAD) {
					color = rt::BLUE;
				} else { // TAIL
					color = rt::CYAN;
				}
				pixelbuffer.setPixel(x, y, color);
			}
		}

		// update field to next state
		field = next;
	}

	void handleInput() {
		if (input.getKeyDown(rt::KeyCode::Space)) {
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
	rt::PixelBuffer pixelbuffer("assets/base01.pbf");
	MyApp application(pixelbuffer, 8);

	while (!application.quit())
	{
		application.run();
	}

	return 0;
}