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


class MyApp : public cnv::Application
{
private:
	std::vector<MCell*> m_field;
	std::vector<MCell*> m_breadcrumbs;
	MCell* m_current = nullptr;
	bool m_backtracking = false;
	size_t m_horbias = 1;
	size_t m_verbias = 1;

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

	void initGenerator()
	{
		// reset
		m_current = nullptr;
		for (size_t i = 0; i < m_field.size(); i++) {
			delete[] m_field[i];
		}
		m_field.clear();
		m_breadcrumbs.clear();
		m_backtracking = false;

		// new empty m_field
		for (size_t y = 0; y < HEIGHT; y++) {
			for (size_t x = 0; x < WIDTH; x++) {
				MCell* cell = new MCell();
				cell->col = x;
				cell->row = y;
				m_field.push_back(cell);
			}
		}
		m_current = m_field[0];
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
		index = rt::index(x+1,y,WIDTH);
		if( index < WIDTH*HEIGHT && index >= 0 && x < WIDTH-1) {
			if (!m_field[index]->visited) {
				for (size_t i = 0; i < hbias; i++) {
					neighbours.push_back(m_field[index]);
				}
			}
		}
		// look left
		index = rt::index(x-1,y,WIDTH);
		if( index < WIDTH*HEIGHT && index >= 0 && x > 0 ) {
			if (!m_field[index]->visited) {
				for (size_t i = 0; i < hbias; i++) {
					neighbours.push_back(m_field[index]);
				}
			}
		}
		// look down
		index = rt::index(x,y+1,WIDTH);
		if( index < WIDTH*HEIGHT && index >= 0 ) {
			if (!m_field[index]->visited) {
				for (size_t i = 0; i < vbias; i++) {
					neighbours.push_back(m_field[index]);
				}
			}
		}
		// look up
		index = rt::index(x,y-1,WIDTH);
		if( index < WIDTH*HEIGHT && index >= 0 ) {
			if (!m_field[index]->visited) {
				for (size_t i = 0; i < vbias; i++) {
					neighbours.push_back(m_field[index]);
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
		// make 'm_current' find the next place to be
		m_current->visited = true;
		// STEP 1: while there is a neighbour...
		MCell* next = getRandomUnvisitedSeperatedNeighbour(m_current, m_horbias, m_verbias);
		if (next != nullptr) { // there's still an unvisited neighbour. We're not stuck
			m_backtracking = false;
			next->visited = true;

			// STEP 2
			m_breadcrumbs.push_back(m_current); // drop a breadcrumb on the stack

			// STEP 3
			removeWalls(m_current, next); // break through the wall

			// STEP 4
			m_current = next;
		} else { // we're stuck! backtrack our steps...
			m_backtracking = true;
			if (m_breadcrumbs.size() > 0) {
				m_current = m_breadcrumbs.back(); // make previous our m_current cell
				m_breadcrumbs.pop_back(); // remove from the m_breadcrumbs (eat the breadcrumb)
			}
		}

		// We're back at the start m_field[0]
		if (m_current->col == 0 && m_current->row == 0) {
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
				rt::RGBAColor color = BLACK;
				int index = rt::index(x/2, y/2, WIDTH);
				MCell* cell = m_field[index];
				if (cell->visited) {
					color = WHITE;
				} else {
					color = GRAY;
				}
				rt::vec2i pos = rt::vec2i(x+1, y+1);
				pixelbuffer.setPixel(pos.x, pos.y, color);

				if (m_current == cell) {
					if(m_backtracking) {
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
	MyApp application(WIDTH*2+1, HEIGHT*2+1, 24, 8);

	while (!application.quit())
	{
		application.run();
	}

	return 0;
}
