#ifndef PIXELBUFFER_H
#define PIXELBUFFER_H

#include <iostream>
#include <fstream>
#include <vector>

#include <canvas/color.h>

namespace rt {

// =========================================================
struct vec2i {
    int x = 0;
    int y = 0;

    vec2i() : x(0), y(0) {}
    vec2i(int x, int y) : x(x), y(y) {}
    vec2i(const vec2i& v) : x(v.x), y(v.y) {}

    inline vec2i  operator+ (const vec2i& rhs) const { return vec2i(x+rhs.x, y+rhs.y); }
    inline vec2i& operator+=(const vec2i& rhs) { x += rhs.x; y += rhs.y; return *this; }
    inline vec2i  operator- (const vec2i& rhs) const { return vec2i(x-rhs.x, y-rhs.y); }
    inline vec2i& operator-=(const vec2i& rhs) { x -= rhs.x; y -= rhs.y; return *this; }
    inline vec2i  operator* (const vec2i& rhs) const { return vec2i(x*rhs.x, y*rhs.y); }
    inline vec2i& operator*=(const vec2i& rhs) { x *= rhs.x; y *= rhs.y; return *this; }
    inline vec2i  operator/ (const vec2i& rhs) const { if (rhs.x != 0 && rhs.y != 0) { return vec2i(x/rhs.x, y/rhs.y); } else { return *this; } }
    inline vec2i& operator/=(const vec2i& rhs) { if(rhs.x != 0 && rhs.y != 0) {x /= rhs.x; y /= rhs.y; } return *this; }

    inline vec2i  operator+ (int rhs) const { return vec2i(x+rhs, y+rhs); }
    inline vec2i& operator+=(int rhs) { x += rhs; y += rhs; return *this; }
    inline vec2i  operator- (int rhs) const { return vec2i(x-rhs, y-rhs); }
    inline vec2i& operator-=(int rhs) { x -= rhs; y -= rhs; return *this; }
    inline vec2i  operator* (int rhs) const { return vec2i(x*rhs, y*rhs); }
    inline vec2i& operator*=(int rhs) { x *= rhs; y *= rhs; return *this; }
    inline vec2i  operator/ (int rhs) const { if (rhs != 0) { return vec2i(x/rhs, y/rhs); } else { return *this; } }
    inline vec2i& operator/=(int rhs) { if(rhs != 0) {x /= rhs; y /= rhs; } return *this; }

    inline bool operator==(const vec2i& rhs) const { return (x==rhs.x && y==rhs.y); }
    inline bool operator!=(const vec2i& rhs) const { return !(*this == rhs); }

    inline bool operator< (const vec2i& rhs) const { return (this->magSQ() < rhs.magSQ()); }
    inline bool operator> (const vec2i& rhs) const { return rhs < *this; }
    inline bool operator<=(const vec2i& rhs) const { return !(*this > rhs); }
    inline bool operator>=(const vec2i& rhs) const { return !(*this < rhs); }

    inline int magSQ() const { return ((x*x) + (y*y)); }
    inline int mag()   const { return sqrt(magSQ()); }
};
inline std::ostream& operator<<(std::ostream& os, const vec2i& obj) { return os << "(" << obj.x << ", " << obj.y << ")"; }


inline vec2i wrap(const vec2i& pos, int cols, int rows) {
    vec2i wrapped(pos);

    if (pos.x < 0) { wrapped.x = cols-1; }
    if (pos.x > cols-1) { wrapped.x = 0; }
    if (pos.y < 0) { wrapped.y = rows-1; }
    if (pos.y > rows-1) { wrapped.y = 0; }

    return wrapped;
}

inline int idFromPos(int x, int y, int cols) {
    return (y*cols)+x;
}

inline int idFromPos(const vec2i& pos, int cols) {
    return idFromPos(pos.x, pos.y, cols);
}

// =========================================================

class PixelBuffer {
private:
    struct PBHeader {
        uint8_t typep = 0x70;      // 1 byte: 0x70 = 'p'
        uint8_t typeb = 0x62;      // 1 byte: 0x62 = 'b'
        uint16_t width = 0x0000;   // 2 bytes: 0-65535
        uint16_t height = 0x0000;  // 2 bytes: 0-65535
        uint8_t bitdepth = 0x00;   // 1 byte: 8, 16, 24, 32
        uint8_t end = 0x3A;        // 1 byte: 0x3A = ':'
    };                             // sizeof(PBHeader) = 8 bytes
    // uint64_t header = 0x706208000400203A; // 8x4 pixels, bitdepth 32, Little Endian
    // uint64_t header = 0x706200080004203A; // 8x4 pixels, bitdepth 32, Big Endian

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
        _pixels.reserve(width*height);
        for (size_t i = 0; i < numpixels; i++) {
            _pixels.emplace_back(0, 0, 0, 255);
        }
    }

    PixelBuffer(const std::string& filename)
    {
        read(filename);
    }

    PixelBuffer(const PixelBuffer& other)
    {
        _header.width = other._header.width;
        _header.height = other._header.height;
        _header.bitdepth = other._header.bitdepth;

        size_t numpixels = _header.width * _header.height;
        for (size_t i = 0; i < numpixels; i++) {
            _pixels.push_back(other._pixels[i]);
        }
    }

    ~PixelBuffer()
    {
        _header.width = 0;
        _header.height = 0;
        _pixels.clear();
    }

    void printInfo() const
    {
        uint16_t width = _header.width;
        uint16_t height = _header.height;
        uint8_t bitdepth = _header.bitdepth;

        // std::cout << "sizeof(header): " << sizeof(_header) << " B" << std::endl;
        std::cout << "width: " << width << " pixels" << std::endl;
        std::cout << "height: " << height << " pixels" << std::endl;
        std::cout << "bitdepth: " << (int) bitdepth << " b/pixel" << std::endl;
        std::cout << "#number of pixels: " << (int) _pixels.size() << std::endl;
        std::cout << "memsize of pixels: " << (width * height * (bitdepth/8)) << " B";
        std::cout << " | " << (width * height * (bitdepth/8)) / 1024.0f << " KiB";
        std::cout << " | " << (width * height * (bitdepth/8)) / 1024 / 1024.0f << " MiB" << std::endl;
    }

    bool valid() const
	{
        return _header.typep == 0x70 &&
            _header.typeb == 0x62 &&
            (_header.bitdepth > 0 && _header.bitdepth < 33) &&
            _header.end == 0x3A &&
            _header.width * _header.height == _pixels.size();
    }

    int read(const std::string& filename)
    {
        // Try to open the file from disk
        std::ifstream file(filename, std::fstream::in|std::fstream::binary|std::fstream::ate);

        if (!file.is_open()) {
            std::cout << "Unable to open file: " << filename << std::endl;
            return 0;
        }

        // Read the file into a bytearray
        const int size = file.tellg();
        char* memblock = new char[size];
        file.seekg(0, std::fstream::beg);
        file.read(memblock, size);
        file.close();

        // Build header
        _header = *(PBHeader*)&memblock[0];

        // Build list of pixels
        size_t numpixels = _header.width * _header.height;
        _pixels.clear();
        _pixels.reserve(numpixels);
        size_t start = sizeof(_header);

        for (size_t i = 0; i < numpixels; i++) {
            RGBAColor pixel;
            if (_header.bitdepth == 8 || _header.bitdepth == 16) {
                char val = memblock[start+0];
                pixel.r = val;
                pixel.g = val;
                pixel.b = val;
                pixel.a = 255;
            }

            if (_header.bitdepth == 16) {
                pixel.a = memblock[start+1];
            }
            else if (_header.bitdepth == 24 || _header.bitdepth == 32) {
                pixel.r = memblock[start+0];
                pixel.g = memblock[start+1];
                pixel.b = memblock[start+2];
                pixel.a = 255;
            }

            if (_header.bitdepth == 32) {
                pixel.a = memblock[start+3];
            }

            _pixels.emplace_back(pixel);

            start += _header.bitdepth / 8;
        }

        delete[] memblock;

        return size;
    }

    int write(const std::string& filename) const
    {
        // Try to write to a file
        std::ofstream file(filename, std::fstream::out|std::fstream::binary|std::fstream::trunc);
        if (!file.is_open()) {
            std::cout << "Unable to write to file: " << filename << std::endl;
            return 0;
        }

        // Write header
        file.write((char*)&_header, sizeof(_header));

        // Write pixeldata
        // This would work if we only needed to write 4 bytes/pixel files:
        // file.write((char*)&_pixels[0], _pixels.size()*_header.bitdepth);

        // But we also need to handle the bitdepth
        for (auto& pixel : _pixels) {
            if (_header.bitdepth == 8 || _header.bitdepth == 16) {
                char value = (pixel.r + pixel.b + pixel.g) / 3;
                file.write(&value, 1);
            }

            if (_header.bitdepth == 16) {
                file.write((char*)&pixel.a, 1);
            }
            else if (_header.bitdepth == 24 || _header.bitdepth == 32) {
                file.write((char*)&pixel.r, 1);
                file.write((char*)&pixel.g, 1);
                file.write((char*)&pixel.b, 1);
            }

            if (_header.bitdepth == 32) {
                file.write((char*)&pixel.a, 1);
            }
        }

        file.close();

        return 1;
    }

	PixelBuffer copy(uint16_t x, uint16_t y, uint16_t width, uint16_t height) const
	{
		PixelBuffer buffer = PixelBuffer(width, height, header().bitdepth);

		size_t maxheight = height + y;
		size_t maxwidth = width + x;
		for (size_t ny = y; ny < maxheight; ny++) {
			for (size_t nx = x; nx < maxwidth; nx++) {
				RGBAColor pixel = getPixel(nx, ny);
				buffer.setPixel(nx-x, ny-y, pixel);
			}
		}

		return buffer;
	}

	int paste(const PixelBuffer& brush, short pos_x, short pos_y)
	{
		size_t height = brush.header().height;
		size_t width = brush.header().width;
		// size_t bitdepth = brush.header().bitdepth;
		for (size_t y = 0; y < height; y++) {
			for (size_t x = 0; x < width; x++) {
				RGBAColor color = brush.getPixel(x, y);
				// TODO handle alpha
				// if (bitdepth == 24 || bitdepth == 16) { }
				setPixel(x+pos_x, y+pos_y, color);
			}
		}

		return 1;
	}

    int setPixel(int x, int y, RGBAColor color)
    {
		// Sanity check
		if ( (x < 0) || (x >_header.width) || (y < 0) || (y > _header.height) ) {
			return 0;
		}

        size_t index = (y * _header.width) + x;
		if (index >= _pixels.size()) { // invalid pixels!
			return 0;
		}

        _pixels[index] = color;

		return 1;
    }

    RGBAColor getPixel(int x, int y) const
    {
		// Sanity check
		if ( (x < 0) || (x >_header.width) || (y < 0) || (y > _header.height) ) {
			return { 0, 0, 0, 0 };
		}

        size_t index = (y * _header.width) + x;
		if (index >= _pixels.size()) { // invalid pixels!
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
		drawLine(x0,    y0,    x0+x1, y0,    color);
		drawLine(x0+x1, y0,    x0+x1, y0+y1, color);
		drawLine(x0,    y0+y1, x0+x1, y0+y1, color);
		drawLine(x0,    y0,    x0,    y0+y1, color);
	}

    void fill(RGBAColor color) {
        for (size_t y = 0; y < _header.height; y++) {
            for (size_t x = 0; x < _header.width; x++) {
                setPixel(x, y, color);
            }
        }
	}

	void drawCircle(int circlex, int circley, int radius, RGBAColor color) {
		int x = radius;
		int y = 0;
		int err = 0;

		std::vector<rt::vec2i> positions;

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

	void blur() {
		size_t rows = _header.height;
		size_t cols = _header.width;

		for (size_t y = 0; y < rows; y++) {
			for (size_t x = 0; x < cols; x++) {
				// check surrounding colors and average values
				int totalr = 0; // total red
				int totalg = 0; // total green
				int totalb = 0; // total blue
				int totala = 0; // total alpha
				for (int r = -1; r < 2; r++) {
					for (int c = -1; c < 2; c++) {
						rt::vec2i n = rt::wrap(rt::vec2i(x+c, y+r), cols, rows);
                        rt::RGBAColor color = getPixel(n.x, n.y);
						totalr += color.r;
						totalg += color.g;
						totalb += color.b;
						totala += color.a;
					}
				}
                uint8_t r = totalr / 9;
                uint8_t g = totalg / 9;
                uint8_t b = totalb / 9;
                uint8_t a = totala / 9;
                rt::RGBAColor avg = { r, g, b, a };
				setPixel(x, y, avg);
			}
		}
	}

}; // PixelBuffer

} // namespace rt

#endif // PIXELBUFFER_H
