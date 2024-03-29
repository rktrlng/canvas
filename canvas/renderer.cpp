/**
 * @file renderer.cpp
 * @brief cnv::Renderer implementation
 * @see https://github.com/rktrlng/pixelbuffer
 */

#include <cstdio>
#include <cstdlib>
#include <string>
#include <vector>
#include <fstream>

#include <canvas/renderer.h>


// GLFW3 Callbacks
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}



namespace cnv {

Renderer::Renderer(int w, int h) :
	_window_width(w), _window_height(h), _window(nullptr)
{
	this->init();
}

Renderer::~Renderer()
{
	// Cleanup VBO and shader
	glDeleteProgram(_programID);

	glfwDestroyWindow(_window);
}

int Renderer::init()
{
	// Initialise GLFW
	if( !glfwInit() )
	{
		fprintf( stderr, "Failed to initialize GLFW\n" );
		return -1;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_SAMPLES, 0);

	// Open a window and create its OpenGL context
	_window = glfwCreateWindow( _window_width, _window_height, "Canvas", NULL, NULL);
	if( _window == NULL ){
		fprintf( stderr, "Failed to open GLFW window.\n" );
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(_window);

	// Initialize GLEW
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		return -1;
	}

	// Ensure we can capture the escape key being pressed
	glfwSetInputMode(_window, GLFW_STICKY_KEYS, GL_TRUE);

	// show mouse cursor
	showMouse();

	// vsync (0=off, 1=on)
	glfwSwapInterval(0);

	// setup callback functions
	glfwSetFramebufferSizeCallback(_window, framebuffer_size_callback);

	// dark background
	glClearColor(0.2f, 0.2f, 0.2f, 0.0f);

	// Enable depth test
	//glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	//glDepthFunc(GL_LESS);

	// Cull triangles which normal is not towards the camera
	glEnable(GL_CULL_FACE);

	// Create and compile our GLSL program from the shaders
	_programID = this->loadShaders();

	_projectionMatrix = glm::ortho(0.0f, (float)_window_width, (float)_window_height, 0.0f, 0.1f, 100.0f);

	// View matrix
	static glm::vec3 position = glm::vec3( 0, 0, 10 ); // Initial position : on +Z
	_viewMatrix = glm::lookAt(
			position, /* Camera is at (xpos,ypos,zpos), in World Space */
			position + glm::vec3(0, 0, -1), /* and looks towards Z */
			glm::vec3(0, 1, 0)  /* Head is up */
		);

	// Use our shader
	glUseProgram(_programID);

	return 0;
}

float Renderer::updateDeltaTime() {
	// lastTime is initialised only the first time this function is called
	static double lastTime = glfwGetTime();
	// get the current time
	double currentTime = glfwGetTime();

	// Compute time difference between current and last time
	float deltaTime = float(currentTime - lastTime);

	// For the next frame, the "last time" will be "now"
	lastTime = currentTime;
	return deltaTime;
}

void Renderer::hideMouse()
{
	glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
}

void Renderer::showMouse()
{
	glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

bool Renderer::displayCanvas(Canvas* canvas, float px, float py, float sx, float sy, float rot)
{
	// Clear the screen
	glClear(GL_COLOR_BUFFER_BIT);

	renderCanvas(canvas, px, py, sx, sy, rot);

	// Swap buffers
	glfwSwapBuffers(_window);

	glfwPollEvents();
	if (glfwGetKey(_window, GLFW_KEY_ESCAPE) == GLFW_PRESS || glfwWindowShouldClose(_window)) {
		return false;
	}

	return true;
}

void Renderer::renderCanvas(Canvas* canvas, float px, float py, float sx, float sy, float rot)
{
	// Build the Model matrix
	glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(px, py, 0.0f));
	glm::mat4 rotationMatrix    = glm::eulerAngleYXZ(0.0f, 0.0f, rot);
	glm::mat4 scalingMatrix     = glm::scale(glm::mat4(1.0f), glm::vec3(sx, sy, 1.0f));

	glm::mat4 modelMatrix = translationMatrix * rotationMatrix * scalingMatrix;

	glm::mat4 MVP = _projectionMatrix * _viewMatrix * modelMatrix;

	// Send our transformation to the currently bound shader,
	// in the "MVP" uniform
	GLuint matrixID = glGetUniformLocation(_programID, "MVP");
	glUniformMatrix4fv(matrixID, 1, GL_FALSE, &MVP[0][0]);

	// pixelbuffer to opengl texture
	// also regenerate mesh in case of pb->read("file.pbf");
	if (!canvas->locked())
	{
		canvas->generateTexture();
		canvas->generateGeometry(canvas->width(), canvas->height());
	}

	// Bind our texture in Texture Unit 0
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, canvas->texture());
	// Set our "textureSampler" sampler to user Texture Unit 0
	GLuint textureID  = glGetUniformLocation(_programID, "textureSampler");
	glUniform1i(textureID, 0);

	// 1st attribute buffer : vertices
	GLuint vertexPositionID = glGetAttribLocation(_programID, "vertexPosition");
	glEnableVertexAttribArray(vertexPositionID);
	glBindBuffer(GL_ARRAY_BUFFER, canvas->vertexbuffer());
	glVertexAttribPointer(
		vertexPositionID, // The attribute we want to configure
		3,          // size : x+y+z => 3
		GL_FLOAT,   // type
		GL_FALSE,   // normalized?
		0,          // stride
		(void*)0    // array buffer offset
	);

	// 2nd attribute buffer : UVs
	GLuint vertexUVID = glGetAttribLocation(_programID, "vertexUV");
	glEnableVertexAttribArray(vertexUVID);
	glBindBuffer(GL_ARRAY_BUFFER, canvas->uvbuffer());
	glVertexAttribPointer(
		vertexUVID, // The attribute we want to configure
		2,          // size : U+V => 2
		GL_FLOAT,   // type
		GL_FALSE,   // normalized?
		0,          // stride
		(void*)0    // array buffer offset
	);

	// Draw the triangles
	glDrawArrays(GL_TRIANGLES, 0, 2*3); // 2*3 indices starting at 0 -> 2 triangles

	glDisableVertexAttribArray(vertexPositionID);
	glDisableVertexAttribArray(vertexUVID);
}

GLuint Renderer::loadShaders()
{
	// Create the shaders
	GLuint vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	// Vertex Shader code
	std::string vertexShaderCode;
	vertexShaderCode += "#version 120\n";
	vertexShaderCode += "uniform mat4 MVP;\n";
	vertexShaderCode += "attribute vec3 vertexPosition;\n";
	vertexShaderCode += "attribute vec2 vertexUV;\n";
	vertexShaderCode += "varying vec2 UV;\n";
	vertexShaderCode += "void main() {\n";
	vertexShaderCode += "  gl_Position =  MVP * vec4(vertexPosition, 1);\n";
	vertexShaderCode += "  UV = vertexUV;\n";
	vertexShaderCode += "}\n";

	// Fragment Shader code
	std::string fragmentShaderCode;
	fragmentShaderCode += "#version 120\n";
	fragmentShaderCode += "varying vec2 UV;\n";
	fragmentShaderCode += "uniform sampler2D textureSampler;\n";
	fragmentShaderCode += "void main() {\n";
	fragmentShaderCode += "  gl_FragColor = texture2D( textureSampler, UV );\n";
	fragmentShaderCode += "}\n";


	GLint result = GL_FALSE;
	int infoLogLength;

	// Compile Vertex Shader
	printf("Compiling vertex shader\n");
	char const * vertexSourcePointer = vertexShaderCode.c_str();
	glShaderSource(vertexShaderID, 1, &vertexSourcePointer , NULL);
	glCompileShader(vertexShaderID);

	// Check Vertex Shader
	glGetShaderiv(vertexShaderID, GL_COMPILE_STATUS, &result);
	glGetShaderiv(vertexShaderID, GL_INFO_LOG_LENGTH, &infoLogLength);
	if ( infoLogLength > 0 ){
		std::vector<char> vertexShaderErrorMessage(infoLogLength+1);
		glGetShaderInfoLog(vertexShaderID, infoLogLength, NULL, &vertexShaderErrorMessage[0]);
		printf("%s\n", &vertexShaderErrorMessage[0]);
	}

	// Compile Fragment Shader
	printf("Compiling fragment shader\n");
	char const * fragmentSourcePointer = fragmentShaderCode.c_str();
	glShaderSource(fragmentShaderID, 1, &fragmentSourcePointer , NULL);
	glCompileShader(fragmentShaderID);

	// Check Fragment Shader
	glGetShaderiv(fragmentShaderID, GL_COMPILE_STATUS, &result);
	glGetShaderiv(fragmentShaderID, GL_INFO_LOG_LENGTH, &infoLogLength);
	if ( infoLogLength > 0 ){
		std::vector<char> fragmentShaderErrorMessage(infoLogLength+1);
		glGetShaderInfoLog(fragmentShaderID, infoLogLength, NULL, &fragmentShaderErrorMessage[0]);
		printf("%s\n", &fragmentShaderErrorMessage[0]);
	}

	// Link the program
	printf("Linking program\n");
	GLuint programID = glCreateProgram();
	glAttachShader(programID, vertexShaderID);
	glAttachShader(programID, fragmentShaderID);
	glLinkProgram(programID);

	// Check the program
	glGetProgramiv(programID, GL_LINK_STATUS, &result);
	glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &infoLogLength);
	if ( infoLogLength > 0 ){
		std::vector<char> programErrorMessage(infoLogLength+1);
		glGetProgramInfoLog(programID, infoLogLength, NULL, &programErrorMessage[0]);
		printf("%s\n", &programErrorMessage[0]);
	}

	glDeleteShader(vertexShaderID);
	glDeleteShader(fragmentShaderID);

	return programID;
}


} // namespace cnv
