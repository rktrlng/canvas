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
		grayscale();
		floyd_steinberg();

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
	// https://en.wikipedia.org/wiki/Floyd%E2%80%93Steinberg_dithering
	void floyd_steinberg()
	{
		auto& pixelbuffer = layers[0]->pixelbuffer;
		size_t rows = pixelbuffer.height();
		size_t cols = pixelbuffer.width();
		for (size_t y = 0; y < rows-1; y++) {
			for (size_t x = 1; x < cols-1; x++) {
				// oldpixel := pixels[x][y]
				// newpixel := find_closest_palette_color(oldpixel)
				// pixels[x][y] := newpixel
				// quant_error := oldpixel - newpixel
				// pixels[x + 1][y    ] := pixels[x + 1][y    ] + quant_error × 7 / 16
				// pixels[x - 1][y + 1] := pixels[x - 1][y + 1] + quant_error × 3 / 16
				// pixels[x    ][y + 1] := pixels[x    ][y + 1] + quant_error × 5 / 16
				// pixels[x + 1][y + 1] := pixels[x + 1][y + 1] + quant_error × 1 / 16


				// oldpixel := pixels[x][y]
				pb::RGBAColor oldpixel = pixelbuffer.getPixel(x, y);
				// newpixel := find_closest_palette_color(oldpixel)
				pb::RGBAColor newpixel = pb::Color::quantize(oldpixel);
				// pixels[x][y] := newpixel
				pixelbuffer.setPixel(x, y, newpixel);
				// quant_error := oldpixel - newpixel
				float errorR = oldpixel.r - newpixel.r;
				float errorG = oldpixel.g - newpixel.g;
				float errorB = oldpixel.b - newpixel.b;

				float r, g, b;
				// pixels[x + 1][y    ] := pixels[x + 1][y    ] + quant_error × 7 / 16
				pb::RGBAColor east = pixelbuffer.getPixel(x+1, y);
				r = round(east.r + errorR * (7.0 / 16.0)); if (r > 255) { r = 255; } if (r < 0) { r = 0; }
				g = round(east.g + errorG * (7.0 / 16.0)); if (g > 255) { g = 255; } if (g < 0) { g = 0; }
				b = round(east.b + errorB * (7.0 / 16.0)); if (b > 255) { b = 255; } if (b < 0) { b = 0; }
				pixelbuffer.setPixel(x+1, y, pb::RGBAColor(r, g, b, east.a));
				
				// pixels[x - 1][y + 1] := pixels[x - 1][y + 1] + quant_error × 3 / 16
				pb::RGBAColor southwest = pixelbuffer.getPixel(x-1, y+1);
				r = round(southwest.r + errorR * (3.0 / 16.0)); if (r > 255) { r = 255; } if (r < 0) { r = 0; }
				g = round(southwest.g + errorG * (3.0 / 16.0)); if (g > 255) { g = 255; } if (g < 0) { g = 0; }
				b = round(southwest.b + errorB * (3.0 / 16.0)); if (b > 255) { b = 255; } if (b < 0) { b = 0; }
				pixelbuffer.setPixel(x-1, y+1, pb::RGBAColor(r, g, b, southwest.a));

				// pixels[x    ][y + 1] := pixels[x    ][y + 1] + quant_error × 5 / 16
				pb::RGBAColor south = pixelbuffer.getPixel(x, y+1);
				r = round(south.r + errorR * (5.0 / 16.0)); if (r > 255) { r = 255; } if (r < 0) { r = 0; }
				g = round(south.g + errorG * (5.0 / 16.0)); if (g > 255) { g = 255; } if (g < 0) { g = 0; }
				b = round(south.b + errorB * (5.0 / 16.0)); if (b > 255) { b = 255; } if (b < 0) { b = 0; }
				pixelbuffer.setPixel(x, y+1, pb::RGBAColor(r, g, b, south.a));

				// pixels[x + 1][y + 1] := pixels[x + 1][y + 1] + quant_error × 1 / 16
				pb::RGBAColor southeast = pixelbuffer.getPixel(x+1, y+1);
				r = round(southeast.r + errorR * (1.0 / 16.0)); if (r > 255) { r = 255; } if (r < 0) { r = 0; }
				g = round(southeast.g + errorG * (1.0 / 16.0)); if (g > 255) { g = 255; } if (g < 0) { g = 0; }
				b = round(southeast.b + errorB * (1.0 / 16.0)); if (b > 255) { b = 255; } if (b < 0) { b = 0; }
				pixelbuffer.setPixel(x+1, y+1, pb::RGBAColor(r, g, b, southeast.a));
			}
		}
	}

	void grayscale()
	{
		auto& pixelbuffer = layers[0]->pixelbuffer;
		for (size_t i = 0; i < pixelbuffer.pixels().size(); i++) {
			pixelbuffer.pixels()[i] = pb::Color::grayscale(pixelbuffer.pixels()[i]);
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
