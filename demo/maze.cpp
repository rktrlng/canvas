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
#include <bitset>

#include <canvas/application.h>

struct MCell {
	int col = 0; // x
	int row = 0; // y
	// 0001 = top
	// 0010 = right
	// 0100 = bottom
	// 1000 = left
	// 0000 = no walls
	// 1111 = all walls
	std::bitset<4> walls = 15;
	bool visited = false;
};


const int WIDTH  = 32;
const int HEIGHT = 32;

std::vector<MCell*> field;

class MyApp : public rt::Application
{
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

	void init()
	{
		for (size_t y = 0; y < HEIGHT; y++) {
			for (size_t x = 0; x < WIDTH; x++) {
				MCell* cell = new MCell();
				cell->col = x;
				cell->row = y;
				field.push_back(cell);
			}
		}

		// test
		int index = 5;
		field[index]->visited = true;
		field[index+HEIGHT]->visited = true;
		removeWalls(field[index], field[index+HEIGHT]);
		field[index+1]->visited = true;
		removeWalls(field[index], field[index+1]);
		field[index-1]->visited = true;
		removeWalls(field[index], field[index-1]);
	}

	void update(float deltatime) override
	{
		handleInput();

		static float frametime = 0.0f;
		float maxtime = 0.05f - deltatime;
		frametime += deltatime;
		if (frametime >= maxtime) {
			drawMaze();
			frametime = 0.0f;
		}
	}

private:
	void removeWalls(MCell* c, MCell* n)
	{
		int dx = c->col - n->col;
		if (dx == 1) {
			c->walls[3] = 0; // left
			n->walls[1] = 0; // right
		} else if (dx == -1) {
			c->walls[1] = 0;
			n->walls[3] = 0;
		}

		int dy = c->row - n->row;
		if (dy == 1) {
			c->walls[0] = 0; // top
			n->walls[2] = 0; // bottom
		} else if (dy == -1) {
			c->walls[2] = 0;
			n->walls[0] = 0;
		}
	}

	void drawMaze()
	{
		auto& pixelbuffer = layers[0]->pixelbuffer;

		for (size_t y = 0; y < HEIGHT*2; y+=2) {
			for (size_t x = 0; x < WIDTH*2; x+=2) {
				pb::RGBAColor color = BLACK;
				int index = pb::idFromPos(x/2, y/2, WIDTH);
				MCell* cell = field[index];
				if (cell->visited) {
					color = WHITE;
				} else {
					color = GRAY;
				}
				pixelbuffer.setPixel(x+1, y+1, color);

				// draw walls
				auto walls = cell->walls;
				if (walls[0]) { pixelbuffer.setPixel(x+1, y+1-1, BLACK); } else { pixelbuffer.setPixel(x+1, y+1-1, WHITE); }
				if (walls[1]) { pixelbuffer.setPixel(x+1+1, y+1, BLACK); } else { pixelbuffer.setPixel(x+1+1, y+1, WHITE); }
				// if (walls[2]) { pixelbuffer.setPixel(x+1, y+1+1, BLACK); } else { pixelbuffer.setPixel(x+1, y+1+1, WHITE); }
				if (walls[3]) { pixelbuffer.setPixel(x+1-1, y+1, BLACK); } else { pixelbuffer.setPixel(x-1+1, y+1, WHITE); }
			}
		}
		layers[0]->lock();
	}

	void handleInput()
	{
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
	MyApp application(WIDTH*2+1, HEIGHT*2+1, 24, 8);

	while (!application.quit())
	{
		application.run();
	}

	return 0;
}
