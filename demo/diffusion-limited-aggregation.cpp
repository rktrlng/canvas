/**
 * @file particles.cpp
 *
 * @brief particles implementation
 *
 * Copyright 2021-2022 @rktrlng
 * https://github.com/rktrlng/canvas
 */

#include <ctime>
#include <vector>

#include <canvas/application.h>

const float ROT_SPEED = 0.01f;
const int MAX_ELEMENTS = 10000;
const int EDGE = 5;

struct Element
{
	pb::vec2i position;
	pb::RGBAColor color;
	bool fixed;
	int cols;
	int rows;

	Element(int x, int y, int c, int r)
	{
		position = pb::vec2i(x, y);
		color = RED;
		fixed = false;
		cols = c;
		rows = r;
	}

	void move()
	{
		if (!fixed) {
			int dx = (rand()%3) - 1;
			int dy = (rand()%3) - 1;
			position.x += dx;
			position.y += dy;
			// position = pb::wrap(position, cols, rows);
		}
	}
};


class MyApp : public rt::Application
{
private:
	std::vector<Element*> elements;

public:
	MyApp(uint16_t width, uint16_t height, uint8_t bitdepth, uint8_t factor) : rt::Application(width, height, bitdepth, factor)
	{
		std::srand(std::time(nullptr));
		init();
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
		for (size_t i = 0; i < elements.size(); i++) {
			if (!elements[i]->fixed) { count++; }
		}
		return count;
	}

	bool edgeTouched()
	{
		int cols = layers[0]->pixelbuffer.width();
		int rows = layers[0]->pixelbuffer.height();
		bool touched = false;
		for (size_t i = 0; i < elements.size(); i++) {
			if ( elements[i]->fixed &&
				(elements[i]->position.x < EDGE ||
				elements[i]->position.x > cols-EDGE ||
				elements[i]->position.y < EDGE ||
				elements[i]->position.y > rows-EDGE)
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
		
		for (size_t i = 0; i < elements.size(); i++)
		{
			delete elements[i];
			elements[i] = nullptr;
		}
		elements.clear();
		
		for (size_t i = 0; i < MAX_ELEMENTS; i++)
		{
			int x = rand()%cols;
			int y = rand()%rows;
			// int x = pb::rand_bm() * cols;
			// int y = pb::rand_bm() * rows;
			elements.push_back(new Element(x, y, cols, rows));
		}

		elements[0]->position.x = cols / 2;
		elements[0]->position.y = rows / 2;
		elements[0]->fixed = true;
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
		for (size_t i = 0; i < elements.size(); i++) {
			if (elements[i]->fixed) {
				pixelbuffer.setPixel(elements[i]->position.x, elements[i]->position.y, elements[i]->color);
			}
		}

		if (edgeTouched()) {
			static int count = 0;
			std::string filename = pixelbuffer.createFilename("difflimagg", count);
			pixelbuffer.write(filename);
			std::cout << "write " << filename << std::endl;
			count++;
			init();
		}

		// move nonfixed elements and set status
		for (size_t i = 0; i < elements.size(); i++) {
			if (elements[i]->fixed) {
				continue;
			}

			// Look around
			pb::vec2i pos = elements[i]->position;
			for (int y = -1; y < 2; y++) {
				for (int x = -1; x < 2; x++) {
					if (y==0 && x==0) continue;
					pb::vec2i neighbour = pb::vec2i(pos.x + x, pos.y + y);
					pb::RGBAColor color = pixelbuffer.getPixel(neighbour.x, neighbour.y);
					if (color != TRANSPARENT) {
						// we found the tree
						elements[i]->fixed = true;
					}
				}
			}

			if (!elements[i]->fixed) {
				// element is still free moving
				elements[i]->move();
				borders(elements[i], cols, rows);
			}
		}

		// draw moving elements
		for (size_t i = 0; i < elements.size(); i++) {
			if (elements[i]->fixed) {
				continue;
			}
			pixelbuffer.setPixel(elements[i]->position.x, elements[i]->position.y, elements[i]->color);
		}
	}

	void rotateColors()
	{
		for (size_t i = 0; i < elements.size(); i++) {
			if (elements[i]->fixed) {
				continue;
			}
			elements[i]->color = pb::Color::rotate(elements[i]->color, ROT_SPEED);
		}
	}

	void handleInput()
	{
		if (input.getKeyDown(rt::KeyCode::Space)) {
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
