/**
 * @file mazesolver.cpp
 *
 * @brief Maze Solver implementation
 *
 * Copyright 2022 @rktrlng
 * https://github.com/rktrlng/canvas
 */

#include <ctime>
#include <vector>
#include <bitset>

#include <canvas/application.h>

struct MCell {
	int col = 0; // x
	int row = 0; // y
	bool wall = true;
	bool valid = false;
	bool visited = true;
};


class MyApp : public rt::Application
{
private:
	std::vector<MCell*> field;
	std::vector<MCell*> breadcrumbs;
	std::vector<MCell*> path;
	MCell* current = nullptr;
	MCell* start = nullptr;
	MCell* end = nullptr;
	bool backtracking = false;
	size_t cols = 0;
	size_t rows = 0;

public:
	// MyApp(uint16_t cols, uint16_t rows, uint8_t bitdepth, uint8_t factor) : rt::Application(cols, rows, bitdepth, factor)
	// {
	// 	std::srand(std::time(nullptr));
	// 	init();
	// }

	MyApp(pb::PixelBuffer& pixelbuffer, uint8_t factor) : rt::Application(pixelbuffer, factor)
	{
		std::srand(std::time(nullptr));
		init();
	}

	virtual ~MyApp()
	{
		
	}

	void init()
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

		auto& pixelbuffer = layers[0]->pixelbuffer;
		cols = pixelbuffer.header().width;
		rows = pixelbuffer.header().height;

		// new empty field
		for (size_t y = 0; y < rows; y++) {
			for (size_t x = 0; x < cols; x++) {
				MCell* cell = new MCell();
				cell->col = x;
				cell->row = y;
				if (pixelbuffer.getPixel(x, y) == BLACK) { cell->wall = true; cell->visited = false; } // wall
				if (pixelbuffer.getPixel(x, y) == WHITE) { cell->wall = false; cell->visited = false; cell->valid = true;} // empty field
				if (pixelbuffer.getPixel(x, y) == RED)   { cell->wall = false; cell->valid = true; start = cell; } // startpoint
				if (pixelbuffer.getPixel(x, y) == BLUE)  { cell->wall = false; cell->valid = true; cell->visited = false; end = cell; } // endpoint
				field.push_back(cell);
			}
		}
		current = start;
		path.push_back(current);
	}

	void update(float deltatime) override
	{
		handleInput();

		static float frametime = 0.0f;
		float maxtime = 0.005f - deltatime;
		frametime += deltatime;
		if (frametime >= maxtime) {
			static bool s = true;
			if(s) {
				s = step();
				if (!s) {
					drawMaze();
					std::cout << "done" << std::endl;
					auto& pixelbuffer = layers[0]->pixelbuffer;
					std::string name = "maze00000_solved.pbf";
					pixelbuffer.write(name);
					std::cout << name << std::endl;
				}
			}
			drawMaze();
			frametime = 0.0f;
		}
	}

private:
	MCell* getNextValidNeighbour(MCell* mc)
	{
		// keep a list of possible neighbours
		std::vector<MCell*> neighbours;
		size_t x = mc->col;
		size_t y = mc->row;
		size_t index = 0;

		// look right
		index = pb::idFromPos(x+1,y,cols);
		if (!field[index]->wall && field[index]->valid && !field[index]->visited) {
			neighbours.push_back(field[index]);
		}
		// look left
		index = pb::idFromPos(x-1,y,cols);
		if (!field[index]->wall && field[index]->valid && !field[index]->visited) {
			neighbours.push_back(field[index]);
		}
		// look down
		index = pb::idFromPos(x,y+1,cols);
		if (!field[index]->wall && field[index]->valid && !field[index]->visited) {
			neighbours.push_back(field[index]);
		}
		// look up
		index = pb::idFromPos(x,y-1,cols);
		if (!field[index]->wall && field[index]->valid && !field[index]->visited) {
			neighbours.push_back(field[index]);
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
		MCell* next = getNextValidNeighbour(current);
		if (next != nullptr) { // there's still a valid neighbour. We're not stuck
			backtracking = false;
			next->visited = true;

			// STEP 2
			breadcrumbs.push_back(current); // drop a breadcrumb on the stack

			// STEP 3
			// removeWalls(current, next); // break through the wall

			// STEP 4
			current = next;
			path.push_back(current); // still looks good...
		} else { // we're stuck! backtrack our steps...
			backtracking = true;
			if (breadcrumbs.size() > 0) {
				current = breadcrumbs.back(); // make previous our current cell
				current->valid = false;
				breadcrumbs.pop_back(); // remove from the breadcrumbs (eat the breadcrumb)
			}
			if (path.size() > 0) {
				path.pop_back(); // nope, wrong track!
			}
		}

		// We've found the exit!
		if (current->col == end->col && current->row == end->row) {
			return false;
		}

		return true;
	}

	void drawMaze()
	{
		auto& pixelbuffer = layers[0]->pixelbuffer;

		for (size_t y = 0; y < rows; y++) {
			for (size_t x = 0; x < cols; x++) {
				pb::RGBAColor color = BLACK;
				int index = pb::idFromPos(x, y, cols);
				MCell* cell = field[index];
				if (cell->wall) {
					color = BLACK;
				} else {
					color = WHITE;
				}
				if (cell->visited) {
					// color = GRAY;
				}
				pb::vec2i pos = pb::vec2i(x, y);
				pixelbuffer.setPixel(pos.x, pos.y, color);
			}
		}

		for (size_t i = 0; i < path.size(); i++) {
			pixelbuffer[pb::idFromPos(path[i]->col, path[i]->row, cols)] = YELLOW;
		}

		// draw holes for start + end
		pixelbuffer.setPixel(start->col, start->row, RED);
		pixelbuffer.setPixel(end->col, end->row, BLUE);

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
	pb::PixelBuffer pixelbuffer("maze00000.pbf");
	MyApp application(pixelbuffer, 8);

	while (!application.quit())
	{
		application.run();
	}

	return 0;
}
