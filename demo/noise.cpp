/**
 * @file noise.cpp
 *
 * @brief Perlin Noise implementation
 *
 * Copyright 2021-2022 @rktrlng
 * https://github.com/rktrlng/canvas
 */

#include <ctime>

#include <canvas/application.h>
#include <canvas/noise.h>

class MyApp : public cnv::Application
{
private:
	cnv::PerlinNoise m_pn;
public:
	MyApp(uint16_t width, uint16_t height, uint8_t bitdepth, uint8_t factor) : cnv::Application(width, height, bitdepth, factor)
	{
		srand((unsigned)time(nullptr));

		unsigned int seed = rand()%1000;
		// unsigned int seed = 42;
		m_pn = cnv::PerlinNoise(seed);
	}

	// MyApp(rt::PixelBuffer& pixelbuffer, uint8_t factor) : cnv::Application(pixelbuffer, factor)
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
		float maxtime = 0.05f;
		frametime += deltatime;
		if (frametime >= maxtime)
		{
			noise();
			layers[0]->lock();

			frametime = 0.0f;
		}
	}

private:
	struct Octave
	{
		int freq = 1;
		int multiplier = 1;
		Octave(int f, int m) : freq(f), multiplier(m) { }
	};

	double coherentNoise(double x, double y, double z, const std::vector<Octave>& octaves)
	{
		double n = 0.0;
		double div = 0.0;

		for (size_t i = 0; i < octaves.size(); i++)
		{
			int freq = octaves[i].freq;
			int mult = octaves[i].multiplier;
			double a = m_pn.noise(x*freq, y*freq, z*freq) * mult;
			n += a;
			div += mult;
		}
		n /= div;

		return n;
	}

	void noise()
	{
		auto& pixelbuffer = layers[0]->pixelbuffer;

		static double z = 0.0f;
		z += 0.005f;

		size_t rows = pixelbuffer.height();
		size_t cols = pixelbuffer.width();
		for (size_t i = 0; i < rows; i++) {
			for (size_t j = 0; j < cols; j++) {
				double x = (double)j/((double)cols);
				double y = (double)i/((double)rows);
				// double z = 0.0f;

				std::vector<Octave> octaves; // { frequency, multiplier }
				// octaves.push_back( { 1, 32} );
				octaves.push_back( { 2, 16} );
				octaves.push_back( { 4, 8} );
				octaves.push_back( { 8, 4} );
				octaves.push_back( {16, 2} );
				// octaves.push_back( {32, 1} );
				double n = coherentNoise(x, y, z, octaves);

				uint8_t p = 255 * n;

				// Wood like structure
				if (false) {
					n = 20 * m_pn.noise(x, y, z);
					n = n - floor(n);
					p = 255 * n;
				}

				rt::RGBAColor color = rt::RGBAColor(p, p, p, 255);
				pixelbuffer.setPixel(j, i, color);
			}
		}
		// pixelbuffer.blur();
		pixelbuffer.contrast_8();
		pixelbuffer.posterize_8(10);
	}

	void handleInput() {
		if (input.getKeyDown(cnv::KeyCode::Space)) {
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
	MyApp application(128, 128, 8, 4); // width, height, bitdepth, factor

	while (!application.quit())
	{
		application.run();
	}

	return 0;
}
