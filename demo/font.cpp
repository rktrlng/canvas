/**
 * @file font.cpp
 *
 * @brief Apple ][ Font Demo
 *
 * Copyright 2021-2022 @rktrlng
 * https://github.com/rktrlng/canvas
 */

#include <ctime>

#include <canvas/application.h>

class MyApp : public cnv::Application
{
public:
	MyApp(uint16_t width, uint16_t height, uint8_t bitdepth, uint8_t factor) : cnv::Application(width, height, bitdepth, factor)
	{
		layers[0]->pixelbuffer.fill(BLACK);
		fillGlyphs();
		font();
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
		float maxtime = 0.5f;
		frametime += deltatime;
		if (frametime >= maxtime)
		{
			auto& pixelbuffer = layers[0]->pixelbuffer;
			pixelbuffer.fill(BLACK);
			font();
			layers[0]->lock();

			frametime = 0.0f;
		}
	}

private:
	std::vector<rt::PixelBuffer> glyphs;
	void font()
	{
		static bool cursor = true;
		drawText(10, 10,"]LIST");
		drawText(10, 20,"10 PRINT \"HELLO WORLD!\"");
		drawText(10, 30,"20 GOTO 10");
		drawText(10, 40,"]RUN");
		drawText(10, 50,"HELLO WORLD!");
		drawText(10, 60,"HELLO WORLD!");
		drawText(10, 70,"HELLO WORLD!");
		drawText(10, 80,"HELLO WORLD!");
		drawText(10, 90,"HELLO WORLD!");
		drawText(10,100,"HELLO WORLD!");
		drawText(10,110,"HELLO WORLD!");
		drawText(10,120,"HELLO WORLD!");
		drawText(10,130,"HELLO WORLD!");
		drawText(10,140,"^C");
		drawText(10,150,"BREAK IN 20");
		if (cursor)
		{
			drawText(10,160,"]@");
			cursor = false;
		} else {
			drawText(10,160,"]");
			cursor = true;
		}
	}

	void drawText(int x, int y, const std::string& text)
	{
		auto& pixelbuffer = layers[0]->pixelbuffer;
		for (size_t i = 0; i < text.length(); i++)
		{
			pixelbuffer.paste(glyphs[text[i]-32], (i*6)+x, y);
		}
	}

	void fillGlyphs()
	{
		rt::PixelBuffer font = rt::PixelBuffer("assets/applefont.pbf");
		for (size_t i = 0; i < 512; i += 6)
		{
			rt::PixelBuffer character = font.copy(i,0,6,8);
			glyphs.push_back(character);
		}
		
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
	MyApp application(240, 180, 8, 4);

	while (!application.quit())
	{
		application.run();
	}

	return 0;
}
