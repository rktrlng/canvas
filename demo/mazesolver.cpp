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

struct PCell {
	int col = 0; // x
	int row = 0; // y
	bool visited = false;
	bool wall = true;
};

enum class State { SEARCHING, BACKTRACKING };

class MyApp : public rt::Application
{
private:
	std::vector<PCell*> solverfield;
	std::vector<PCell*> breadcrumbs;
	std::vector<PCell*> solution;
	PCell* seeker = nullptr;
	PCell* start = nullptr;
	PCell* end = nullptr;
	State state = State::SEARCHING;
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
		initSolver();
	}

	virtual ~MyApp()
	{
		
	}

	void initSolver()
	{
		// reset
		seeker = nullptr;
		for (size_t i = 0; i < solverfield.size(); i++) {
			delete[] solverfield[i];
		}
		solverfield.clear();
		breadcrumbs.clear();
		solution.clear();

		state = State::SEARCHING;

		auto& pixelbuffer = layers[0]->pixelbuffer;
		cols = pixelbuffer.header().width;
		rows = pixelbuffer.header().height;

		// new empty solverfield
		for (size_t y = 0; y < rows; y++) {
			for (size_t x = 0; x < cols; x++) {
				PCell* cell = new PCell();
				cell->col = x;
				cell->row = y;
				pb::RGBAColor color = pixelbuffer.getPixel(x, y);
				// if (color == BLACK) { cell->wall = true; } // wall (default)
				if (color == WHITE || color == ORANGE || color == GRAY) { cell->wall = false; } // empty solverfield
				if (color == RED)   { cell->wall = false; start = cell; } // startpoint
				if (color == BLUE)  { cell->wall = false; end = cell; } // endpoint
				solverfield.push_back(cell);
			}
		}
		seeker = start;
		solution.push_back(seeker);
	}

	void update(float deltatime) override
	{
		handleInput();

		static float frametime = 0.0f;
		float maxtime = 0.01f - deltatime;
		frametime += deltatime;
		if (frametime >= maxtime) {
			// bool solved = solveMaze();
			solveMaze();
			
			frametime = 0.0f;
		}
	}

	bool solveMaze()
	{
		static bool found = false;
		if(!found) {
			found = solveStep();
			if (found) {
				drawMazeSolver();
				std::cout << "done" << std::endl;
				auto& pixelbuffer = layers[0]->pixelbuffer;
				// remove .pbf extension if there is one
				size_t lastindex = filename.find_last_of(".");
				if((filename.substr(lastindex + 1) == "pbf")) {
					filename = filename.substr(0, lastindex); 
				}
				filename += "_solved_" + std::to_string(solverfield.size()) + "_" + std::to_string(solution.size()) + ".pbf";
				pixelbuffer.write(filename);
				std::cout << filename << std::endl;
			}
		}
		drawMazeSolver();

		return found;
	}

private:
	PCell* getNextUnvistedDirectNeighbour(PCell* mc)
	{
		// keep a list of possible neighbours
		std::vector<PCell*> neighbours;
		size_t x = mc->col;
		size_t y = mc->row;
		size_t index = 0;

		// look right
		index = pb::index(x+1,y,cols);
		if (!solverfield[index]->wall && !solverfield[index]->visited) {
			neighbours.push_back(solverfield[index]);
		}
		// look left
		index = pb::index(x-1,y,cols);
		if (!solverfield[index]->wall && !solverfield[index]->visited) {
			neighbours.push_back(solverfield[index]);
		}
		// look down
		index = pb::index(x,y+1,cols);
		if (!solverfield[index]->wall && !solverfield[index]->visited) {
			neighbours.push_back(solverfield[index]);
		}
		// look up
		index = pb::index(x,y-1,cols);
		if (!solverfield[index]->wall && !solverfield[index]->visited) {
			neighbours.push_back(solverfield[index]);
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

	bool solveStep()
	{
		// make 'seeker' find the next place to be
		seeker->visited = true;
		// while there is a neighbour...
		PCell* next = getNextUnvistedDirectNeighbour(seeker);
		if (next != nullptr) { // there's still an unvisited neighbour. We're not stuck
			state = State::SEARCHING;
			next->visited = true;

			breadcrumbs.push_back(seeker); // drop a breadcrumb on the stack

			seeker = next;
			solution.push_back(seeker); // still looks good...
		} else { // we're stuck! backtrack our steps...
			state = State::BACKTRACKING;
			if (breadcrumbs.size() > 0) {
				seeker = breadcrumbs.back(); // make previous our seeker cell
				breadcrumbs.pop_back(); // remove from the breadcrumbs (eat the breadcrumb)
			}
			if (solution.size() > 0) {
				solution.pop_back(); // nope, wrong track!
			}
		}

		// We've found the exit!
		if (seeker->col == end->col && seeker->row == end->row) {
			return true;
		}

		return false;
	}

	void drawMazeSolver()
	{
		auto& pixelbuffer = layers[0]->pixelbuffer;

		for (size_t y = 0; y < rows; y++) {
			for (size_t x = 0; x < cols; x++) {
				pb::RGBAColor color = BLACK;
				int index = pb::index(x, y, cols);
				PCell* cell = solverfield[index];
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

		// draw solution so far
		for (size_t i = 0; i < solution.size(); i++) {
			pixelbuffer[pb::index(solution[i]->col, solution[i]->row, cols)] = ORANGE;
		}

		// draw start + end
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
		std::cout << "Usage: ./mazesolver [solution]" << std::endl;
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
