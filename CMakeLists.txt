# CMake entry point
cmake_minimum_required (VERSION 3.0)
project (Canvas)

if( CMAKE_BINARY_DIR STREQUAL CMAKE_SOURCE_DIR )
	message( FATAL_ERROR "Please select another Build Directory ! (and give it a clever name, like 'build')" )
endif()
if( CMAKE_SOURCE_DIR MATCHES " " )
	message( "Your Source Directory contains spaces. If you experience problems when compiling, this can be the cause." )
endif()
if( CMAKE_BINARY_DIR MATCHES " " )
	message( "Your Build Directory contains spaces. If you experience problems when compiling, this can be the cause." )
endif()

# Compile external dependencies
set(OpenGL_GL_PREFERENCE GLVND)
find_package(OpenGL REQUIRED)

if(UNIX)
	set(CMAKE_CXX_FLAGS "-Wall -Werror -std=c++11 -O2")

	# For leak-checking and profiling, add '-pg' to CXX flags
	# set(CMAKE_CXX_FLAGS "-Wall -pg -std=c++11 -O2")
	# valgrind --leak-check=full ./demo
	# ./demo # creates cmon.out
	# gprof demo gmon.out > analysis.txt
endif(UNIX)

if(WIN32)
	# Create a group for Visual Studio
	SOURCE_GROUP(common REGULAR_EXPRESSION ".*/common/.*" )
	# suppress pedantic warnings
	add_definitions( "/W1 /D_CRT_SECURE_NO_WARNINGS /wd4514 /nologo" )
endif(WIN32)

add_definitions( # g++ -Dxxx
	-DTW_STATIC
	-DTW_NO_LIB_PRAGMA
	-DTW_NO_DIRECT3D
	-DGLEW_STATIC
	-D_CRT_SECURE_NO_WARNINGS
)

# Compile external dependencies
add_subdirectory (external)

include_directories( # g++ -Ipath/to/headerfiles
	external/glfw-3.3/include/
	external/glm-0.9.9.2/
	external/glew-1.9.0/include/
	pixelbuffer/
	.
)

link_directories( # g++ -Lpath/to/library

)

set(ALL_GRAPHICS_LIBS
	${OPENGL_LIBRARY}
	glfw
	GLEW_190
)

# Canvas (libcanvas.a)
add_library(canvas # ar rcs libcanvas.a
	canvas/application.h
	canvas/application.cpp
	canvas/renderer.h
	canvas/renderer.cpp
	canvas/input.h
	canvas/input.cpp
	canvas/canvas.h
	canvas/canvas.cpp
	canvas/noise.h
	canvas/noise.cpp
)

#asciiart
add_executable(asciiart
	demo/asciiart.cpp
)

# Start
add_executable(start # g++ demo/start.cpp -o start
	demo/start.cpp
)
target_link_libraries(start # g++ -lcanvas
	canvas
	${ALL_GRAPHICS_LIBS}
)

# Demo
add_executable(demo # g++ demo/demo.cpp -o demo
	demo/demo.cpp
)
target_link_libraries(demo # g++ -lcanvas
	canvas
	${ALL_GRAPHICS_LIBS}
)
file(
	COPY demo/assets
	DESTINATION ${CMAKE_BINARY_DIR}
)

# Wireworld
add_executable(wireworld # g++ demo/wireworld.cpp -o wireworld
	demo/wireworld.cpp
)
target_link_libraries(wireworld # g++ -lcanvas
	canvas
	${ALL_GRAPHICS_LIBS}
)
file(
	COPY demo/assets
	DESTINATION ${CMAKE_BINARY_DIR}
)

# Cave
add_executable(cave # g++ demo/cave.cpp -o cave
	demo/cave.cpp
)
target_link_libraries(cave # g++ -lcanvas
	canvas
	${ALL_GRAPHICS_LIBS}
)

# gameoflife
add_executable(gameoflife # g++ demo/gameoflife.cpp -o gameoflife
	demo/gameoflife.cpp
)
target_link_libraries(gameoflife # g++ -lcanvas
	canvas
	${ALL_GRAPHICS_LIBS}
)
file(
	COPY demo/assets
	DESTINATION ${CMAKE_BINARY_DIR}
)

# voronoi
add_executable(voronoi # g++ demo/voronoi.cpp -o voronoi
	demo/voronoi.cpp
)
target_link_libraries(voronoi # g++ -lcanvas
	canvas
	${ALL_GRAPHICS_LIBS}
)

# noise
add_executable(noise # g++ demo/noise.cpp -o noise
	demo/noise.cpp
)
target_link_libraries(noise # g++ -lcanvas
	canvas
	${ALL_GRAPHICS_LIBS}
)

# automata
add_executable(automata # g++ demo/automata.cpp -o automata
	demo/automata.cpp
)
target_link_libraries(automata # g++ -lcanvas
	canvas
	${ALL_GRAPHICS_LIBS}
)

# font
add_executable(font # g++ demo/font.cpp -o font
	demo/font.cpp
)
target_link_libraries(font # g++ -lcanvas
	canvas
	${ALL_GRAPHICS_LIBS}
)
file(
	COPY demo/assets
	DESTINATION ${CMAKE_BINARY_DIR}
)

# pbfviewer
add_executable(pbfviewer # g++ demo/pbfviewer.cpp -o pbfviewer
	demo/pbfviewer.cpp
)
target_link_libraries(pbfviewer # g++ -lcanvas
	canvas
	${ALL_GRAPHICS_LIBS}
)
if(UNIX)
	install(
		FILES ${CMAKE_BINARY_DIR}/pbfviewer
		DESTINATION /usr/local/bin
		PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
	)
endif(UNIX)

# particles
add_executable(particles # g++ demo/particles.cpp -o particles
	demo/particles.cpp
)
target_link_libraries(particles # g++ -lcanvas
	canvas
	${ALL_GRAPHICS_LIBS}
)

# blur
add_executable(blur # g++ demo/blur.cpp -o blur
	demo/blur.cpp
)
target_link_libraries(blur # g++ -lcanvas
	canvas
	${ALL_GRAPHICS_LIBS}
)

# mazegenerator
add_executable(mazegenerator # g++ demo/mazegenerator.cpp -o mazegenerator
	demo/mazegenerator.cpp
)
target_link_libraries(mazegenerator # g++ -lcanvas
	canvas
	${ALL_GRAPHICS_LIBS}
)

# mazesolver
add_executable(mazesolver # g++ demo/mazesolver.cpp -o mazesolver
	demo/mazesolver.cpp
)
target_link_libraries(mazesolver # g++ -lcanvas
	canvas
	${ALL_GRAPHICS_LIBS}
)

# maze
add_executable(maze # g++ demo/maze.cpp -o maze
	demo/maze.cpp
)
target_link_libraries(maze # g++ -lcanvas
	canvas
	${ALL_GRAPHICS_LIBS}
)

# hitomezashi
add_executable(hitomezashi # g++ demo/hitomezashi.cpp -o hitomezashi
	demo/hitomezashi.cpp
)
target_link_libraries(hitomezashi # g++ -lcanvas
	canvas
	${ALL_GRAPHICS_LIBS}
)

# painter
add_executable(painter # g++ demo/painter.cpp -o painter
	demo/painter.cpp
)
target_link_libraries(painter # g++ -lcanvas
	canvas
	${ALL_GRAPHICS_LIBS}
)

# flowfield
add_executable(flowfield # g++ demo/flowfield.cpp -o flowfield
	demo/flowfield.cpp
)
target_link_libraries(flowfield # g++ -lcanvas
	canvas
	${ALL_GRAPHICS_LIBS}
)

# diffusion-limited-aggregation
add_executable(diffusion-limited-aggregation # g++ demo/diffusion-limited-aggregation.cpp -o diffusion-limited-aggregation
	demo/diffusion-limited-aggregation.cpp
)
target_link_libraries(diffusion-limited-aggregation # g++ -lcanvas
	canvas
	${ALL_GRAPHICS_LIBS}
)

# dithering-floyd-steinberg
add_executable(dithering-floyd-steinberg # g++ demo/dithering-floyd-steinberg.cpp -o dithering-floyd-steinberg
	demo/dithering-floyd-steinberg.cpp
)
target_link_libraries(dithering-floyd-steinberg # g++ -lcanvas
	canvas
	${ALL_GRAPHICS_LIBS}
)

# oscillation
add_executable(oscillation # g++ demo/oscillation.cpp -o oscillation
	demo/oscillation.cpp
)
target_link_libraries(oscillation # g++ -lcanvas
	canvas
	${ALL_GRAPHICS_LIBS}
)

# cube
add_executable(cube # g++ demo/cube.cpp -o cube
	demo/cube.cpp
)
target_link_libraries(cube # g++ -lcanvas
	canvas
	${ALL_GRAPHICS_LIBS}
)

# bezier
add_executable(bezier # g++ demo/bezier.cpp -o bezier
	demo/bezier.cpp
)
target_link_libraries(bezier # g++ -lcanvas
	canvas
	${ALL_GRAPHICS_LIBS}
)

# nca
add_executable(nca # g++ demo/nca.cpp -o nca
	demo/nca.cpp
)
target_link_libraries(nca # g++ -lcanvas
	canvas
	${ALL_GRAPHICS_LIBS}
)
