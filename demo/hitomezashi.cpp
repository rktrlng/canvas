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
	const uint8_t STEP = 5;
	size_t m_counter = 0;
	std::vector<bool> m_xstitch;
	std::vector<bool> m_ystitch;
public:
	MyApp(uint16_t width, uint16_t height, uint8_t bitdepth, uint8_t factor) : cnv::Application(width, height, bitdepth, factor)
	{
		std::srand(std::time(nullptr));

		auto& pixelbuffer = layers[0]->pixelbuffer;
		size_t cols = pixelbuffer.header().width;
		size_t rows = pixelbuffer.header().height;
		m_xstitch = randomSequence(cols/STEP);
		m_ystitch = randomSequence(rows/STEP);

		hitomezashi();
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
		size_t amount_h = layers[0]->pixelbuffer.header().width / STEP;
		size_t amount_v = layers[0]->pixelbuffer.header().height / STEP;
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
		for (size_t y = 0; y < m_ystitch.size(); y++) {
			size_t xpos = 0;
			if (m_ystitch[y]) { xpos += STEP; }
			for (size_t x = 0; x < cols; x+=STEP) {
				pixelbuffer.drawLine(xpos+1, ypos, xpos+STEP-1, ypos, BLACK);
				xpos += STEP*2;
			}
			ypos += STEP;
		}

		// vertical stitches
		size_t xpos = 0;
		for (size_t x = 0; x < m_xstitch.size(); x++) {
			size_t ypos = 0;
			if (m_xstitch[x]) { ypos += STEP; }
			for (size_t y = 0; y < rows; y+=STEP) {
				pixelbuffer.drawLine(xpos, ypos+1, xpos, ypos+STEP-1, BLACK);
				ypos += STEP*2;
			}
			xpos += STEP;
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
			std::string filename = layers[0]->pixelbuffer.createFilename("hitomezashi", m_counter);
			std::cout << filename << std::endl;
			layers[0]->pixelbuffer.write(filename);
			m_counter++;
		}

		if (input.getKeyDown(cnv::KeyCode::R)) {
			m_xstitch = randomSequence(layers[0]->pixelbuffer.header().width / STEP);
			m_ystitch = randomSequence(layers[0]->pixelbuffer.header().height / STEP);
		}

		if (input.getKeyDown(cnv::KeyCode::M)) { // magic!
			//m_ xstitch = repeatSequence(26);
			m_xstitch = repeatSequence((uint64_t)0b11010);

			// m_ystitch = repeatSequence(98);
			// m_ystitch = repeatSequence(72930);
			m_ystitch = repeatSequence((uint64_t)0b01000111001110001);
		}

		if (input.getKeyDown(cnv::KeyCode::Alpha0)) { m_xstitch = repeatSequence(0);}
		if (input.getKeyDown(cnv::KeyCode::Alpha1)) { m_xstitch = repeatSequence(1);}
		if (input.getKeyDown(cnv::KeyCode::Alpha2)) { m_xstitch = repeatSequence(2);}
		if (input.getKeyDown(cnv::KeyCode::Alpha3)) { m_xstitch = repeatSequence(1);}
		if (input.getKeyDown(cnv::KeyCode::Alpha4)) { m_xstitch = repeatSequence(4);}
		if (input.getKeyDown(cnv::KeyCode::Alpha5)) { m_xstitch = repeatSequence(5);}
		if (input.getKeyDown(cnv::KeyCode::Alpha6)) { m_xstitch = repeatSequence(6);}
		if (input.getKeyDown(cnv::KeyCode::Alpha7)) { m_xstitch = repeatSequence(1);}
		if (input.getKeyDown(cnv::KeyCode::Alpha8)) { m_xstitch = repeatSequence(8);}
		if (input.getKeyDown(cnv::KeyCode::Alpha9)) { m_xstitch = repeatSequence(9);}

		if (input.getKeyDown(cnv::KeyCode::F10)) { m_ystitch = repeatSequence(0);}
		if (input.getKeyDown(cnv::KeyCode::F1))  { m_ystitch = repeatSequence(1);}
		if (input.getKeyDown(cnv::KeyCode::F2))  { m_ystitch = repeatSequence(2);}
		if (input.getKeyDown(cnv::KeyCode::F3))  { m_ystitch = repeatSequence(1);}
		if (input.getKeyDown(cnv::KeyCode::F4))  { m_ystitch = repeatSequence(4);}
		if (input.getKeyDown(cnv::KeyCode::F5))  { m_ystitch = repeatSequence(5);}
		if (input.getKeyDown(cnv::KeyCode::F6))  { m_ystitch = repeatSequence(6);}
		if (input.getKeyDown(cnv::KeyCode::F7))  { m_ystitch = repeatSequence(1);}
		if (input.getKeyDown(cnv::KeyCode::F8))  { m_ystitch = repeatSequence(8);}
		if (input.getKeyDown(cnv::KeyCode::F9))  { m_ystitch = repeatSequence(9);}

		if (input.getKeyDown(cnv::KeyCode::X)) {
			for (size_t i = 0; i < m_xstitch.size(); i++){
				std::cout << m_xstitch[i] << " ";
			}
			std::cout << std::endl;
		}
		if (input.getKeyDown(cnv::KeyCode::Y)) {
			for (size_t i = 0; i < m_ystitch.size(); i++){
				std::cout << m_ystitch[i] << " ";
			}
			std::cout << std::endl;
		}

		if (input.getMouseDown(0)) {
			int x = (int) input.getMouseX();
			int y = (int) input.getMouseY();

			if (x%STEP == 0) { // column
				size_t index = x/STEP;
				m_xstitch[index] = !m_xstitch[index];
			}

			if (y%STEP == 0) { // row
				size_t index = y/STEP;
				m_ystitch[index] = !m_ystitch[index];
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
