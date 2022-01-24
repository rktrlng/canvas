/**
 * @file flowfield.cpp
 *
 * @brief Flowfield implementation
 *
 * Copyright 2022 @rktrlng
 * https://github.com/rktrlng/canvas
 */

#include <ctime>
#include <deque>

#include <canvas/application.h>
#include <canvas/noise.h>

class MyApp : public rt::Application
{
private:
	rt::PerlinNoise pn;
	std::vector<pb::vec2f> field;
	std::deque<pb::vec2f> particles;
	size_t flowscale = 8;
	const size_t maxparticles = 1000;
	const double zspeed = 0.001; // z-noise change
	const int pspeed = 50; // particle speed
public:
	MyApp(uint16_t width, uint16_t height, uint8_t bitdepth, uint8_t factor) : rt::Application(width, height, bitdepth, factor)
	{
		srand((unsigned)time(nullptr));

		unsigned int seed = rand()%1000;
		// unsigned int seed = 42;
		pn = rt::PerlinNoise(seed);

		rt::Canvas* particleCanvas = new rt::Canvas(width, height, bitdepth, factor);
		layers.push_back(particleCanvas);
		particleCanvas->pixelbuffer.fill(BLACK);
	}

	// MyApp(rt::PixelBuffer& pixelbuffer, uint8_t factor) : rt::Application(pixelbuffer, factor)
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
			updateFlowField();
			handleParticles(frametime);

			layers[0]->lock();
			layers[1]->lock();

			frametime = 0.0f;
		}
	}

private:
	void handleParticles(float deltatime)
	{
		auto& pixelbuffer = layers[1]->pixelbuffer;
		int rows = pixelbuffer.header().height;
		int cols = pixelbuffer.header().width;

		// update positions
		for (size_t i = 0; i < particles.size(); i++) {
			pb::vec2f particle = particles[i];

			int flowindex = pb::idFromPos(particle.x/flowscale, particle.y/flowscale, cols/flowscale);
			particle.x += field[flowindex].x * deltatime * pspeed;
			particle.y += field[flowindex].y * deltatime * pspeed;

			if (particle.x < 0) particle.x = cols-1;
			if (particle.x > cols) particle.x = 0;
			if (particle.y < 0) particle.y = rows-1;
			if (particle.y > rows) particle.y = 0;

			particles[i] = particle;
		}

		// draw particles
		for (size_t i = 0; i < particles.size(); i++) {
			pb::vec2f particle = particles[i];
			pixelbuffer.setPixel(particle.x, particle.y, WHITE);
		}

		// handle number of particles
		pb::vec2f p = { pb::rand_float()*cols, pb::rand_float()*rows };
		// pb::vec2f p = { pb::rand_bm()*cols, pb::rand_bm()*rows };
		// pb::vec2f p = { (float)cols/2, (float)rows/2 };

		particles.push_back(p);
		if (particles.size() > maxparticles) {
			particles.pop_front();
		}

		pixelbuffer.blur();
	}
	
	void updateFlowField()
	{
		field.clear();

		auto& pixelbuffer = layers[0]->pixelbuffer;
		size_t rows = pixelbuffer.header().height;
		size_t cols = pixelbuffer.header().width;

		// find min + max
		std::vector<pb::RGBAColor> colors = pixelbuffer.pixels();
		uint8_t min = 255;
		uint8_t max = 0;
		for (size_t i = 0; i < colors.size(); i++) {
			if (colors[i].r < min) min = colors[i].r;
			if (colors[i].r > max) max = colors[i].r;
		}

		// noise to field values
		for (size_t y = 0; y < rows; y += flowscale) {
			for (size_t x = 0; x < cols; x += flowscale) {
				pb::RGBAColor color = pixelbuffer.getPixel(x+flowscale/2, y+flowscale/2);
				float angle = pb::map(color.r, min, max, -3.1415926f, 3.1415926f);
				pb::vec2f vec = pb::vec2f::fromAngle(angle);
				field.push_back(vec);
			}
		}
	}

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

		for (size_t i = 0; i < octaves.size(); i++) {
			int freq = octaves[i].freq;
			int mult = octaves[i].multiplier;
			double a = pn.noise(x*freq, y*freq, z*freq) * mult;
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
		z += zspeed;

		size_t rows = pixelbuffer.header().height;
		size_t cols = pixelbuffer.header().width;
		for (size_t i = 0; i < rows; i++) {
			for (size_t j = 0; j < cols; j++) {
				double x = (double)j/((double)cols);
				double y = (double)i/((double)rows);
				// double z = 0.0f;

				std::vector<Octave> octaves; // { frequency, multiplier }
				// octaves.push_back( { 1, 32} );
				// octaves.push_back( { 2, 16} );
				// octaves.push_back( { 4, 8} );
				octaves.push_back( { 8, 4} );
				octaves.push_back( {16, 2} );
				octaves.push_back( {32, 1} );
				double n = coherentNoise(x, y, z, octaves);

				uint8_t p = 255 * n;

				// posterize
				if (false) {
					int numcolors = 9;
					p = pb::map(p, 0, 255, 0, numcolors); // narrow down
					p = pb::map(p, 0, numcolors, 0, 255); // stretch back
				}

				// Wood like structure
				if (false) {
					n = 20 * pn.noise(x, y, z);
					n = n - floor(n);
					p = 255 * n;
				}

				pb::RGBAColor color = pb::RGBAColor(p, p, p, 255);
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
	MyApp application(320, 320, 32, 3); // width, height, bitdepth, factor

	while (!application.quit())
	{
		application.run();
	}

	return 0;
}
