/**
 * @file hitomezashi.cpp
 *
 * @brief Hitomezashi Stitching Patterns
 *
 * Copyright 2022 @rktrlng
 * https://github.com/rktrlng/canvas
 */

#include <ctime>
#include <deque>

#include <canvas/application.h>

class MyApp : public cnv::Application
{
private:
	const uint8_t step = 5;
	size_t counter = 0;
	std::vector<bool> xstitch;
	std::vector<bool> ystitch;
public:
	MyApp(uint16_t width, uint16_t height, uint8_t bitdepth, uint8_t factor) : cnv::Application(width, height, bitdepth, factor)
	{
		std::srand(std::time(nullptr));

		auto& pixelbuffer = layers[0]->pixelbuffer;
		size_t cols = pixelbuffer.header().width;
		size_t rows = pixelbuffer.header().height;
		xstitch = randomSequence(cols/step);
		ystitch = randomSequence(rows/step);

		hitomezashi();
	}

	// MyApp(pb::PixelBuffer& pixelbuffer, uint8_t factor) : cnv::Application(pixelbuffer, factor)
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
		float maxtime = 0.0167f - deltatime;
		frametime += deltatime;
		if (frametime >= maxtime) {
			hitomezashi();
			frametime = 0.0f;
		}
	}

private:
	std::vector<bool> randomSequence(size_t amount)
	{
		std::vector<bool> sequence;
		for (size_t i = 0; i < amount; i++) {
			sequence.push_back(rand()%2);
		}
		return sequence;
	}

	std::vector<bool> repeatSequence(uint64_t value)
	{
		size_t amount_h = layers[0]->pixelbuffer.header().width / step;
		size_t amount_v = layers[0]->pixelbuffer.header().height / step;
		size_t amount = amount_h > amount_v ? amount_h : amount_v;
		std::vector<bool> sequence;
		size_t counter = 0;
		for (size_t i = 0; i < amount; i++) {
			uint64_t n = value;
			do {
				counter++;
				sequence.push_back(n&1);
				n >>= 1;
			} while (n > 0);
			if (counter >= amount) break;
		}
		return sequence;
	}

	void hitomezashi()
	{
		layers[0]->pixelbuffer.fill(WHITE);

		auto& pixelbuffer = layers[0]->pixelbuffer;
		size_t cols = pixelbuffer.header().width;
		size_t rows = pixelbuffer.header().height;

		// horizontal stitches
		size_t ypos = 0;
		for (size_t y = 0; y < ystitch.size(); y++) {
			size_t xpos = 0;
			if (ystitch[y]) { xpos += step; }
			for (size_t x = 0; x < cols; x+=step) {
				pixelbuffer.drawLine(xpos+1, ypos, xpos+step-1, ypos, BLACK);
				xpos += step*2;
			}
			ypos += step;
		}

		// vertical stitches
		size_t xpos = 0;
		for (size_t x = 0; x < xstitch.size(); x++) {
			size_t ypos = 0;
			if (xstitch[x]) { ypos += step; }
			for (size_t y = 0; y < rows; y+=step) {
				pixelbuffer.drawLine(xpos, ypos+1, xpos, ypos+step-1, BLACK);
				ypos += step*2;
			}
			xpos += step;
		}

		// draw mouse cursor
		int x = (int) input.getMouseX();
		int y = (int) input.getMouseY();

		pixelbuffer.setPixel(x-1, y+0, RED);
		pixelbuffer.setPixel(x+1, y+0, RED);
		pixelbuffer.setPixel(x+0, y-1, RED);
		pixelbuffer.setPixel(x+0, y+1, RED);

		layers[0]->lock();
	}

	void handleInput()
	{
		if (input.getKeyDown(cnv::KeyCode::Space)) {
			std::string filename = layers[0]->pixelbuffer.createFilename("hitomezashi", counter);
			std::cout << filename << std::endl;
			layers[0]->pixelbuffer.write(filename);
			counter++;
		}

		if (input.getKeyDown(cnv::KeyCode::R)) {
			xstitch = randomSequence(layers[0]->pixelbuffer.header().width / step);
			ystitch = randomSequence(layers[0]->pixelbuffer.header().height / step);
		}

		if (input.getKeyDown(cnv::KeyCode::M)) { // magic!
			// xstitch = repeatSequence(26);
			xstitch = repeatSequence((uint64_t)0b11010);

			// ystitch = repeatSequence(98);
			// ystitch = repeatSequence(72930);
			ystitch = repeatSequence((uint64_t)0b01000111001110001);
		}

		if (input.getKeyDown(cnv::KeyCode::Alpha0)) { xstitch = repeatSequence(0);}
		if (input.getKeyDown(cnv::KeyCode::Alpha1)) { xstitch = repeatSequence(1);}
		if (input.getKeyDown(cnv::KeyCode::Alpha2)) { xstitch = repeatSequence(2);}
		if (input.getKeyDown(cnv::KeyCode::Alpha3)) { xstitch = repeatSequence(1);}
		if (input.getKeyDown(cnv::KeyCode::Alpha4)) { xstitch = repeatSequence(4);}
		if (input.getKeyDown(cnv::KeyCode::Alpha5)) { xstitch = repeatSequence(5);}
		if (input.getKeyDown(cnv::KeyCode::Alpha6)) { xstitch = repeatSequence(6);}
		if (input.getKeyDown(cnv::KeyCode::Alpha7)) { xstitch = repeatSequence(1);}
		if (input.getKeyDown(cnv::KeyCode::Alpha8)) { xstitch = repeatSequence(8);}
		if (input.getKeyDown(cnv::KeyCode::Alpha9)) { xstitch = repeatSequence(9);}

		if (input.getKeyDown(cnv::KeyCode::F10)) { ystitch = repeatSequence(0);}
		if (input.getKeyDown(cnv::KeyCode::F1)) { ystitch = repeatSequence(1);}
		if (input.getKeyDown(cnv::KeyCode::F2)) { ystitch = repeatSequence(2);}
		if (input.getKeyDown(cnv::KeyCode::F3)) { ystitch = repeatSequence(1);}
		if (input.getKeyDown(cnv::KeyCode::F4)) { ystitch = repeatSequence(4);}
		if (input.getKeyDown(cnv::KeyCode::F5)) { ystitch = repeatSequence(5);}
		if (input.getKeyDown(cnv::KeyCode::F6)) { ystitch = repeatSequence(6);}
		if (input.getKeyDown(cnv::KeyCode::F7)) { ystitch = repeatSequence(1);}
		if (input.getKeyDown(cnv::KeyCode::F8)) { ystitch = repeatSequence(8);}
		if (input.getKeyDown(cnv::KeyCode::F9)) { ystitch = repeatSequence(9);}

		if (input.getKeyDown(cnv::KeyCode::X)) {
			for (size_t i = 0; i < xstitch.size(); i++){
				std::cout << xstitch[i] << " ";
			}
			std::cout << std::endl;
		}
		if (input.getKeyDown(cnv::KeyCode::Y)) {
			for (size_t i = 0; i < ystitch.size(); i++){
				std::cout << ystitch[i] << " ";
			}
			std::cout << std::endl;
		}

		if (input.getMouseDown(0)) {
			int x = (int) input.getMouseX();
			int y = (int) input.getMouseY();

			if (x%step == 0) { // column
				size_t index = x/step;
				xstitch[index] = !xstitch[index];
			}

			if (y%step == 0) { // row
				size_t index = y/step;
				ystitch[index] = !ystitch[index];
			}
		}

		int scrolly = input.getScrollY();
		if (scrolly != 0) {
			std::cout << "scroll: " << scrolly << std::endl;
		}
	}

};


int main( void )
{
	MyApp application(320, 180, 8, 5);
	application.hideMouse();

	while (!application.quit())
	{
		application.run();
	}

	return 0;
}
