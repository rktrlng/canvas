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

const int WIDTH  = 40;
const int HEIGHT = 22;

const bool write_generated = false;
const bool write_solved = false;

enum class State { GENERATING, SEARCHING, GENBACKTRACKING, SOLVEBACKTRACKING, DONEGENERATING, DONESEARCHING, VICTORY };

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

struct PCell {
	int col = 0; // x
	int row = 0; // y
	bool visited = false;
	bool wall = true;
};


class MyApp : public cnv::Application
{
private:
	State m_state = State::GENERATING;
	int m_mazenum = 0;
	size_t m_cols = 0;
	size_t m_rows = 0;

	// ##### Generator #####
	std::vector<MCell*> m_generatorfield;
	std::vector<MCell*> m_breadcrumbs_generator;
	MCell* m_gencurrent = nullptr;
	size_t m_horbias = 1;
	size_t m_verbias = 1;

	// ##### Solver #####
	std::vector<PCell*> m_solverfield;
	std::vector<PCell*> m_breadcrumbs_solver;
	std::vector<PCell*> m_solution;
	PCell* m_seeker = nullptr;
	PCell* m_start = nullptr;
	PCell* m_end = nullptr;

	std::vector<rt::RGBAColor> m_palette;

public:
	MyApp(uint16_t width, uint16_t height, uint8_t bitdepth, uint8_t factor) : cnv::Application(width, height, bitdepth, factor)
	{
		std::srand(std::time(nullptr));
		layers[0]->pixelbuffer.fill(BLACK);
		initGenerator();
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
		float maxtime = 0.025f - deltatime;
		frametime += deltatime;
		if (frametime >= maxtime) {
			static float donetime = 0.0f;
			static float victime = 0.0f;

			switch (m_state)
			{
			case State::GENERATING:
				if (generateMaze()) {
					m_state = State::DONEGENERATING;
				}
				break;
			case State::GENBACKTRACKING:
				if (generateMaze()) {
					m_state = State::DONEGENERATING;
				}
				break;
			case State::SOLVEBACKTRACKING:
				if (solveMaze()) {
					m_state = State::DONESEARCHING;
				}
				break;
			case State::SEARCHING:
				if (solveMaze()) {
					m_state = State::DONESEARCHING;
				}
				break;
			case State::DONEGENERATING:
				donetime += frametime;
				if (donetime > 3.0f) {
					donetime = 0.0f;
					initSolver();
					m_state = State::SEARCHING;
				}
				break;
			case State::DONESEARCHING:
				initGenerator();
				m_state = State::VICTORY;
				break;
			case State::VICTORY:
				drawMazeSolver(frametime);
				victime += frametime;
				if (victime > 10.0f) {
					victime = 0.0f;
					m_state = State::GENERATING;
				}
				break;
			default:
				// m_state = State::GENERATING;
				break;
			}
			// std::cout << "m_state: " << (int)m_state << std::m_endl;
			
			frametime = 0.0f;
		}
	}

private:
	// #########################################
	// # Generator
	// #########################################
	bool generateMaze()
	{
		static bool done = false;
		if(!done) {
			done = generateStep();
			if (done) {
				drawMazeGenerator();
				auto& pixelbuffer = layers[0]->pixelbuffer;
				pixelbuffer.setPixel(1, 1, RED); // m_start
				pixelbuffer.setPixel(WIDTH*2-1, HEIGHT*2-1, BLUE); // m_end

				if (write_generated) {
					std::string name = pixelbuffer.createFilename("maze", m_mazenum);
					pixelbuffer.write(name);
					std::cout << name << std::endl;
				}

				done = false;
				return true;
			}
			done = false;
		}
		drawMazeGenerator();

		return done;
	}

	void initGenerator()
	{
		// reset
		m_gencurrent = nullptr;

		for (size_t i = 0; i < m_generatorfield.size(); i++) {
			delete[] m_generatorfield[i];
		}
		m_generatorfield.clear();

		m_breadcrumbs_generator.clear();

		m_state = State::GENERATING;

		// new empty m_generatorfield
		for (size_t y = 0; y < HEIGHT; y++) {
			for (size_t x = 0; x < WIDTH; x++) {
				MCell* cell = new MCell();
				cell->col = x;
				cell->row = y;
				m_generatorfield.push_back(cell);
			}
		}
		m_gencurrent = m_generatorfield[0];
	}

	bool generateStep()
	{
		// make 'm_gencurrent' find the next place to be
		m_gencurrent->visited = true;
		// STEP 1: while there is a neighbour...
		MCell* next = getRandomUnvisitedSeperatedNeighbour(m_gencurrent, m_horbias, m_verbias);
		if (next != nullptr) { // there's still an unvisited neighbour. We're not stuck
			m_state = State::GENERATING;
			next->visited = true;

			// STEP 2
			m_breadcrumbs_generator.push_back(m_gencurrent); // drop a breadcrumb on the stack

			// STEP 3
			removeWalls(m_gencurrent, next); // break through the wall

			// STEP 4
			m_gencurrent = next;
		} else { // we're stuck! backtrack our steps...
			m_state = State::GENBACKTRACKING;
			if (m_breadcrumbs_generator.size() > 0) {
				m_gencurrent = m_breadcrumbs_generator.back(); // make previous our m_gencurrent cell
				m_breadcrumbs_generator.pop_back(); // remove from the breadcrumbs (eat the breadcrumb)
			}
		}

		// We're back at the m_start m_generatorfield[0]
		if (m_gencurrent->col == 0 && m_gencurrent->row == 0) {
			return true;
		}

		return false;
	}

	void drawMazeGenerator()
	{
		auto& pixelbuffer = layers[0]->pixelbuffer;

		for (size_t y = 0; y < HEIGHT*2; y+=2) {
			for (size_t x = 0; x < WIDTH*2; x+=2) {
				rt::RGBAColor color = BLACK;
				int index = rt::index(x/2, y/2, WIDTH);
				MCell* cell = m_generatorfield[index];
				if (cell->visited) {
					color = WHITE;
				} else {
					color = GRAY;
				}
				rt::vec2i pos = rt::vec2i(x+1, y+1);
				pixelbuffer.setPixel(pos.x, pos.y, color);

				if (m_gencurrent == cell) {
					if(m_state == State::GENBACKTRACKING) {
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

	MCell* getRandomUnvisitedSeperatedNeighbour(MCell* mc, size_t hbias = 1, size_t vbias = 1)
	{
		// keep a list of possible neighbours
		std::vector<MCell*> neighbours;
		int x = mc->col;
		int y = mc->row;
		int index = 0;

		// look right
		index = rt::index(x+1,y,WIDTH);
		if( index < WIDTH*HEIGHT && index >= 0 && x < WIDTH-1) {
			if (!m_generatorfield[index]->visited) {
				for (size_t i = 0; i < hbias; i++) {
					neighbours.push_back(m_generatorfield[index]);
				}
			}
		}
		// look left
		index = rt::index(x-1,y,WIDTH);
		if( index < WIDTH*HEIGHT && index >= 0 && x > 0 ) {
			if (!m_generatorfield[index]->visited) {
				for (size_t i = 0; i < hbias; i++) {
					neighbours.push_back(m_generatorfield[index]);
				}
			}
		}
		// look down
		index = rt::index(x,y+1,WIDTH);
		if( index < WIDTH*HEIGHT && index >= 0 ) {
			if (!m_generatorfield[index]->visited) {
				for (size_t i = 0; i < vbias; i++) {
					neighbours.push_back(m_generatorfield[index]);
				}
			}
		}
		// look up
		index = rt::index(x,y-1,WIDTH);
		if( index < WIDTH*HEIGHT && index >= 0 ) {
			if (!m_generatorfield[index]->visited) {
				for (size_t i = 0; i < vbias; i++) {
					neighbours.push_back(m_generatorfield[index]);
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

	// #########################################
	// # Solver
	// #########################################
	void initSolver()
	{
		// reset
		m_seeker = nullptr;
		m_start = nullptr;
		m_end = nullptr;

		for (size_t i = 0; i < m_solverfield.size(); i++) {
			delete[] m_solverfield[i];
		}
		m_solverfield.clear();

		m_breadcrumbs_solver.clear();
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
				if (color == BLUE)  { cell->wall = false; m_end = cell; } // m_endpoint
				m_solverfield.push_back(cell);
			}
		}
		m_seeker = m_start;
		m_solution.push_back(m_seeker);

		m_palette.clear();
		rt::RGBAColor color = RED;
		size_t amount = 600;
		for (size_t i = 0; i < amount; i++) {
			m_palette.push_back(color);
			color = rt::rotate(color, 1.0f / amount);
		}
	}

	bool solveMaze()
	{
		static bool found = false;
		if(!found) {
			found = solveStep();
			if (found) {
				drawMazeSolver(0.0f);

				if (write_solved) {
					auto& pixelbuffer = layers[0]->pixelbuffer;
					std::string filename = pixelbuffer.createFilename("maze", m_mazenum);
					// remove .pbf extension if there is one
					size_t lastindex = filename.find_last_of(".");
					if((filename.substr(lastindex + 1) == "pbf")) {
						filename = filename.substr(0, lastindex); 
					}
					filename += "_solved_" + std::to_string(m_solverfield.size()) + "_" + std::to_string(m_solution.size()) + ".pbf";
					pixelbuffer.write(filename);
					std::cout << filename << std::endl;
				}

				m_mazenum++;
				found = false;
				return true;
			}
			found = false;
		}
		drawMazeSolver(0.0f);

		return found;
	}

	bool solveStep()
	{
		// make 'm_seeker' find the next place to be
		m_seeker->visited = true;
		// while there is a neighbour...
		PCell* next = getNextUnvisitedDirectNeighbour(m_seeker);
		if (next != nullptr) { // there's still an unvisited neighbour. We're not stuck
			m_state = State::SEARCHING;
			next->visited = true;

			m_breadcrumbs_solver.push_back(m_seeker); // drop a breadcrumb on the stack

			m_seeker = next;
			m_solution.push_back(m_seeker); // still looks good...
		} else { // we're stuck! backtrack our steps...
			m_state = State::SOLVEBACKTRACKING;
			if (m_breadcrumbs_solver.size() > 0) {
				m_seeker = m_breadcrumbs_solver.back(); // make previous our m_seeker cell
				m_breadcrumbs_solver.pop_back(); // remove from the breadcrumbs (eat the breadcrumb)
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

	void drawMazeSolver(float deltatime)
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
			// pixelbuffer[rt::index(m_solution[i]->col, m_solution[i]->row, m_cols)] = ORANGE;
			rt::RGBAColor color = m_palette[i % m_palette.size()];
			pixelbuffer[rt::index(m_solution[i]->col, m_solution[i]->row, m_cols)] = color;
		}

		if (m_state == State::VICTORY) {
			for (size_t i = 0; i < m_palette.size(); i++) {
				rt::RGBAColor color = m_palette[i];
				m_palette[i] = rt::rotate(color, 1.0f - deltatime);
			}
		}

		// draw m_start + m_end
		// pixelbuffer.setPixel(m_start->col, m_start->row, RED);
		// pixelbuffer.setPixel(m_end->col, m_end->row, BLUE);
		pixelbuffer.setPixel(m_end->col, m_end->row, m_palette[(m_solution.size()-1)%m_palette.size()]);

		layers[0]->lock();
	}

	PCell* getNextUnvisitedDirectNeighbour(PCell* mc)
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


int main( void )
{
	MyApp application(WIDTH*2+1, HEIGHT*2+1, 24, 16);
	application.hideMouse();

	while (!application.quit())
	{
		application.run();
	}

	return 0;
}
