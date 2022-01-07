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
const int HEIGHT = 24;


class MyApp : public rt::Application
{
private:
	std::vector<MCell*> field;
	std::vector<MCell*> stack;
	MCell* current = nullptr;
	bool backtracking = false;

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
		current = field[0];
	}

	void update(float deltatime) override
	{
		handleInput();

		static float frametime = 0.0f;
		float maxtime = 0.025f - deltatime;
		frametime += deltatime;
		if (frametime >= maxtime) {
			static bool s = true;
			if(s) {
				s = step();
				if (!s) {
					std::cout << "Done! Thank you." << std::endl;
					drawMaze();
					layers[0]->pixelbuffer.setPixel(1, 0, WHITE);
					layers[0]->pixelbuffer.setPixel(1, 1, WHITE);
					layers[0]->pixelbuffer.setPixel(WIDTH*2, HEIGHT*2-1, WHITE);
					layers[0]->pixelbuffer.write("maze.pbf");
				}
			}
			drawMaze();
			frametime = 0.0f;
		}
	}

private:
	MCell* getRandomUnvisitedNeighbour(MCell* mc)
	{
		// keep a list of possible neighbours
		std::vector<MCell*> neighbours;
		int x = mc->col;
		int y = mc->row;
		int index = 0;

		// look right
		index = pb::idFromPos(x+1,y,WIDTH);
		if( index < WIDTH*HEIGHT && index >= 0 && x < WIDTH-1) {
			if (!field[index]->visited) {
				neighbours.push_back(field[index]);
			}
		}
		// look left
		index = pb::idFromPos(x-1,y,WIDTH);
		if( index < WIDTH*HEIGHT && index >= 0 && x > 0 ) {
			if (!field[index]->visited) {
				neighbours.push_back(field[index]);
			}
		}
		// look down
		index = pb::idFromPos(x,y+1,WIDTH);
		if( index < WIDTH*HEIGHT && index >= 0 ) {
			if (!field[index]->visited) {
				neighbours.push_back(field[index]);
			}
		}
		// look up
		index = pb::idFromPos(x,y-1,WIDTH);
		if( index < WIDTH*HEIGHT && index >= 0 ) {
			if (!field[index]->visited) {
				neighbours.push_back(field[index]);
			}
		}

		// there's a valid neighbour!
		if (neighbours.size() > 0) {
			// pick one from the list
			int r = rand()%neighbours.size();
			return neighbours[r];
		}

		// no neighbours
		return nullptr;
	}

	bool step()
	{
		// make 'current' find the next place to be
		current->visited = true;
		// STEP 1: while there is a neighbour...
		MCell* next = this->getRandomUnvisitedNeighbour(current);
		if (next != nullptr) { // there's still an unvisited neighbour. We're not stuck
			backtracking = false;
			next->visited = true;

			// STEP 2
			stack.push_back(current); // drop a breadcrumb on the stack

			// STEP 3
			this->removeWalls(current, next); // break through the wall

			// STEP 4
			current = next;
		} else { // we're stuck! backtrack our steps...
			backtracking = true;
			if (stack.size() > 0) {
				current = stack.back(); // make previous our current cell
				stack.pop_back(); // remove from the stack (eat the breadcrumb)
			}
		}

		// We're back at the start field[0]
		if (current->col == 0 && current->row == 0) {
			return false;
		}

		return true;
	}

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
				pb::vec2i pos = pb::vec2i(x+1, y+1);
				pixelbuffer.setPixel(pos.x, pos.y, color);

				if (current == cell) {
					if(backtracking) {
						pixelbuffer.setPixel(pos.x, pos.y, RED);
					} else {
						pixelbuffer.setPixel(pos.x, pos.y, BLUE);
					}
				}

				// draw walls
				auto walls = cell->walls;
				if (walls[0]) { pixelbuffer.setPixel(pos.x, pos.y-1, BLACK); } else { pixelbuffer.setPixel(pos.x, pos.y-1, WHITE); }
				if (walls[1]) { pixelbuffer.setPixel(pos.x+1, pos.y, BLACK); } else { pixelbuffer.setPixel(pos.x+1, pos.y, WHITE); }
				// if (walls[2]) { pixelbuffer.setPixel(pos.x, pos.y+1, BLACK); } else { pixelbuffer.setPixel(pos.x, pos.y+1, WHITE); }
				// if (walls[3]) { pixelbuffer.setPixel(pos.x-1, pos.y, BLACK); } else { pixelbuffer.setPixel(pos.x-1, pos.y, WHITE); }
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
	MyApp application(WIDTH*2+1, HEIGHT*2+1, 8, 8);

	while (!application.quit())
	{
		application.run();
	}

	return 0;
}
