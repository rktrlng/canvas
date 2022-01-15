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


class MyApp : public rt::Application
{
private:
	State state = State::GENERATING;
	int mazenum = 0;
	size_t cols = 0;
	size_t rows = 0;

	// ##### Generator #####
	std::vector<MCell*> generatorfield;
	std::vector<MCell*> breadcrumbs_generator;
	MCell* gencurrent = nullptr;
	size_t horbias = 1;
	size_t verbias = 1;

	// ##### Solver #####
	std::vector<PCell*> solverfield;
	std::vector<PCell*> breadcrumbs_solver;
	std::vector<PCell*> solution;
	PCell* seeker = nullptr;
	PCell* start = nullptr;
	PCell* end = nullptr;

	std::vector<pb::RGBAColor> palette;

public:
	MyApp(uint16_t width, uint16_t height, uint8_t bitdepth, uint8_t factor) : rt::Application(width, height, bitdepth, factor)
	{
		std::srand(std::time(nullptr));
		layers[0]->pixelbuffer.fill(BLACK);
		initGenerator();
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
		float maxtime = 0.025f - deltatime;
		frametime += deltatime;
		if (frametime >= maxtime) {
			switch (state)
			{
			case State::GENERATING:
				if (generateMaze()) {
					state = State::DONEGENERATING;
				}
				break;
			case State::GENBACKTRACKING:
				if (generateMaze()) {
					state = State::DONEGENERATING;
				}
				break;
			case State::SOLVEBACKTRACKING:
				if (solveMaze()) {
					state = State::DONESEARCHING;
				}
				break;
			case State::SEARCHING:
				if (solveMaze()) {
					state = State::DONESEARCHING;
				}
				break;
			case State::DONEGENERATING:
				static float donetime = 0.0f;
				donetime += frametime;
				if (donetime > 3.0f) {
					donetime = 0.0f;
					initSolver();
					state = State::SEARCHING;
				}
				break;
			case State::DONESEARCHING:
				initGenerator();
				state = State::VICTORY;
				break;
			case State::VICTORY:
				drawMazeSolver(frametime);
				static float victime = 0.0f;
				victime += frametime;
				if (victime > 10.0f) {
					victime = 0.0f;
					state = State::GENERATING;
				}
				break;
			default:
				// state = State::GENERATING;
				break;
			}
			// std::cout << "state: " << (int)state << std::endl;
			
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
				pixelbuffer.setPixel(1, 1, RED); // start
				pixelbuffer.setPixel(WIDTH*2-1, HEIGHT*2-1, BLUE); // end

				if (write_generated) {
					std::string name = pixelbuffer.createFilename("maze", mazenum);
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
		gencurrent = nullptr;

		for (size_t i = 0; i < generatorfield.size(); i++) {
			delete[] generatorfield[i];
		}
		generatorfield.clear();

		breadcrumbs_generator.clear();

		state = State::GENERATING;

		// new empty generatorfield
		for (size_t y = 0; y < HEIGHT; y++) {
			for (size_t x = 0; x < WIDTH; x++) {
				MCell* cell = new MCell();
				cell->col = x;
				cell->row = y;
				generatorfield.push_back(cell);
			}
		}
		gencurrent = generatorfield[0];
	}

	bool generateStep()
	{
		// make 'gencurrent' find the next place to be
		gencurrent->visited = true;
		// STEP 1: while there is a neighbour...
		MCell* next = getRandomUnvisitedSeperatedNeighbour(gencurrent, horbias, verbias);
		if (next != nullptr) { // there's still an unvisited neighbour. We're not stuck
			state = State::GENERATING;
			next->visited = true;

			// STEP 2
			breadcrumbs_generator.push_back(gencurrent); // drop a breadcrumb on the stack

			// STEP 3
			removeWalls(gencurrent, next); // break through the wall

			// STEP 4
			gencurrent = next;
		} else { // we're stuck! backtrack our steps...
			state = State::GENBACKTRACKING;
			if (breadcrumbs_generator.size() > 0) {
				gencurrent = breadcrumbs_generator.back(); // make previous our gencurrent cell
				breadcrumbs_generator.pop_back(); // remove from the breadcrumbs (eat the breadcrumb)
			}
		}

		// We're back at the start generatorfield[0]
		if (gencurrent->col == 0 && gencurrent->row == 0) {
			return true;
		}

		return false;
	}

	void drawMazeGenerator()
	{
		auto& pixelbuffer = layers[0]->pixelbuffer;

		for (size_t y = 0; y < HEIGHT*2; y+=2) {
			for (size_t x = 0; x < WIDTH*2; x+=2) {
				pb::RGBAColor color = BLACK;
				int index = pb::idFromPos(x/2, y/2, WIDTH);
				MCell* cell = generatorfield[index];
				if (cell->visited) {
					color = WHITE;
				} else {
					color = GRAY;
				}
				pb::vec2i pos = pb::vec2i(x+1, y+1);
				pixelbuffer.setPixel(pos.x, pos.y, color);

				if (gencurrent == cell) {
					if(state == State::GENBACKTRACKING) {
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
		index = pb::idFromPos(x+1,y,WIDTH);
		if( index < WIDTH*HEIGHT && index >= 0 && x < WIDTH-1) {
			if (!generatorfield[index]->visited) {
				for (size_t i = 0; i < hbias; i++) {
					neighbours.push_back(generatorfield[index]);
				}
			}
		}
		// look left
		index = pb::idFromPos(x-1,y,WIDTH);
		if( index < WIDTH*HEIGHT && index >= 0 && x > 0 ) {
			if (!generatorfield[index]->visited) {
				for (size_t i = 0; i < hbias; i++) {
					neighbours.push_back(generatorfield[index]);
				}
			}
		}
		// look down
		index = pb::idFromPos(x,y+1,WIDTH);
		if( index < WIDTH*HEIGHT && index >= 0 ) {
			if (!generatorfield[index]->visited) {
				for (size_t i = 0; i < vbias; i++) {
					neighbours.push_back(generatorfield[index]);
				}
			}
		}
		// look up
		index = pb::idFromPos(x,y-1,WIDTH);
		if( index < WIDTH*HEIGHT && index >= 0 ) {
			if (!generatorfield[index]->visited) {
				for (size_t i = 0; i < vbias; i++) {
					neighbours.push_back(generatorfield[index]);
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
		seeker = nullptr;
		start = nullptr;
		end = nullptr;

		for (size_t i = 0; i < solverfield.size(); i++) {
			delete[] solverfield[i];
		}
		solverfield.clear();

		breadcrumbs_solver.clear();
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

		palette.clear();
		pb::RGBAColor color = RED;
		size_t amount = 600;
		for (size_t i = 0; i < amount; i++) {
			palette.push_back(color);
			color = pb::Color::rotate(color, 1.0f / amount);
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
					std::string filename = pixelbuffer.createFilename("maze", mazenum);
					// remove .pbf extension if there is one
					size_t lastindex = filename.find_last_of(".");
					if((filename.substr(lastindex + 1) == "pbf")) {
						filename = filename.substr(0, lastindex); 
					}
					filename += "_solved_" + std::to_string(solverfield.size()) + "_" + std::to_string(solution.size()) + ".pbf";
					pixelbuffer.write(filename);
					std::cout << filename << std::endl;
				}

				mazenum++;
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
		// make 'seeker' find the next place to be
		seeker->visited = true;
		// while there is a neighbour...
		PCell* next = getNextUnvisitedDirectNeighbour(seeker);
		if (next != nullptr) { // there's still an unvisited neighbour. We're not stuck
			state = State::SEARCHING;
			next->visited = true;

			breadcrumbs_solver.push_back(seeker); // drop a breadcrumb on the stack

			seeker = next;
			solution.push_back(seeker); // still looks good...
		} else { // we're stuck! backtrack our steps...
			state = State::SOLVEBACKTRACKING;
			if (breadcrumbs_solver.size() > 0) {
				seeker = breadcrumbs_solver.back(); // make previous our seeker cell
				breadcrumbs_solver.pop_back(); // remove from the breadcrumbs (eat the breadcrumb)
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

	void drawMazeSolver(float deltatime)
	{
		auto& pixelbuffer = layers[0]->pixelbuffer;

		for (size_t y = 0; y < rows; y++) {
			for (size_t x = 0; x < cols; x++) {
				pb::RGBAColor color = BLACK;
				int index = pb::idFromPos(x, y, cols);
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
			// pixelbuffer[pb::idFromPos(solution[i]->col, solution[i]->row, cols)] = ORANGE;
			pb::RGBAColor color = palette[i % palette.size()];
			pixelbuffer[pb::idFromPos(solution[i]->col, solution[i]->row, cols)] = color;
		}

		if (state == State::VICTORY) {
			for (size_t i = 0; i < palette.size(); i++) {
				pb::RGBAColor color = palette[i];
				palette[i] = pb::Color::rotate(color, 1.0f - deltatime);
			}
		}

		// draw start + end
		// pixelbuffer.setPixel(start->col, start->row, RED);
		// pixelbuffer.setPixel(end->col, end->row, BLUE);
		pixelbuffer.setPixel(end->col, end->row, palette[(solution.size()-1)%palette.size()]);

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
		index = pb::idFromPos(x+1,y,cols);
		if (!solverfield[index]->wall && !solverfield[index]->visited) {
			neighbours.push_back(solverfield[index]);
		}
		// look left
		index = pb::idFromPos(x-1,y,cols);
		if (!solverfield[index]->wall && !solverfield[index]->visited) {
			neighbours.push_back(solverfield[index]);
		}
		// look down
		index = pb::idFromPos(x,y+1,cols);
		if (!solverfield[index]->wall && !solverfield[index]->visited) {
			neighbours.push_back(solverfield[index]);
		}
		// look up
		index = pb::idFromPos(x,y-1,cols);
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
	MyApp application(WIDTH*2+1, HEIGHT*2+1, 24, 16);

	while (!application.quit())
	{
		application.run();
	}

	return 0;
}
