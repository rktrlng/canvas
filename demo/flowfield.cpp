/**
 * @file flowm_field.cpp
 *
 * @brief Flowm_field implementation
 *
 * Copyright 2022 @rktrlng
 * https://github.com/rktrlng/canvas
 */

#include <ctime>
#include <deque>

#include <canvas/application.h>
#include <canvas/noise.h>

class MyApp : public cnv::Application
{
private:
	cnv::PerlinNoise m_pn;
	std::vector<rt::vec2f> m_field;
	std::deque<rt::vec2f> m_particles;
	size_t m_flowscale = 8;
	const size_t MAXPARTICLES = 2500;
	const double ZSPEED = 0.001; // z-noise change
	const int PSPEED = 50; // particle speed
	const int AT_ONCE = 6; // # of particles to spawn per tick
public:
	MyApp(uint16_t width, uint16_t height, uint8_t bitdepth, uint8_t factor) : cnv::Application(width, height, bitdepth, factor)
	{
		srand((unsigned)time(nullptr));

		unsigned int seed = rand()%1000;
		// unsigned int seed = 42;
		m_pn = cnv::PerlinNoise(seed);

		cnv::Canvas* particleCanvas = new cnv::Canvas(width, height, bitdepth, factor);
		layers.push_back(particleCanvas);
		particleCanvas->pixelbuffer.fill(BLACK);

		// fill list of particles half way
		for (size_t i = 0; i < MAXPARTICLES/2; i++) {
			rt::vec2f p = { rt::rand_float()*width, rt::rand_float()*height };
			m_particles.push_back(p);
		}
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
			updateFlowm_field();
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
		int rows = pixelbuffer.height();
		int cols = pixelbuffer.width();

		// update positions
		for (size_t i = 0; i < m_particles.size(); i++) {
			auto& particle = m_particles[i];

			int flowindex = rt::index(particle.x/m_flowscale, particle.y/m_flowscale, cols/m_flowscale);
			particle.x += m_field[flowindex].x * deltatime * PSPEED;
			particle.y += m_field[flowindex].y * deltatime * PSPEED;

			if (particle.x < 0) particle.x = cols-1;
			if (particle.x > cols) particle.x = 0;
			if (particle.y < 0) particle.y = rows-1;
			if (particle.y > rows) particle.y = 0;
		}

		// draw particles
		for (size_t i = 0; i < m_particles.size(); i++) {
			rt::vec2f particle = m_particles[i];
			pixelbuffer.setPixel(particle.x, particle.y, WHITE);
		}

		// handle number of particles
		for (int i = 0; i < AT_ONCE; i++) {
			rt::vec2f p = { rt::rand_float()*cols, rt::rand_float()*rows };
			// rt::vec2f p = { rt::rand_bm()*cols, rt::rand_bm()*rows };
			// rt::vec2f p = { (float)cols/2, (float)rows/2 };

			m_particles.push_back(p);

			if (m_particles.size() > MAXPARTICLES) {
				m_particles.pop_front();
			}
		}

		pixelbuffer.blur();

		// Colorize
		for (size_t i = 0; i < pixelbuffer.pixels().size(); i++) {
			rt::HSVAColor hsva = rt::RGBA2HSVA(pixelbuffer[i]);
			hsva.h = 0.999f - hsva.v;
			hsva.s = 1;
			pixelbuffer[i] = rt::HSVA2RGBA(hsva);
		}
	}
	
	void updateFlowm_field()
	{
		m_field.clear();

		auto& pixelbuffer = layers[0]->pixelbuffer;
		size_t rows = pixelbuffer.height();
		size_t cols = pixelbuffer.width();

		// find min + max
		auto& colors = pixelbuffer.pixels();
		uint8_t min = 255;
		uint8_t max = 0;
		for (size_t i = 0; i < colors.size(); i++) {
			if (colors[i].r < min) min = colors[i].r;
			if (colors[i].r > max) max = colors[i].r;
		}

		// noise to m_field values
		for (size_t y = 0; y < rows; y += m_flowscale) {
			for (size_t x = 0; x < cols; x += m_flowscale) {
				rt::RGBAColor color = pixelbuffer.getPixel(x+m_flowscale/2, y+m_flowscale/2);
				float angle = rt::map(color.r, min, max, -3.1415926f*1.9f, 3.1415926f*1.9f);
				rt::vec2f vec = rt::vec2f::fromAngle(angle);
				m_field.push_back(vec);
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
		z += ZSPEED;

		size_t rows = pixelbuffer.height();
		size_t cols = pixelbuffer.width();
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
					p = rt::map(p, 0, 255, 0, numcolors); // narrow down
					p = rt::map(p, 0, numcolors, 0, 255); // stretch back
				}

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
	MyApp application(320, 320, 32, 3); // width, height, bitdepth, factor

	while (!application.quit())
	{
		application.run();
	}

	return 0;
}
