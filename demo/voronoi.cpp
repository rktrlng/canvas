#include <ctime>

#include <canvas/application.h>

struct Agent
{
	rt::vec2i position;
	int fieldx;
	int fieldy;
	rt::RGBAColor color;

	Agent(int width, int height)
	{
		fieldx = width;
		fieldy = height;
		position.x = rand()%width;
		position.y = rand()%height;
		color.r = rand()%255;
		color.g = rand()%255;
		color.b = rand()%255;
	}

	void move()
	{
		rt::vec2i delta((rand()%3)-1, (rand()%3)-1);
		position += delta;
		// position = rt::wrap(position, fieldx, fieldy);
	}
};

class MyApp : public rt::Application
{
private:
	std::vector<Agent*> agents;

public:
	MyApp(uint16_t width, uint16_t height, uint8_t bitdepth, uint8_t factor) : rt::Application(width, height, bitdepth, factor)
	{
		std::srand(std::time(nullptr));

		// layers.push_back( new rt::Canvas(width, height) );
		for (size_t i = 0; i < 25; i++)
		{
			agents.push_back(new Agent(width, height));
		}
	}

	// MyApp(rt::PixelBuffer& pixelbuffer, uint8_t factor) : rt::Application(pixelbuffer, factor)
	// {
	// 	uint16_t cols = pixelbuffer.header().width;
	// 	uint16_t rows = pixelbuffer.header().height;
	// 	layers.push_back( new rt::Canvas(cols, rows, pixelbuffer.header().bitdepth) );
	// 	layers[0]->pixelbuffer = pixelbuffer;
	// }

	virtual ~MyApp()
	{
		for (auto canvas : layers) {
			delete canvas;
		}
		layers.clear();
	}

	void update(float deltatime) override
	{
		handleInput();
		
		static float frametime = 0.0f;
		float maxtime = 0.05f;
		frametime += deltatime;
		if (frametime >= maxtime)
		{
			layers[0]->unlock();
			handleAgents();
			voronoi();
			layers[0]->pixelbuffer.blur();
			layers[0]->lock();

			frametime = 0.0f;
		}
	}

private:
	void handleAgents()
	{
		for (size_t i = 0; i < agents.size(); i++)
		{
			agents[i]->move();
		}
	}

	void voronoi()
	{
		auto& pixelbuffer = layers[0]->pixelbuffer;

		size_t rows = pixelbuffer.header().height;
		size_t cols = pixelbuffer.header().width;
		for (size_t y = 0; y < rows; y++) {
			for (size_t x = 0; x < cols; x++) {
				int max_distance = rt::vec2i(rows, cols).mag();
				int min_distance = max_distance;
				Agent* agent = agents[0];
				for (size_t i = 0; i < agents.size(); i++)
				{
					rt::vec2i delta = rt::vec2i(x,y) - agents[i]->position;
					int current_distance = delta.mag();
					if (current_distance < min_distance)
					{
						min_distance = current_distance;
						agent = agents[i];
					}
				}
				
				// map min_distance to color
				rt::RGBAColor color = agent->color;
				// int value = rt::map(min_distance, 0, max_distance, 0, 255);
				// color.r = value;
				// color.g = value;
				// color.b = value;

				pixelbuffer.setPixel(x, y, color);
			}
		}
	}

	void darken(int amount)
	{
		auto& pixelbuffer = layers[0]->pixelbuffer;

		size_t rows = pixelbuffer.header().height;
		size_t cols = pixelbuffer.header().width;
		for (size_t y = 0; y < rows; y++) {
			for (size_t x = 0; x < cols; x++) {
				rt::RGBAColor current = pixelbuffer.getPixel(x, y);
				if (current.r > amount) {current.r -= amount; }
				if (current.g > amount) {current.g -= amount; }
				if (current.b > amount) {current.b -= amount; }
				pixelbuffer.setPixel(x, y, current);
			}
		}
	}

	void handleInput() {
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
	MyApp application(320, 180, 24, 4);

	while (!application.quit())
	{
		application.run();
	}

	return 0;
}
