/**
 * @file canvas.h
 * @brief cnv::Canvas header
 * @see https://github.com/rktrlng/pixelbuffer
 */

#ifndef CANVAS_H
#define CANVAS_H

#include <string>

#include <GL/glew.h>

#include <pixelbuffer/pixelbuffer.h>

namespace cnv {

class Canvas
{
	public:
		Canvas(uint16_t width, uint16_t height, uint8_t bitdepth = 32, uint8_t scale = 1);
		Canvas(const rt::PixelBuffer& pb);
		Canvas(const std::string& imagepath);
		virtual ~Canvas();

		GLuint texture() { return _texture; };
		GLuint vertexbuffer() { return _vertexbuffer; };
		GLuint uvbuffer() { return _uvbuffer; };

		uint16_t width() { return pixelbuffer.width(); };
		uint16_t height() { return pixelbuffer.height(); };

		GLuint generateTexture();
		int generateGeometry(int width, int height);

		// lock regenerates the texture after you're done with the pixels for the renderer to keep drawing
		void lock() { _locked = true; generateTexture(); generateGeometry(width(), height()); }
		bool locked() { return _locked; }

	public:
		rt::PixelBuffer pixelbuffer;
		uint8_t scale;
		rt::vec2i position;

	private:
		GLuint _texture;
		GLuint _vertexbuffer;
		GLuint _uvbuffer;

		bool _locked = false;
};

} // namespace cnv

#endif /* CANVAS_H */
