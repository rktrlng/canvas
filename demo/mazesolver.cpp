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
	bool visited = false;
	bool wall = true;
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
	std::string filename = "";

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
				pb::RGBAColor color = pixelbuffer.getPixel(x, y);
				// if (color == BLACK) { cell->wall = true; } // wall (default)
				if (color == WHITE || color == YELLOW || color == GRAY) { cell->wall = false; } // empty field
				if (color == RED)   { cell->wall = false; start = cell; } // startpoint
				if (color == BLUE)  { cell->wall = false; end = cell; } // endpoint
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
			static bool found = false;
			if(!found) {
				found = step();
				if (found) {
					drawMaze();
					std::cout << "done" << std::endl;
					auto& pixelbuffer = layers[0]->pixelbuffer;
					// remove .pbf extension if there is one
					size_t lastindex = filename.find_last_of(".");
					if((filename.substr(lastindex + 1) == "pbf")) {
						filename = filename.substr(0, lastindex); 
					}
					filename += "_solved_" + std::to_string(field.size()) + "_" + std::to_string(path.size()) + ".pbf";
					pixelbuffer.write(filename);
					std::cout << filename << std::endl;
				}
			}
			drawMaze();
			frametime = 0.0f;
		}
	}

private:
	MCell* getNextUnvistedNeighbour(MCell* mc)
	{
		// keep a list of possible neighbours
		std::vector<MCell*> neighbours;
		size_t x = mc->col;
		size_t y = mc->row;
		size_t index = 0;

		// look right
		index = pb::idFromPos(x+1,y,cols);
		if (!field[index]->wall && !field[index]->visited) {
			neighbours.push_back(field[index]);
		}
		// look left
		index = pb::idFromPos(x-1,y,cols);
		if (!field[index]->wall && !field[index]->visited) {
			neighbours.push_back(field[index]);
		}
		// look down
		index = pb::idFromPos(x,y+1,cols);
		if (!field[index]->wall && !field[index]->visited) {
			neighbours.push_back(field[index]);
		}
		// look up
		index = pb::idFromPos(x,y-1,cols);
		if (!field[index]->wall && !field[index]->visited) {
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
		// while there is a neighbour...
		MCell* next = getNextUnvistedNeighbour(current);
		if (next != nullptr) { // there's still an unvisited neighbour. We're not stuck
			backtracking = false;
			next->visited = true;

			breadcrumbs.push_back(current); // drop a breadcrumb on the stack

			current = next;
			path.push_back(current); // still looks good...
		} else { // we're stuck! backtrack our steps...
			backtracking = true;
			if (breadcrumbs.size() > 0) {
				current = breadcrumbs.back(); // make previous our current cell
				breadcrumbs.pop_back(); // remove from the breadcrumbs (eat the breadcrumb)
			}
			if (path.size() > 0) {
				path.pop_back(); // nope, wrong track!
			}
		}

		// We've found the exit!
		if (current->col == end->col && current->row == end->row) {
			return true;
		}

		return false;
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


int main(int argc, char *argv[])
{
	std::string filename = "maze00000.pbf";

	if (argc == 1) {
		std::cout << "Usage: ./mazesolver [path]" << std::endl;
	}
	if (argc == 2) {
		filename = argv[1];
	}

	pb::PixelBuffer pixelbuffer(filename);
	MyApp application(pixelbuffer, 8);
	application.filename = filename;

	while (!application.quit())
	{
		application.run();
	}

	return 0;
}
