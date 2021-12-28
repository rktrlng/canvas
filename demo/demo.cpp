#include <ctime>

#include <canvas/application.h>

class MyApp : public rt::Application
{
public:
	// MyApp(uint16_t width, uint16_t height, uint8_t bitdepth, uint8_t factor) : rt::Application(width, height, bitdepth, factor) 
	// {
	// 
	// }

	MyApp(pb::PixelBuffer& pixelbuffer, uint8_t factor, bool locked) : rt::Application(pixelbuffer, factor, locked)
	{

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
	pb::PixelBuffer pixelbuffer("assets/pencils.pbf");
	pb::PixelBuffer letter_f("assets/letter_f.pbf");

	pb::PixelBuffer brush0 = pixelbuffer.copy(32, 32, 64, 48);
	pb::PixelBuffer brush1 = pixelbuffer.copy(32, 32, 24, 64);

	pixelbuffer.blur();
	pixelbuffer.blur();
	pixelbuffer.paste(brush0, 55, 64);
	pixelbuffer.blur();
	pixelbuffer.blur();
	pixelbuffer.paste(brush1, 85, 8);
	pixelbuffer.blur();

	pixelbuffer.paste(brush0, 8, 8);
	pixelbuffer.paste(letter_f, 8, 64);

	pixelbuffer.write("assets/pencils_blurred.pbf");

	MyApp application(pixelbuffer, 4, true);

	while (!application.quit())
	{
		application.run();
	}

	return 0;
}
