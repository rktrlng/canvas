#include <ctime>
#include <string>
#include <sstream>

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

		std::srand(std::time(nullptr));
		random(60);
		// fill field for cave
		field = std::vector<uint8_t>(rows*cols, 0);
		int counter = 0;
		for (size_t y = 0; y < rows; y++) {
			for (size_t x = 0; x < cols; x++) {
				rt::RGBAColor color = pixelbuffer.getPixel(x, y);
				if (color == rt::BLACK) { field[counter] = 0; }
				if (color == rt::WHITE) { field[counter] = 1; }

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
			const int iterations = 30;
			static int count = 0;
			if (count < iterations) {
				cave();
				std::cout << count << "\n";
			}
			count++;
			if ( count > iterations) {
				count = iterations;
			}

			frametime = 0.0f;
		}
	}

private:
	// internal data to work with (value are 0,1)
	std::vector<uint8_t> field;

	void cave()
	{
		// get pixelbuffer, rows and cols
		auto& pixelbuffer = layers[0]->pixelbuffer;
		size_t rows = pixelbuffer.header().height;
		size_t cols = pixelbuffer.header().width;

		{
			static int counter = 0;
			std::stringstream filename;
			filename <<  "caves/cave";
			const int leadingzeros = 4;
			for (int i = 1; i < leadingzeros+1; i++) {
				if (counter < pow(10, i)) { filename << "0"; }
			}
			filename << counter << ".pbf";
			counter++;
			pixelbuffer.write(filename.str());
			std::cout << filename.str() << std::endl;
		}

		// set the next state
		std::vector<uint8_t> next = std::vector<uint8_t>(cols*rows, 0);
		for (size_t y = 0; y < rows; y++) {
			for (size_t x = 0; x < cols; x++) {
				// Apply rules for each pixel:
				int current = field[rt::idFromPos(x,y,cols)];

				// check 8 neighbours and count the ones that are a HEAD
				int nc = 0; // number of neighbour cells
				for (int r = -1; r < 2; r++) {
					for (int c = -1; c < 2; c++) {
						rt::vec2i n = rt::wrap(rt::vec2i(x+c, y+r), cols, rows);
						if (field[rt::idFromPos(n.x,n.y,cols)] == 0) { nc++; }
					}
				}
				if (nc < 4){ current = 1; }
				if (nc > 4){ current = 0; }
				next[rt::idFromPos(x,y,cols)] = current;

				// update pixelbuffer from (current) field
				rt::RGBAColor color;
				int index = rt::idFromPos(x,y,cols);
				if (field[index] == 0) {
					color = rt::BLACK;
				} else {
					color = rt::WHITE;
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
		}

		if (input.getMouseDown(0)) {
			std::cout << "click " << (int) input.getMouseX() << "," << (int) input.getMouseY() << std::endl;
			layers[0]->pixelbuffer.blur();
		}

		int scrolly = input.getScrollY();
		if (scrolly != 0) {
			std::cout << "scroll: " << scrolly << std::endl;
		}
	}

	void random(int percentage = 50) {
		// get pixelbuffer, rows and cols
		auto& pixelbuffer = layers[0]->pixelbuffer;
		size_t rows = pixelbuffer.header().height;
		size_t cols = pixelbuffer.header().width;

		for (size_t y = 0; y < rows; y++) {
			for (size_t x = 0; x < cols; x++) {
				rt::RGBAColor color = rt::BLACK;
				int value = rand()%100;
				if (value < percentage) {
					color = rt::WHITE;
				}
				pixelbuffer.setPixel(x, y, color);
			}
		}
	}

};


int main( void )
{
	rt::PixelBuffer pixelbuffer(160, 90, 8);

	MyApp application(pixelbuffer, 4);

	while (!application.quit())
	{
		application.run();
	}

	return 0;
}
