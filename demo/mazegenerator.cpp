/**
 * @file mazegenerator.cpp
 *
 * @brief Maze Generator implementation
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
	std::vector<MCell*> breadcrumbs;
	MCell* current = nullptr;
	bool backtracking = false;
	size_t horbias = 1;
	size_t verbias = 1;

public:
	MyApp(uint16_t width, uint16_t height, uint8_t bitdepth, uint8_t factor) : rt::Application(width, height, bitdepth, factor)
	{
		std::srand(std::time(nullptr));
		initGenerator();
	}

	// MyApp(rt::PixelBuffer& pixelbuffer, uint8_t factor) : rt::Application(pixelbuffer, factor)
	// {
	// 
	// }

	virtual ~MyApp()
	{
		
	}

	void initGenerator()
	{
		// reset
		current = nullptr;
		for (size_t i = 0; i < field.size(); i++) {
			delete[] field[i];
		}
		field.clear();
		for (size_t i = 0; i < breadcrumbs.size(); i++) {
			delete[] breadcrumbs[i];
		}
		breadcrumbs.clear();
		backtracking = false;

		// new empty field
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

		static int mazecounter = 0;
		static float frametime = 0.0f;
		float maxtime = 0.005f - deltatime;
		frametime += deltatime;
		if (frametime >= maxtime) {
			static bool s = true;
			if(s) {
				s = generateStep();
				if (!s) {
					drawMazeGenerator();
					auto& pixelbuffer = layers[0]->pixelbuffer;
					pixelbuffer.setPixel(1, 1, RED); // start
					pixelbuffer.setPixel(WIDTH*2-1, HEIGHT*2-1, BLUE); // end
					std::string name = pixelbuffer.createFilename("maze", mazecounter);
					pixelbuffer.write(name);
					std::cout << name << std::endl;
					mazecounter++;
					initGenerator();
				}
				s = true;
			}
			drawMazeGenerator();
			frametime = 0.0f;
		}
	}

private:
	MCell* getRandomUnvisitedSeperatedNeighbour(MCell* mc, size_t hbias = 1, size_t vbias = 1)
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
				for (size_t i = 0; i < hbias; i++) {
					neighbours.push_back(field[index]);
				}
			}
		}
		// look left
		index = pb::idFromPos(x-1,y,WIDTH);
		if( index < WIDTH*HEIGHT && index >= 0 && x > 0 ) {
			if (!field[index]->visited) {
				for (size_t i = 0; i < hbias; i++) {
					neighbours.push_back(field[index]);
				}
			}
		}
		// look down
		index = pb::idFromPos(x,y+1,WIDTH);
		if( index < WIDTH*HEIGHT && index >= 0 ) {
			if (!field[index]->visited) {
				for (size_t i = 0; i < vbias; i++) {
					neighbours.push_back(field[index]);
				}
			}
		}
		// look up
		index = pb::idFromPos(x,y-1,WIDTH);
		if( index < WIDTH*HEIGHT && index >= 0 ) {
			if (!field[index]->visited) {
				for (size_t i = 0; i < vbias; i++) {
					neighbours.push_back(field[index]);
				}
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

	bool generateStep()
	{
		// make 'current' find the next place to be
		current->visited = true;
		// STEP 1: while there is a neighbour...
		MCell* next = getRandomUnvisitedSeperatedNeighbour(current, horbias, verbias);
		if (next != nullptr) { // there's still an unvisited neighbour. We're not stuck
			backtracking = false;
			next->visited = true;

			// STEP 2
			breadcrumbs.push_back(current); // drop a breadcrumb on the stack

			// STEP 3
			removeWalls(current, next); // break through the wall

			// STEP 4
			current = next;
		} else { // we're stuck! backtrack our steps...
			backtracking = true;
			if (breadcrumbs.size() > 0) {
				current = breadcrumbs.back(); // make previous our current cell
				breadcrumbs.pop_back(); // remove from the breadcrumbs (eat the breadcrumb)
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

	void drawMazeGenerator()
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
	MyApp application(WIDTH*2+1, HEIGHT*2+1, 24, 8);

	while (!application.quit())
	{
		application.run();
	}

	return 0;
}
