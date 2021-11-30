#include <ctime>

#include <canvas/application.h>

class MyApp : public rt::Application
{
private:
	rt::PerlinNoise pn;
public:
	MyApp(uint16_t width, uint16_t height, uint8_t factor) : rt::Application(width, height, factor) 
	{
		srand((unsigned)time(nullptr));

		unsigned int seed = rand()%1000;
		// unsigned int seed = 42;
		pn = rt::PerlinNoise(seed);
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
		float maxtime = 0.05f;
		frametime += deltatime;
		if (frametime >= maxtime)
		{
			noise();
			frametime = 0.0f;
		}
	}

private:
	void noise()
	{
		auto& pixelbuffer = layers[0]->pixelbuffer;

		static double z = 0.0f;
		z += 0.01f;

		size_t rows = pixelbuffer.header().height;
		size_t cols = pixelbuffer.header().width;
		for (size_t i = 0; i < rows; i++) {
			for (size_t j = 0; j < cols; j++) {
				double x = (double)j/((double)cols);
				double y = (double)i/((double)rows);
				// double z = 0.0f;

				// find a nice noise for our purpose
				// pn.noise(xsize, ysize, zsize) * multiplier;
				double a = pn.noise( 5*x,  5*y,  5*z) * 4;
				double b = pn.noise(13*x, 13*y, 13*z) * 1;

				// average of noises ("octaves")
				double n = (a+b) / 5;

				// make an uint8_t in range 0-255 from n
				uint8_t p = floor(255 * n);

				// posterize
				if (false) {
					int numcolors = 9;
					p = rt::map(p, 0, 255, 0, numcolors); // narrow down
					p = rt::map(p, 0, numcolors, 0, 255); // stretch back
				}

				// Wood like structure
				if (false) {
					n = 20 * pn.noise(x, y, z);
					n = n - floor(n);
					p = floor(255*n);
				}

				rt::RGBAColor color = rt::RGBAColor(p, p, p, 255);
				pixelbuffer.setPixel(j, i, color);
			}
		}
		// pixelbuffer.blur();
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
	MyApp application(128, 128, 4);

	while (!application.quit())
	{
		application.run();
	}

	return 0;
}
