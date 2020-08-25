#ifndef PIXELBUFFER_H
#define PIXELBUFFER_H

#include <iostream>
#include <fstream>
#include <vector>

#include <common/color.h>

namespace rt {

class PixelBuffer {
private:
    //   +----------------------- typep (1 byte) 0x70 = 'p'
    //   |  +-------------------- typeb (1 byte) 0x62 = 'b'
    //   |  |   +---------------- width (2 bytes) 0-65535
    //   |  |   |     +---------- height (2 bytes) 0-65535
    //   |  |   |     |    +----- bitdepth (1 byte) 1, 3, 4
    //   |  |   |     |    |  +-- end (1 byte) 0x3A = ':'
    //   v  v   v     v    v  v
    // |  |  |  .  |  .  |  |  |
    //  -- -- -- -- -- -- -- --
    struct PBHeader {
        uint8_t typep = 0x70;
        uint8_t typeb = 0x62;
        uint16_t width = 0x0000;
        uint16_t height = 0x0000;
        uint8_t bitdepth = 0x04;
        uint8_t end = 0x3A;
    };
    // uint64_t imghdr = 0x706200080004013A; // 8x4 pixels, bitdepth 1, Big Endian
    // uint64_t imghdr = 0x706208000400013A; // 8x4 pixels, bitdepth 1, Little Endian

    PBHeader _header;
    std::vector<RGBAColor> _pixels;

public:
    const PBHeader header() const { return _header; }
    std::vector<RGBAColor>& pixels() { return _pixels; }
    const std::vector<RGBAColor>& pixels() const { return _pixels; }

    PixelBuffer()
    {
        // default header
        // empty list of pixels
    }

    PixelBuffer(uint16_t width, uint16_t height, uint8_t bitdepth)
    {
        _header.width = width;
        _header.height = height;
        _header.bitdepth = bitdepth;
        size_t numpixels = width * height;
        _pixels.reserve(numpixels);
        for (size_t i = 0; i < numpixels; i++) {
            _pixels.emplace_back( 0, 0, 0, 0 );
        }
    }

    PixelBuffer(const std::string& filename)
    {
        read(filename);
    }

    // Copy constructor
    PixelBuffer(const PixelBuffer& other)
    {
        _header.width = other._header.width;
        _header.height = other._header.height;
        _header.bitdepth = other._header.bitdepth;

        size_t numpixels = _header.width * _header.height;
        _pixels.reserve(numpixels);
        for (size_t i = 0; i < numpixels; i++) {
            _pixels.emplace_back( other._pixels[i] );
        }
    }

    ~PixelBuffer()
    {
        _header.width = 0;
        _header.height = 0;
        _pixels.clear();
    }

    void printInfo()
    {
        uint16_t width = _header.width;
        uint16_t height = _header.height;
        uint8_t bitdepth = _header.bitdepth;

        std::cout << "sizeof(header): " << sizeof(_header) << " B" << std::endl;
        std::cout << "width: " << width << " pixels" << std::endl;
        std::cout << "height: " << height << " pixels" << std::endl;
        std::cout << "bitdepth: " << (int) bitdepth << " B/pixel" << std::endl;
        std::cout << "#number of pixels: " << (int) _pixels.size() << std::endl;
        std::cout << "memsize of pixels: " << (width * height * bitdepth) << " B";
        std::cout << " | " << (width * height * bitdepth) / 1024.0f << " KiB";
        std::cout << " | " << (width * height * bitdepth) / 1024 / 1024.0f << " MiB" << std::endl;
    }

    bool valid() {
        return _header.typep == 0x70 &&
            _header.typeb == 0x62 &&
            (_header.bitdepth > 0 && _header.bitdepth < 5) &&
            _header.end == 0x3A &&
            _header.width * _header.height == _pixels.size();
    }

    int read(const std::string& filename)
    {
        // Try to read the file from disk:
        std::ifstream file(filename, std::fstream::in|std::fstream::binary|std::fstream::ate);
        if (!file.is_open()) {
            std::cout << "Unable to read file: " << filename << std::endl;
            return 0;
        }

        file.seekg(0, std::fstream::beg);

        char p; file.read(&p, 1); _header.typep = p;
        char b; file.read(&b, 1); _header.typeb = b;

        char highw; file.read(&highw, 1); // width
        char loww; file.read(&loww, 1);
        char highh; file.read(&highh, 1); // height
        char lowh; file.read(&lowh, 1);

        // Little Endian:
        _header.width = (loww << 8) | (highw & 0xFF);
        _header.height = (lowh << 8) | (highh & 0xFF);
        
        char d; file.read(&d, 1); _header.bitdepth = d;
        char e; file.read(&e, 1); _header.end = e;

        _pixels.clear();
        size_t numpixels = _header.width * _header.height;
        _pixels.reserve(numpixels);
        for (size_t i = 0; i < numpixels; i++)
        {
            RGBAColor pixel;
            if (_header.bitdepth == 1 || _header.bitdepth == 2)
            {
                char val; file.read(&val, 1);
                pixel.r = val;
                pixel.g = val;
                pixel.b = val;
                pixel.a = 255;
            }

            if (_header.bitdepth == 2)
            {
                char a; file.read(&a, 1); pixel.a = a;
            }
            else if (_header.bitdepth == 3 || _header.bitdepth == 4)
            {
                char r; file.read(&r, 1); pixel.r = r;
                char g; file.read(&g, 1); pixel.g = g;
                char b; file.read(&b, 1); pixel.b = b;
                pixel.a = 255;
            }

            if (_header.bitdepth == 4)
            {
                char a; file.read(&a, 1); pixel.a = a;
            }
            
            _pixels.emplace_back(pixel);
        }
        return 1;
    }

    int write(const std::string& filename)
    {
        // Try to write to a file
        std::ofstream file(filename, std::fstream::out|std::fstream::binary|std::fstream::trunc);
        if (!file.is_open())
        {
            std::cout << "Unable to write to file: " << filename << std::endl;
            return 0;
        }

        // Start writing the _header
        char typep = _header.typep; file.write(&typep, 1);  // 'p'
        char typeb = _header.typeb; file.write(&typeb, 1);  // 'b'

        // Little Endian:
        char highw = (_header.width & 0xFF); file.write(&highw, 1);
        char loww = ((_header.width >> 8) & 0xFF); file.write(&loww, 1); // 0-65535
        char highh = (_header.height & 0xFF); file.write(&highh, 1);
        char lowh = ((_header.height >> 8) & 0xFF); file.write(&lowh, 1); // 0-65535

        char depth = _header.bitdepth; file.write(&depth, 1);  // 1, 3, or 4
        char end = _header.end; file.write(&end, 1);  // ':'

        // Write pixeldata to the file
        for (auto& pixel : _pixels)
        {
            if (_header.bitdepth == 1 || _header.bitdepth == 2)
            {
                char value = (pixel.r + pixel.b + pixel.g) / 3;
                file.write(&value, 1);
            }

            if (_header.bitdepth == 2)
            {
                char a = pixel.a; file.write(&a, 1);
            }
            else if (_header.bitdepth == 3 || _header.bitdepth == 4)
            {
                char r = pixel.r; file.write(&r, 1);
                char g = pixel.g; file.write(&g, 1);
                char b = pixel.b; file.write(&b, 1);
            }

            if (_header.bitdepth == 4)
            {
                char a = pixel.a; file.write(&a, 1);
            }
        }

        file.close();
        return 1;
    }

    int setPixel(int x, int y, RGBAColor color)
    {
		// Sanity check
		if ( (x < 0) || (x >_header.width) || (y < 0) || (y > _header.height) ) {
			return 0;
		}

        size_t index = (y * _header.width) + x;
		if (index > _pixels.size()) { // invalid pixels!
			return 0;
		}

        _pixels[index] = color;

		return 1;
    }

    RGBAColor getPixel(int x, int y)
    {
		// Sanity check
		if ( (x < 0) || (x >_header.width) || (y < 0) || (y > _header.height) ) {
			return { 0, 0, 0, 0 };
		}

        size_t index = (y * _header.width) + x;
		if (index > _pixels.size()) { // invalid pixels!
			return { 0, 0, 0, 0 };
		}

		return _pixels[index];
    }

	void drawLine(int x0, int y0, int x1, int y1, RGBAColor color)
	{
		bool steep = false;
		if (std::abs(x0-x1) < std::abs(y0-y1)) {
			std::swap(x0, y0);
			std::swap(x1, y1);
			steep = true;
		}
		if (x0 > x1) {
			std::swap(x0, x1);
			std::swap(y0, y1);
		}
		int dx = x1-x0;
		int dy = y1-y0;
		int derror2 = std::abs(dy)*2;
		int error2 = 0;
		int y = y0;

		for (int x = x0; x <= x1; x++) {
			if (steep) {
				setPixel(y, x, color);
			} else {
				setPixel(x, y, color);
			}
			error2 += derror2;

			if (error2 > dx) {
				y += (y1 > y0 ? 1 : -1);
				error2 -= dx*2;
			}
		}
	}

	void drawSquare(int x0, int y0, int x1, int y1, RGBAColor color) {
		drawLine(x0,    y0   , x0+x1, y0,    color);
		drawLine(x0+x1, y0   , x0+x1, y0+y1, color);
		drawLine(x0,    y0+y1, x0+x1, y0+y1, color);
		drawLine(x0,    y0,    x0,    y0+y1, color);
	}

    void fill(RGBAColor color) {
        for (size_t i = 0; i < _header.height; i++) {
            drawLine(0, i, _header.width, i, color);
        }
	}

	void drawCircle(int circlex, int circley, int radius, RGBAColor color) {
		int x = radius;
		int y = 0;
		int err = 0;

		struct LocalPos {
			int x = 0;
			int y = 0;
			LocalPos(int _x, int _y) : x(_x), y(_y) {}
		};
		std::vector<LocalPos> positions;

		while (x >= y) {
			positions.push_back( {  x,  y } );
			positions.push_back( {  y,  x } );
			positions.push_back( { -y,  x } );
			positions.push_back( { -x,  y } );
			positions.push_back( { -x, -y } );
			positions.push_back( { -y, -x } );
			positions.push_back( {  y, -x } );
			positions.push_back( {  x, -y } );

			if (err <= 0) {
				y += 1;
				err += 2*y + 1;
			}
			if (err > 0) {
				x -= 1;
				err -= 2*x + 1;
			}
		}

		for (auto local : positions) {
			setPixel(circlex + local.x, circley + local.y, color);
		}
	}

};

} // namespace rt

#endif // PIXELBUFFER_H
