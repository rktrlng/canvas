#include <ctime>
#include <string>
#include <sstream>

#include <canvas/application.h>

class MyApp : public rt::Application
{
public:
	MyApp(uint16_t width, uint16_t height, uint8_t factor) : rt::Application(width, height, factor) 
	{
		std::srand(std::time(nullptr));

		for (size_t i = 0; i < 256; i++)
		{
			rule(i, false);
		}

		// layers.push_back( new rt::Canvas(width, height) );
	}

	// MyApp(rt::PixelBuffer& pixelbuffer, uint8_t factor) : rt::Application(pixelbuffer, factor)
	// {
	// 	uint16_t cols = pixelbuffer.header().width;
	// 	uint16_t rows = pixelbuffer.header().height;
	// 	layers.push_back( new rt::Canvas(cols, rows) );
	// 	layers[0]->pixelbuffer = pixelbuffer;
	// }

	virtual ~MyApp()
	{
		for (auto canvas : layers) {
			delete canvas;
		}
		layers.clear();
	}

	void update(float deltatime) override
	{
		handleInput();
		
		static float frametime = 0.0f;
		float maxtime = 1.5f;
		frametime += deltatime;
		if (frametime >= maxtime)
		{
			static int r = 0;
			std::cout << "rule set: " << r << std::endl;
			rule(r);
			r++;
			r %= 256;

			frametime = 0.0f;
		}
	}

private:
	void rule(int num, bool wr = false)
	{
		auto& pixelbuffer = layers[0]->pixelbuffer;

		const size_t rows = pixelbuffer.header().height;
		const size_t cols = pixelbuffer.header().width;

		std::vector<uint8_t> ruleset(8, 0);
		int counter = 0;
		int rn = num;
		while (rn > 0)
		{
			ruleset[counter] = rn & 1;
			rn >>= 1;
			counter++;
		}

		std::vector<uint8_t> row(cols, 0);
		row[cols/2-1] = 1; // first pixel half way
		for (size_t i = 0; i < cols; i++)
		{
			row[i] = rand()%2;
		}

		// draw first row
		for (size_t x = 1; x < cols-1; x++) {
			rt::RGBAColor color = rt::WHITE;
			if (row[x] == 1) {
				color = rt::BLACK;
			}
			pixelbuffer.setPixel(x, 0, color);
		}

		// draw the rest of the owl
		for (size_t y = 1; y < rows; y++) {
			std::vector<uint8_t> next(cols, 0);
			for (size_t x = 1; x < cols-1; x++) {
				int index = 0;
				if (row[x-1] == 1) { index += 4; }
				if (row[x+0] == 1) { index += 2; }
				if (row[x+1] == 1) { index += 1; }

				uint8_t result = ruleset[index];
				next[x] = result;

				rt::RGBAColor color = rt::WHITE;
				if (result) {
					color = rt::BLACK;
				}
				pixelbuffer.setPixel(x, y, color);
			}
			row = next;
		}

		if (wr)
		{
			std::string name = createFilename("rules/", num, "pbf");
			std::cout << "writing: " << name << std::endl;
			pixelbuffer.write(name);
		}
	}

	std::string createFilename(const std::string& prefix, uint32_t counter, const std::string& ext, uint8_t leading0=4) {
		std::stringstream filename;
		filename << prefix;
		for (int i = 1; i < leading0+1; i++) {
			if (counter < pow(10, i)) { filename << "0"; }
		}
		filename << counter << "." << ext;
		return filename.str();
	}

	void handleInput() {
		if (input.getKeyDown(rt::KeyCode::Space)) {
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
	MyApp application(128, 128, 7);

	while (!application.quit())
	{
		application.run();
	}

	return 0;
}
