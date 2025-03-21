/**
 * @file fredkinreplicator.cpp
 *
 * @brief Fredkin Replicator implementation
 *
 * Copyright 2025 @rktrlng
 * https://github.com/rktrlng/canvas
 */

#include <ctime>

#include <canvas/application.h>

class MyApp : public cnv::Application
{
public:
    // MyApp(uint16_t width, uint16_t height, uint8_t bitdepth, uint8_t factor) : cnv::Application(width, height, bitdepth, factor)
    // {
    // 	init();
    // }

    MyApp(rt::PixelBuffer &pixelbuffer, uint8_t factor) : cnv::Application(pixelbuffer, factor)
    {
        init();
    }

    virtual ~MyApp()
    {
    }

    void init()
    {
        srand((unsigned)time(nullptr));

        auto &pixelbuffer = layers[0]->pixelbuffer;
        uint16_t cols = pixelbuffer.width();
        uint16_t rows = pixelbuffer.height();
        pixelbuffer.fill(BLACK);

        // fill field for fredkin replicator
        m_field = std::vector<uint8_t>(rows * cols, 0);
        int center = ((rows*cols)/2) + (cols/2);
        m_field[center] = 1;
    }

    void update(float deltatime) override
    {
        handleInput();

        static float frametime = 0.0f;
        float maxtime = 0.5f;
        frametime += deltatime;

        int lastgeneration = 64;
        static int currentgeneration = 0;
        if (frametime >= maxtime)
        {
            currentgeneration++;
            if (currentgeneration > lastgeneration)
            {
                init();
                currentgeneration = 1;
            }
            std::cout << "generation: " << currentgeneration << "\n";

            fredkinreplicator();

            // std::string filename = "fredkin_";
            // filename.append(std::to_string(currentgeneration));
            // filename.append(".tga");
            // layers[0]->pixelbuffer.writeTGA(filename);
            
            layers[0]->lock();
            frametime = 0.0f;
        }
    }

private:
    const uint8_t DEAD = 0;  // BLACK
    const uint8_t ALIVE = 1; // WHITE

    // internal data to work with (value are 0,1)
    std::vector<uint8_t> m_field;

    void fredkinreplicator()
    {
        // get pixelbuffer, rows and cols
        auto &pixelbuffer = layers[0]->pixelbuffer;
        size_t rows = pixelbuffer.height();
        size_t cols = pixelbuffer.width();

        // set the next state
        std::vector<uint8_t> next = std::vector<uint8_t>(cols * rows, 0);
        for (size_t y = 0; y < rows; y++)
        {
            for (size_t x = 0; x < cols; x++)
            {
                int index = rt::index(x, y, cols);
                uint8_t current = m_field[index];

                // check 8 neighbours and count the ones that are a ALIVE
                int nc = 0; // number of neighbour cells
                for (int r = -1; r < 2; r++)
                {
                    for (int c = -1; c < 2; c++)
                    {
                        if (r == 0 && c == 0)
                        {
                            // this is us
                        }
                        else
                        {
                            rt::vec2i n = rt::wrap(rt::vec2i(x + c, y + r), cols, rows);
                            if (m_field[rt::index(n.x, n.y, cols)] == ALIVE)
                            {
                                nc++;
                            }
                        }
                    }
                }

                // Apply rules for each pixel:
                if (nc % 2 == 0)
                {
                    current = DEAD;
                }
                else
                {
                    current = ALIVE;
                }

                next[index] = current;

                // update pixelbuffer from (current) field
                rt::RGBAColor color;
                if (m_field[index] == ALIVE)
                {
                    color = WHITE;
                }
                else
                {
                    color = BLACK;
                }
                pixelbuffer.setPixel(x, y, color);
            }
        }

        // update field to next state
        m_field = next;
    }

    void handleInput()
    {
        if (input.getKeyDown(cnv::KeyCode::Space))
        {
            std::cout << "spacebar pressed down." << std::endl;
            layers[0]->pixelbuffer.printInfo();
            init();
            // layers[0]->pixelbuffer.write("gameoflife.pbf");
        }

        if (input.getMouseDown(0))
        {
            std::cout << "click " << (int)input.getMouseX() << "," << (int)input.getMouseY() << std::endl;
        }

        int scrolly = input.getScrollY();
        if (scrolly != 0)
        {
            std::cout << "scroll: " << scrolly << std::endl;
        }
    }
};

int main(void)
{
    rt::PixelBuffer pixelbuffer(128, 128, 24);
    MyApp application(pixelbuffer, 8);
    application.hideMouse();

    while (!application.quit())
    {
        application.run();
    }

    return 0;
}
