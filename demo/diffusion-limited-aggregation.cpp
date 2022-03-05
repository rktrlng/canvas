/**
 * @file diffusion-limited-aggregation.cpp
 *
 * @brief diffusion-limited-aggregation implementation
 *
 * Copyright 2022 @rktrlng
 * https://github.com/rktrlng/canvas
 */

#include <ctime>
#include <vector>

#include <canvas/application.h>

const float ROT_SPEED = 0.01f; // color rotation every second
const int MAX_ELEMENTS = 10000;
const int EDGE = 5; // save image if tree is EDGE pixels from edges

struct Element
{
	rt::vec2i position;
	rt::RGBAColor color;
	bool fixed;

	Element(int x, int y)
	{
		position = rt::vec2i(x, y);
		color = RED;
		fixed = false;
	}

	void move()
	{
		rt::vec2i delta = rt::vec2i((rand()%3) - 1, (rand()%3) - 1);
		position += delta;
	}
};


class MyApp : public cnv::Application
{
private:
	std::vector<Element*> m_elements;

public:
	MyApp(uint16_t width, uint16_t height, uint8_t bitdepth, uint8_t factor) : cnv::Application(width, height, bitdepth, factor)
	{
		std::srand(std::time(nullptr));
		init();
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
		float maxtime = 0.01667f - deltatime;
		frametime += deltatime;
		if (frametime >= maxtime) {
			handleElements(deltatime);

			layers[0]->lock();
			frametime = 0.0f;
		}

		static float counttime = 0.0f;
		float countmaxtime = 1.0f - deltatime;
		counttime += deltatime;
		if (counttime >= countmaxtime) {
			rotateColors();
			int count = countFreeElements();
			// std::cout << count << " free elements" << std::endl;
			if (count == 0) {
				init();
			}
			counttime = 0.0f;
		}
	}

	int countFreeElements()
	{
		int count = 0;
		for (size_t i = 0; i < m_elements.size(); i++) {
			if (!m_elements[i]->fixed) { count++; }
		}
		return count;
	}

	bool edgeTouched()
	{
		int cols = layers[0]->pixelbuffer.width();
		int rows = layers[0]->pixelbuffer.height();
		bool touched = false;
		for (size_t i = 0; i < m_elements.size(); i++) {
			if ( m_elements[i]->fixed &&
				(m_elements[i]->position.x < EDGE ||
				m_elements[i]->position.x > cols-EDGE ||
				m_elements[i]->position.y < EDGE ||
				m_elements[i]->position.y > rows-EDGE)
			) {
				touched = true;
			}
		}
		return touched;
	}

private:
	void init()
	{
		std::cout << "Reset: " << MAX_ELEMENTS << " elements" << std::endl;

		layers[0]->pixelbuffer.fill(TRANSPARENT);
		int cols = layers[0]->pixelbuffer.width();
		int rows = layers[0]->pixelbuffer.height();
		
		for (size_t i = 0; i < m_elements.size(); i++)
		{
			delete m_elements[i];
			m_elements[i] = nullptr;
		}
		m_elements.clear();
		
		for (size_t i = 0; i < MAX_ELEMENTS; i++)
		{
			int x = rand()%cols;
			int y = rand()%rows;
			// int x = rt::rand_bm() * cols;
			// int y = rt::rand_bm() * rows;
			m_elements.push_back(new Element(x, y));
		}

		m_elements[0]->position.x = cols / 2;
		m_elements[0]->position.y = rows / 2;
		m_elements[0]->fixed = true;
	}

	void borders(Element* element, int cols, int rows)
	{
		if (element->position.x > cols) {
			element->position.x -= cols;
		}
		if (element->position.x < 0) {
			element->position.x = cols-1;
		}
		if (element->position.y > rows) {
			element->position.y -= rows;
		}
		if (element->position.y < 0) {
			element->position.y = rows-1;
		}
	}

	void handleElements(float deltatime)
	{
		auto& pixelbuffer = layers[0]->pixelbuffer;
		int cols = pixelbuffer.width();
		int rows = pixelbuffer.height();

		pixelbuffer.fill(TRANSPARENT);

		// draw fixed tree
		for (size_t i = 0; i < m_elements.size(); i++) {
			if (m_elements[i]->fixed) {
				pixelbuffer.setPixel(m_elements[i]->position.x, m_elements[i]->position.y, m_elements[i]->color);
			}
		}

		// if almost touches edge, save file
		if (edgeTouched()) {
			static int count = 0;
			std::string filename = pixelbuffer.createFilename("difflimagg", count);
			pixelbuffer.write(filename);
			std::cout << "write " << filename << std::endl;
			count++;
			init();
		}

		// move nonfixed elements and set status
		for (size_t i = 0; i < m_elements.size(); i++) {
			if (m_elements[i]->fixed) {
				continue;
			}

			// Look around
			rt::vec2i pos = m_elements[i]->position;
			for (int y = -1; y < 2; y++) {
				for (int x = -1; x < 2; x++) {
					if (y==0 && x==0) continue;
					rt::vec2i neighbour = rt::vec2i(pos.x + x, pos.y + y);
					rt::RGBAColor color = pixelbuffer.getPixel(neighbour.x, neighbour.y);
					if (color != TRANSPARENT) {
						// we found the tree
						m_elements[i]->fixed = true;
					}
				}
			}

			// move free elements
			if (!m_elements[i]->fixed) {
				m_elements[i]->move();
				borders(m_elements[i], cols, rows);
			}
		}

		// draw moving elements
		for (size_t i = 0; i < m_elements.size(); i++) {
			if (m_elements[i]->fixed) {
				continue;
			}
			pixelbuffer.setPixel(m_elements[i]->position.x, m_elements[i]->position.y, m_elements[i]->color);
		}
	}

	void rotateColors()
	{
		for (size_t i = 0; i < m_elements.size(); i++) {
			if (m_elements[i]->fixed) {
				continue;
			}
			m_elements[i]->color = rt::rotate(m_elements[i]->color, ROT_SPEED);
		}
	}

	void handleInput()
	{
		if (input.getKeyDown(cnv::KeyCode::Space)) {
			std::cout << "spacebar pressed down." << std::endl;
			// layers[0]->pixelbuffer.printInfo();
			init();
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
	MyApp application(320, 320, 32, 3);
	application.hideMouse();

	while (!application.quit())
	{
		application.run();
	}

	return 0;
}
