#include <ctime>

#include <canvas/application.h>

class MyApp : public rt::Application
{
public:
	// MyApp(uint16_t width, uint16_t height, uint8_t bitdepth, uint8_t factor) : rt::Application(width, height, bitdepth, factor) 
	// {
	// 	init();
	// }

	MyApp(pb::PixelBuffer& pixelbuffer, uint8_t factor) : rt::Application(pixelbuffer, factor)
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
				pb::RGBAColor color = pixelbuffer.getPixel(x, y);
				if (color == BLACK) { field[counter] = DEAD; }
				if (color == WHITE) { field[counter] = ALIVE; }

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
			gameoflife();
			layers[0]->lock();

			frametime = 0.0f;
		}
	}

private:
	const uint8_t DEAD = 0; // BLACK
	const uint8_t ALIVE = 1; // WHITE

	// internal data to work with (value are 0,1)
	std::vector<uint8_t> field;

	void gameoflife()
	{
		// get pixelbuffer, rows and cols
		auto& pixelbuffer = layers[0]->pixelbuffer;
		size_t rows = pixelbuffer.header().height;
		size_t cols = pixelbuffer.header().width;

		// set the next state
		std::vector<uint8_t> next = std::vector<uint8_t>(cols*rows, 0);
		for (size_t y = 0; y < rows; y++) {
			for (size_t x = 0; x < cols; x++) {
				int index = pb::idFromPos(x,y,cols);
				uint8_t current = field[index];

				// check 8 neighbours and count the ones that are a ALIVE
				int nc = 0; // number of neighbour cells
				for (int r = -1; r < 2; r++) {
					for (int c = -1; c < 2; c++) {
						if (r == 0 && c == 0) {
							// this is us
						} else {
							pb::vec2i n = pb::wrap(pb::vec2i(x+c, y+r), cols, rows);
							if (field[pb::idFromPos(n.x,n.y,cols)] == ALIVE) { nc++; }
						}
					}
				}

				// Apply rules for each pixel:
				if ((nc == 2 || nc == 3) && current == ALIVE) { current = ALIVE; } // ALIVE on
				if (nc < 2) { current = DEAD; } // lonely
				if (nc > 3) { current = DEAD; } // overpopulation
				if (nc == 3) { current = ALIVE; } // reproduction

				next[index] = current;

				// update pixelbuffer from (current) field
				pb::RGBAColor color;
				if (field[index] == ALIVE) {
					color = WHITE;
				} else {
					color = BLACK;
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
			// layers[0]->pixelbuffer.write("gameoflife.pbf");
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
	pb::PixelBuffer pixelbuffer("assets/gameoflife01.pbf");
	MyApp application(pixelbuffer, 8);

	while (!application.quit())
	{
		application.run();
	}

	return 0;
}
