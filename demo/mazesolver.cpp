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

class MyApp : public cnv::Application
{
private:
	std::vector<PCell*> m_solverfield;
	std::vector<PCell*> m_breadcrumbs;
	std::vector<PCell*> m_solution;
	PCell* m_seeker = nullptr;
	PCell* m_start = nullptr;
	PCell* m_end = nullptr;
	State m_state = State::SEARCHING;
	size_t m_cols = 0;
	size_t m_rows = 0;

public:
	std::string filename = "";

	// MyApp(uint16_t cols, uint16_t rows, uint8_t bitdepth, uint8_t factor) : cnv::Application(cols, rows, bitdepth, factor)
	// {
	// 	std::srand(std::time(nullptr));
	// 	init();
	// }

	MyApp(rt::PixelBuffer& pixelbuffer, uint8_t factor) : cnv::Application(pixelbuffer, factor)
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
		m_seeker = nullptr;
		for (size_t i = 0; i < m_solverfield.size(); i++) {
			delete[] m_solverfield[i];
		}
		m_solverfield.clear();
		m_breadcrumbs.clear();
		m_solution.clear();

		m_state = State::SEARCHING;

		auto& pixelbuffer = layers[0]->pixelbuffer;
		m_cols = pixelbuffer.header().width;
		m_rows = pixelbuffer.header().height;

		// new empty m_solverfield
		for (size_t y = 0; y < m_rows; y++) {
			for (size_t x = 0; x < m_cols; x++) {
				PCell* cell = new PCell();
				cell->col = x;
				cell->row = y;
				rt::RGBAColor color = pixelbuffer.getPixel(x, y);
				// if (color == BLACK) { cell->wall = true; } // wall (default)
				if (color == WHITE || color == ORANGE || color == GRAY) { cell->wall = false; } // empty m_solverfield
				if (color == RED)   { cell->wall = false; m_start = cell; } // m_startpoint
				if (color == BLUE)  { cell->wall = false; m_end = cell; } // endpoint
				m_solverfield.push_back(cell);
			}
		}
		m_seeker = m_start;
		m_solution.push_back(m_seeker);
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
				filename += "_solved_" + std::to_string(m_solverfield.size()) + "_" + std::to_string(m_solution.size()) + ".pbf";
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
		index = rt::index(x+1,y,m_cols);
		if (!m_solverfield[index]->wall && !m_solverfield[index]->visited) {
			neighbours.push_back(m_solverfield[index]);
		}
		// look left
		index = rt::index(x-1,y,m_cols);
		if (!m_solverfield[index]->wall && !m_solverfield[index]->visited) {
			neighbours.push_back(m_solverfield[index]);
		}
		// look down
		index = rt::index(x,y+1,m_cols);
		if (!m_solverfield[index]->wall && !m_solverfield[index]->visited) {
			neighbours.push_back(m_solverfield[index]);
		}
		// look up
		index = rt::index(x,y-1,m_cols);
		if (!m_solverfield[index]->wall && !m_solverfield[index]->visited) {
			neighbours.push_back(m_solverfield[index]);
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
		// make 'm_seeker' find the next place to be
		m_seeker->visited = true;
		// while there is a neighbour...
		PCell* next = getNextUnvistedDirectNeighbour(m_seeker);
		if (next != nullptr) { // there's still an unvisited neighbour. We're not stuck
			m_state = State::SEARCHING;
			next->visited = true;

			m_breadcrumbs.push_back(m_seeker); // drop a breadcrumb on the stack

			m_seeker = next;
			m_solution.push_back(m_seeker); // still looks good...
		} else { // we're stuck! backtrack our steps...
			m_state = State::BACKTRACKING;
			if (m_breadcrumbs.size() > 0) {
				m_seeker = m_breadcrumbs.back(); // make previous our m_seeker cell
				m_breadcrumbs.pop_back(); // remove from the m_breadcrumbs (eat the breadcrumb)
			}
			if (m_solution.size() > 0) {
				m_solution.pop_back(); // nope, wrong track!
			}
		}

		// We've found the exit!
		if (m_seeker->col == m_end->col && m_seeker->row == m_end->row) {
			return true;
		}

		return false;
	}

	void drawMazeSolver()
	{
		auto& pixelbuffer = layers[0]->pixelbuffer;

		for (size_t y = 0; y < m_rows; y++) {
			for (size_t x = 0; x < m_cols; x++) {
				rt::RGBAColor color = BLACK;
				int index = rt::index(x, y, m_cols);
				PCell* cell = m_solverfield[index];
				if (cell->wall) {
					color = BLACK;
				} else {
					color = WHITE;
				}
				if (cell->visited) {
					// color = GRAY;
				}
				rt::vec2i pos = rt::vec2i(x, y);
				pixelbuffer.setPixel(pos.x, pos.y, color);
			}
		}

		// draw m_solution so far
		for (size_t i = 0; i < m_solution.size(); i++) {
			pixelbuffer[rt::index(m_solution[i]->col, m_solution[i]->row, m_cols)] = ORANGE;
		}

		// draw m_start + end
		pixelbuffer.setPixel(m_start->col, m_start->row, RED);
		pixelbuffer.setPixel(m_end->col, m_end->row, BLUE);

		layers[0]->lock();
	}

	void handleInput()
	{
		if (input.getKeyDown(cnv::KeyCode::Space)) {
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
		std::cout << "Usage: ./mazesolver [m_solution]" << std::endl;
	}
	if (argc == 2) {
		filename = argv[1];
	}

	rt::PixelBuffer pixelbuffer(filename);
	MyApp application(pixelbuffer, 8);
	application.filename = filename;

	while (!application.quit())
	{
		application.run();
	}

	return 0;
}
