#include <ctime>

#include <canvas/application.h>

class MyApp : public rt::Application
{
public:
	MyApp(uint16_t width, uint16_t height, uint8_t bitdepth, uint8_t factor) : rt::Application(width, height, bitdepth, factor) 
	{
		// layers.push_back( new rt::Canvas(width, height) );
	}

	MyApp(rt::PixelBuffer& pixelbuffer, uint8_t factor) : rt::Application(pixelbuffer, factor)
	{
		// uint16_t cols = pixelbuffer.header().width;
		// uint16_t rows = pixelbuffer.header().height;
		// layers.push_back( new rt::Canvas(cols, rows, pixelbuffer.header().bitdepth) );
		// layers[0]->pixelbuffer = pixelbuffer;
	}

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
	}

private:
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
	rt::PixelBuffer pixelbuffer("assets/pencils.pbf");
	rt::PixelBuffer letter_f("assets/letter_f.pbf");

	rt::PixelBuffer brush0 = pixelbuffer.copy(32, 32, 64, 48);
	rt::PixelBuffer brush1 = pixelbuffer.copy(32, 32, 24, 64);

	pixelbuffer.blur();
	pixelbuffer.blur();
	pixelbuffer.blur();
	pixelbuffer.blur();
	pixelbuffer.paste(brush0, 55, 64);
	pixelbuffer.paste(brush1, 85, 8);
	pixelbuffer.blur();

	pixelbuffer.paste(brush0, 8, 8);
	pixelbuffer.paste(letter_f, 8, 64);

	pixelbuffer.write("assets/pencils_blurred.pbf");

	MyApp application(pixelbuffer, 4);

	while (!application.quit())
	{
		application.run();
	}

	return 0;
}
