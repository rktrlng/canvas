/**
 * @file m_particles.cpp
 *
 * @brief m_particles implementation
 *
 * Copyright 2021-2022 @rktrlng
 * https://github.com/rktrlng/canvas
 */

#include <ctime>
#include <deque>

#include <canvas/application.h>

const int MAX_PARTICLES = 210;
const int HOR_SPREAD = 150;
const int VER_SPREAD = 200;
const float GRAVITY = 500.0f;
const float FRICTION = 0.992f;
const float ROT_SPEED = 0.0025f;
const bool BLUR = true;

struct Particle
{
	rt::vec2 position;
	rt::vec2 velocity;
	rt::vec2 acceleration;
	float mass;
	rt::RGBAColor color;

	Particle(float x, float y)
	{
		position = rt::vec2(x, y);
		velocity = rt::vec2((rand()%HOR_SPREAD) - (HOR_SPREAD/2), -(rand()%VER_SPREAD));
		acceleration = rt::vec2(0.0f, 0.0f);
		mass = 1.0f;
		color = RED;
	}

	void move(float delta)
	{
		position += velocity * delta;
		velocity += acceleration * delta;
		acceleration *= 0;
	}

	void addForce(rt::vec2 force)
	{
		acceleration += force / mass;
	}
};


class MyApp : public cnv::Application
{
private:
	std::deque<Particle*> m_particles;

public:
	MyApp(uint16_t width, uint16_t height, uint8_t bitdepth, uint8_t factor) : cnv::Application(width, height, bitdepth, factor)
	{
		std::srand(std::time(nullptr));
		layers[0]->pixelbuffer.fill(BLACK);
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
		handleParticles(deltatime);
	}

private:
	void borders(Particle* particle, int cols, int rows)
	{
		if (particle->position.y > rows) {
			particle->position.y = rows-1;
			particle->velocity.y *= -1;
		}
		if (particle->position.x > cols) {
			particle->position.x = cols-1;
			particle->velocity.x *= -1;
		}
		if (particle->position.x < 0) {
			particle->position.x = 0;
			particle->velocity.x *= -1;
		}
		if (particle->position.y < 0) {
			particle->position.y = 0;
			particle->velocity.y *= -1;
		}
	}

	void handleParticles(float deltaTime)
	{
		static float frametime = 0.0f;
		float maxtime = 0.01667f - deltaTime;
		frametime += deltaTime;
		if (frametime >= maxtime) {
			auto& pixelbuffer = layers[0]->pixelbuffer;
			size_t rows = pixelbuffer.height();
			size_t cols = pixelbuffer.width();

			for (size_t i = 0; i < m_particles.size(); i++) {
				pixelbuffer.setPixel(m_particles[i]->position.x, m_particles[i]->position.y, BLACK);
				m_particles[i]->addForce(rt::vec2(0.0f, GRAVITY));
				m_particles[i]->move(frametime);
				m_particles[i]->velocity *= FRICTION;
				m_particles[i]->color = rt::rotate(m_particles[i]->color, ROT_SPEED);
				borders(m_particles[i], cols, rows);
				pixelbuffer.setPixel(m_particles[i]->position.x, m_particles[i]->position.y, m_particles[i]->color);
			}

			if (m_particles.size() < MAX_PARTICLES) {
				Particle* p = new Particle(cols/2, rows/4);
				m_particles.push_back(p);
			}
			else {
				pixelbuffer.setPixel(m_particles[0]->position.x, m_particles[0]->position.y, BLACK);
				m_particles.pop_front();
			}

			frametime = 0.0f;
			if (BLUR) {
				pixelbuffer.blur();
			}
			layers[0]->lock();
		}
	}

	void handleInput()
	{
		if (input.getKeyDown(cnv::KeyCode::Space)) {
			std::cout << "spacebar pressed down." << std::endl;
			// layers[0]->pixelbuffer.printInfo();
			layers[0]->pixelbuffer.fill(BLACK);
			m_particles.clear();
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
	application.hideMouse();

	while (!application.quit())
	{
		application.run();
	}

	return 0;
}
