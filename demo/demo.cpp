#include <canvas/application.h>

class MyApp : public rt::Application
{
public:
	MyApp(uint16_t width, uint16_t height, uint8_t factor) : rt::Application(width, height, factor) 
	{
		layers.push_back( new rt::Canvas(width, height) );
	}

	MyApp(rt::PixelBuffer& pixelbuffer, uint8_t factor) : rt::Application(pixelbuffer, factor)
	{
		uint16_t cols = pixelbuffer.header().width;
		uint16_t rows = pixelbuffer.header().height;
		layers.push_back( new rt::Canvas(cols, rows) );
		layers[0]->pixelbuffer = pixelbuffer;
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

		static float frametime = 0.0f;
		float maxtime = 0.016666667f;
		frametime += deltatime;
		if (frametime >= maxtime)
		{
			//random();
			frametime = 0.0f;
		}
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

	void random()
	{
		auto& pixelbuffer = layers[0]->pixelbuffer;
		size_t rows = pixelbuffer.header().height;
		size_t cols = pixelbuffer.header().width;
		for (size_t y = 0; y < rows; y++)
		{
			for (size_t x = 0; x < cols; x++)
			{
				pixelbuffer.setPixel(x, y, rt::RGBAColor(rand()%256, rand()%256, rand()%256, 255));
			}
		}
	}
};


int main( void )
{
	rt::PixelBuffer pixelbuffer("assets/base01.pbf");
	// pixelbuffer.fill(rt::WHITE);
	// pixelbuffer.setPixel(16, 16, rt::RED);
	// pixelbuffer.read("assets/base01.pbf");
	MyApp application(pixelbuffer, 8);


	// MyApp application(80, 45, 8);

	// auto& canvas = application.layers[0];
	// auto& pixelbuffer = canvas->pixelbuffer;

	// pixelbuffer.read("assets/base01.pbf");
	// canvas->generateTexture();
	// canvas->generateGeometry(canvas->width(), canvas->height());
	// canvas->dynamic = false;

	while (application.run())
	{
		;
	}
	

	return 0;
}
