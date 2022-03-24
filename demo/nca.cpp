/**
 * @file nca.cpp
 *
 * @brief neural cellular automata
 *
 * Copyright 2022 @rktrlng
 * https://github.com/rktrlng/canvas
 */

#include <ctime>

#include <canvas/application.h>

#include <pixelbuffer/math/mat3.h>

class Convolution
{
public:
	rt::mat3 filter;
	virtual float activation(float x) = 0;
};


class GameOfLife : public Convolution
{
public:
	GameOfLife() : Convolution()
	{
		filter[0] = {1,1,1};
		filter[1] = {1,9,1};
		filter[2] = {1,1,1};
	}

	float activation(float x) override
	{
		// if(x == 3.0f || x == 11.0f || x == 12.0f) {
		if(x == 3.0f || x == 11.0f || x == 12.0f || x == 4.0f) {
			return 1.0f;
		}
		return 0.0f;
	}
};


class Slime : public Convolution
{
public:
	Slime() : Convolution()
	{
		filter[0] = { 0.80f, -0.85f,  0.80f};
		filter[1] = {-0.85f, -0.20f, -0.85f};
		filter[2] = { 0.80f, -0.85f,  0.80f};
	}

	float activation(float x) override
	{
		return -1.0f/(0.89f*pow(x, 2.0f)+1.0f)+1.0f;
	}
	
};



class Pathways : public Convolution
{
public:
	Pathways() : Convolution()
	{
		filter[0] = { 0.0f, 1.0f, 0.0f};
		filter[1] = { 1.0f, 1.0f, 1.0f};
		filter[2] = { 0.0f, 1.0f, 0.0f};
	}

	float gaussian(float x, float b)
	{
		return 1.0f/pow(2.0f, (pow(x-b, 2.0f)));
	}

	float activation(float x) override
	{
		return gaussian(x, 3.5f);
	}

};


class MyApp : public cnv::Application
{
private:
	uint16_t cols;
	uint16_t rows;
	std::vector<float> values;

	// GameOfLife convolution;
	// Slime convolution;
	Pathways convolution;

public:
	MyApp(uint16_t width, uint16_t height, uint8_t bitdepth, uint8_t factor) : cnv::Application(width, height, bitdepth, factor)
	{
		std::srand(std::time(nullptr));

		cols = width;
		rows = height;

		init();
	}

	// MyApp(rt::PixelBuffer& pixelbuffer, uint8_t factor) : cnv::Application(pixelbuffer, factor)
	// {
	// 
	// }

	virtual ~MyApp()
	{
		
	}

	void init()
	{
		values.clear();
		for (size_t i = 0; i < (cols*rows); i++) {
			values.push_back(rand()%2);
		}
	}

	void update(float deltatime) override
	{
		handleInput();

		static float frametime = 0.0f;
		float maxtime = 0.2f - deltatime;
		frametime += deltatime;
		if (frametime >= maxtime) {
			updatePixels(convolution);
			frametime = 0.0f;
		}
	}

private:
	void updatePixels(const Convolution& conv)
	{
		std::vector<float> next;

		auto& pixelbuffer = layers[0]->pixelbuffer;

		for (size_t y = 0; y < rows; y++) {
			for (size_t x = 0; x < cols; x++) {
				// run filter on pixels
				float value = 0.0f;
				for (int j = -1; j < 2; j++) {
					for (int i = -1; i < 2; i++) {
						rt::vec2i wrapped = rt::wrap(rt::vec2i(x+i, y+j), cols, rows);
						size_t index = rt::index(wrapped.x, wrapped.y, cols);
						value += values[index];
					}
				}

				float new_value = convolution.activation(value);
				next.push_back(new_value);

				// test
				// size_t index = rt::index(x, y, cols);
				// values[index] = new_value;

				uint8_t gray = new_value * 255;
				rt::RGBAColor color = {gray, gray, gray, 255};
				
				pixelbuffer.setPixel(x, y, color);
			}
		}

		values = next;

		// pixelbuffer.blur();
		layers[0]->lock();
	}

	void handleInput()
	{
		if (input.getKeyDown(cnv::KeyCode::Space)) {
			std::cout << "spacebar pressed down." << std::endl;
			layers[0]->pixelbuffer.printInfo();
			init();
		}

		if (input.getMouse(0)) {
			size_t size = 3;
			int x = (int) input.getMouseX();
			int y = (int) input.getMouseY();
			for (size_t j = 0; j < size; j++) {
				for (size_t i = 0; i < size; i++) {
					int index = rt::index(i+x,j+y,cols);
					values[index] = rand()%2;
				}
			}
			// std::cout << "click " << x << "," << y << std::endl;
		}

		int scrolly = input.getScrollY();
		if (scrolly != 0) {
			std::cout << "scroll: " << scrolly << std::endl;
		}
	}

};


int main( void )
{
	MyApp application(320, 180, 24, 4);

	while (!application.quit())
	{
		application.run();
	}

	return 0;
}
