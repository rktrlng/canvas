#ifndef APPLICATION_H
#define APPLICATION_H

#include <vector>

#include <canvas/renderer.h>
#include <canvas/input.h>
#include <canvas/canvas.h>
#include <canvas/pixelbuffer.h>
#include <canvas/noise.h>

namespace rt {

class Application
{
public:
	Application(uint16_t width, uint16_t height, uint8_t bitdepth = 24, uint8_t factor = 1);
	Application(PixelBuffer& pixelbuffer, uint8_t factor = 1);
	virtual ~Application();

	int quit();
	int run();
	virtual void update(float deltatime) = 0;

private:
	uint8_t factor = 1;
	Renderer renderer;

protected:
	Input input;
	std::vector<Canvas*> layers;
};

#endif // APPLICATION_H

} // namespace rt
