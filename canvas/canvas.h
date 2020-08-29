#ifndef CANVAS_H
#define CANVAS_H

#include <string>

#include <GL/glew.h>

#include <canvas/pixelbuffer.h>

namespace rt {

class Canvas
{
	public:
		Canvas(uint16_t width, uint16_t height, uint8_t bitdepth = 4);
		Canvas(const std::string& imagepath);
		virtual ~Canvas();

		GLuint texture() { return _texture; };
		GLuint vertexbuffer() { return _vertexbuffer; };
		GLuint uvbuffer() { return _uvbuffer; };

		uint16_t width() { return pixelbuffer.header().width; };
		uint16_t height() { return pixelbuffer.header().height; };

		GLuint generateTexture();
		int generateGeometry(int width, int height);

		rt::PixelBuffer pixelbuffer;
		bool dynamic = true;

	private:
		GLuint _texture;
		GLuint _vertexbuffer;
		GLuint _uvbuffer;
};

} // namespace rt

#endif /* CANVAS_H */
