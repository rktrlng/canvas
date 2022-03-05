/**
 * @file canvas.cpp
 *
 * @brief cnv::Canvas implementation
 *
 * Copyright 2021-2022 @rktrlng
 * https://github.com/rktrlng/canvas
 */

#include <iostream>
#include <cstdio>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/euler_angles.hpp>

#include <canvas/canvas.h>

namespace cnv {

Canvas::Canvas(uint16_t width, uint16_t height, uint8_t bitdepth /* = 32 */, uint8_t scale /* = 1 */) :
	pixelbuffer(width, height, bitdepth), scale(scale), position(pb::vec2i(0, 0))
{
	// pixelbuffer = { width, height, bitdepth };
	generateGeometry(width, height); // _vertexbuffer & _uvbuffer
	generateTexture(); // _texture
}

Canvas::Canvas(const pb::PixelBuffer& pb) : pixelbuffer(pb)
{
	// pixelbuffer = pb;
	generateGeometry(pixelbuffer.header().width, pixelbuffer.header().height); // _vertexbuffer & _uvbuffer
	generateTexture(); // _texture
}

Canvas::Canvas(const std::string& imagepath)
{
	pixelbuffer.read(imagepath);
	generateGeometry(pixelbuffer.header().width, pixelbuffer.header().height); // _vertexbuffer & _uvbuffer
	generateTexture(); // _texture
}

Canvas::~Canvas()
{
	glDeleteBuffers(1, &_vertexbuffer); // mesh created in generateGeometry() with glGenBuffers()
	glDeleteBuffers(1, &_uvbuffer);
	glDeleteTextures(1, &_texture); // texture created in generateTexture() with glGenTextures()
}

int Canvas::generateGeometry(int width, int height)
{
	// delete what we have
	glDeleteBuffers(1, &_vertexbuffer);
	glDeleteBuffers(1, &_uvbuffer);

	// Our vertices. Tree consecutive floats give a 3D vertex; Three consecutive vertices give a triangle.
	// A sprite has 1 face (quad) with 2 triangles each, so this makes 1*2=2 triangles, and 2*3 vertices
	GLfloat g_vertex_buffer_data[18] = {
		 0.5f * width, -0.5f * height, 0.0f,
		-0.5f * width, -0.5f * height, 0.0f,
		-0.5f * width,  0.5f * height, 0.0f,

		-0.5f * width,  0.5f * height, 0.0f,
		 0.5f * width,  0.5f * height, 0.0f,
		 0.5f * width, -0.5f * height, 0.0f
	};

	// Two UV coordinates for each vertex.
	GLfloat g_uv_buffer_data[12] = {
		1.0f, 0.0f,
		0.0f, 0.0f,
		0.0f, 1.0f,

		0.0f, 1.0f,
		1.0f, 1.0f,
		1.0f, 0.0f
	};

	glGenBuffers(1, &_vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, _vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

	glGenBuffers(1, &_uvbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, _uvbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_uv_buffer_data), g_uv_buffer_data, GL_STATIC_DRAW);

	return 1;
}

GLuint Canvas::generateTexture()
{
	// delete what we have
	glDeleteTextures(1, &_texture);

	// Create one OpenGL texture
	// Be sure to also delete it from where you called this with glDeleteTextures()
	glGenTextures(1, &_texture);

	// "Bind" the newly created texture : all future texture functions will modify this texture
	glBindTexture(GL_TEXTURE_2D, _texture);

	// No filtering.
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	
	// wrapping
	// GL_REPEAT, GL_MIRRORED_REPEAT or GL_CLAMP_TO_EDGE
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	auto data = pixelbuffer.pixels();
	size_t width = pixelbuffer.header().width;
	size_t height = pixelbuffer.header().height;

	// all our pixelbuffers are RGBA colors (so handle alpha)
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, &data[0]);

	// Return the ID of the texture we just created
	return _texture;
}

} // namespace cnv
