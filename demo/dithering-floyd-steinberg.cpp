/**
 * @file dithering-floyd-steinberg.cpp
 *
 * @brief Dithering: Floyd-Steinberg
 *
 * Copyright 2021-2022 @rktrlng
 * https://github.com/rktrlng/canvas
 */

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
		luminance();
		floyd_steinberg(1);

		// draw black border around image
		uint16_t width = layers[0]->pixelbuffer.width();
		uint16_t height = layers[0]->pixelbuffer.height();
		layers[0]->pixelbuffer.drawSquare(0, 0, width-1, height-1, BLACK);

		// save as pbf file
		layers[0]->pixelbuffer.bitdepth(1);
		layers[0]->pixelbuffer.write("dither.pbf");

		layers[0]->lock();
	}

	virtual ~MyApp()
	{

	}

	void update(float deltatime) override
	{
		handleInput();
	}

private:
	struct Color16 {
		int16_t r = 0;
		int16_t g = 0;
		int16_t b = 0;
		int16_t a = 0; // transparent
		Color16() : r(0), g(0), b(0), a(0) {}
		Color16(int16_t _r, int16_t _g, int16_t _b, int16_t _a) : r(_r), g(_g), b(_b), a(_a) {}
	};

	Color16 quantize_16(Color16 rgba, int factor = 1)
	{
		float r = (float)rgba.r;
		float g = (float)rgba.g;
		float b = (float)rgba.b;

		int16_t nr = round(factor * r / 255) * (255.0f / factor);
		int16_t ng = round(factor * g / 255) * (255.0f / factor);
		int16_t nb = round(factor * b / 255) * (255.0f / factor);

		return Color16(nr, ng, nb, rgba.a);
	}

	// https://en.wikipedia.org/wiki/Floyd%E2%80%93Steinberg_dithering
	void floyd_steinberg(int factor = 1)
	{
		auto& pixelbuffer = layers[0]->pixelbuffer;
		size_t rows = pixelbuffer.height();
		size_t cols = pixelbuffer.width();

		// copy RGBA pixels to Color16 pixels
		std::vector<Color16> values;
		for (size_t i = 0; i < rows*cols; i++) {
			pb::RGBAColor p = pixelbuffer[i];
			Color16 c16 = { p.r, p.g, p.b, p.a };
			values.push_back(c16);
		}

		// floyd-steinberg algorithm:
		// for each y from top to bottom do
		// 	for each x from left to right do
		for (size_t y = 0; y < rows-1; y++) {
			for (size_t x = 1; x < cols-1; x++) {
				size_t index = pb::index(x, y, cols);
				// oldpixel := pixels[x][y]
				Color16 oldpixel = values[index];
				// newpixel := find_closest_palette_color(oldpixel)
				Color16 newpixel = quantize_16(oldpixel, factor);
				// pixels[x][y] := newpixel
				Color16 c16;
				c16.r = newpixel.r;
				c16.g = newpixel.g;
				c16.b = newpixel.b;
				c16.a = newpixel.a;
				values[index] = c16;
				// quant_error := oldpixel - newpixel
				float errorR = oldpixel.r - newpixel.r;
				float errorG = oldpixel.g - newpixel.g;
				float errorB = oldpixel.b - newpixel.b;

				// pixels[x + 1][y    ] := pixels[x + 1][y    ] + quant_error × 7 / 16
				index = pb::index(x+1, y, cols);
				Color16 east = values[index];
				c16.r = round(east.r + errorR * (7.0 / 16.0));
				c16.g = round(east.g + errorG * (7.0 / 16.0));
				c16.b = round(east.b + errorB * (7.0 / 16.0));
				values[index] = c16;
				
				// pixels[x - 1][y + 1] := pixels[x - 1][y + 1] + quant_error × 3 / 16
				index = pb::index(x-1, y+1, cols);
				Color16 southwest = values[index];
				c16.r = round(southwest.r + errorR * (3.0 / 16.0));
				c16.g = round(southwest.g + errorG * (3.0 / 16.0));
				c16.b = round(southwest.b + errorB * (3.0 / 16.0));
				values[index] = c16;

				// pixels[x    ][y + 1] := pixels[x    ][y + 1] + quant_error × 5 / 16
				index = pb::index(x, y+1, cols);
				Color16 south = values[index];
				c16.r = round(south.r + errorR * (5.0 / 16.0));
				c16.g = round(south.g + errorG * (5.0 / 16.0));
				c16.b = round(south.b + errorB * (5.0 / 16.0));
				values[index] = c16;

				// pixels[x + 1][y + 1] := pixels[x + 1][y + 1] + quant_error × 1 / 16
				index = pb::index(x+1, y+1, cols);
				Color16 southeast = values[index];
				c16.r = round(southeast.r + errorR * (1.0 / 16.0));
				c16.g = round(southeast.g + errorG * (1.0 / 16.0));
				c16.b = round(southeast.b + errorB * (1.0 / 16.0));
				values[index] = c16;
			}
		}

		// copy Color16 pixels to RGBA pixels
		for (size_t i = 0; i < values.size(); i++) {
			Color16 c16 = values[i];
			if (c16.r > 255) { c16.r = 255; } if (c16.r < 0) { c16.r = 0; }
			if (c16.g > 255) { c16.g = 255; } if (c16.g < 0) { c16.g = 0; }
			if (c16.b > 255) { c16.b = 255; } if (c16.b < 0) { c16.b = 0; }
			pb::RGBAColor color;
			color.r = static_cast<uint8_t>(c16.r);
			color.g = static_cast<uint8_t>(c16.g);
			color.b = static_cast<uint8_t>(c16.b);
			color.a = pixelbuffer[i].a;
			pixelbuffer[i] = color;
		}
	}

	void luminance()
	{
		auto& pixelbuffer = layers[0]->pixelbuffer;
		for (size_t i = 0; i < pixelbuffer.pixels().size(); i++) {
			pixelbuffer.pixels()[i] = pb::luminance(pixelbuffer.pixels()[i]);
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

};


int main( void )
{
	pb::PixelBuffer pixelbuffer("assets/milkmaid.pbf");
	MyApp application(pixelbuffer, 3, true);

	while (!application.quit())
	{
		application.run();
	}

	return 0;
}
