#ifndef VEC2_H
#define VEC2_H

#include <iostream>
#include <cmath>

namespace rt {

// vec2 definition
template <class T>
class vec2_t {
public:
    T x = 0;
    T y = 0;

    vec2_t<T>() : x(0), y(0) {}
    vec2_t<T>(T x, T y = 0) : x(x), y(y) {}
    vec2_t<T>(const vec2_t<T>& v) : x(v.x), y(v.y) {}

    inline vec2_t<T>  operator+ (const vec2_t<T>& rhs) const { return vec2_t<T>(x+rhs.x, y+rhs.y); }
    inline vec2_t<T>& operator+=(const vec2_t<T>& rhs) { x += rhs.x; y += rhs.y; return *this; }
    inline vec2_t<T>  operator- (const vec2_t<T>& rhs) const { return vec2_t<T>(x-rhs.x, y-rhs.y); }
    inline vec2_t<T>& operator-=(const vec2_t<T>& rhs) { x -= rhs.x; y -= rhs.y; return *this; }
    inline vec2_t<T>  operator* (const vec2_t<T>& rhs) const { return vec2_t<T>(x*rhs.x, y*rhs.y); }
    inline vec2_t<T>& operator*=(const vec2_t<T>& rhs) { x *= rhs.x; y *= rhs.y; return *this; }
    inline vec2_t<T>  operator/ (const vec2_t<T>& rhs) const { if (rhs.x != 0 && rhs.y != 0) { return vec2_t<T>(x/rhs.x, y/rhs.y); } else { return *this; } }
    inline vec2_t<T>& operator/=(const vec2_t<T>& rhs) { if(rhs.x != 0 && rhs.y != 0) {x /= rhs.x; y /= rhs.y; } return *this; }

    inline vec2_t<T>  operator+ (T rhs) const { return vec2_t<T>(x+rhs, y+rhs); }
    inline vec2_t<T>& operator+=(T rhs) { x += rhs; y += rhs; return *this; }
    inline vec2_t<T>  operator- (T rhs) const { return vec2_t<T>(x-rhs, y-rhs); }
    inline vec2_t<T>& operator-=(T rhs) { x -= rhs; y -= rhs; return *this; }
    inline vec2_t<T>  operator* (T rhs) const { return vec2_t<T>(x*rhs, y*rhs); }
    inline vec2_t<T>& operator*=(T rhs) { x *= rhs; y *= rhs; return *this; }
    inline vec2_t<T>  operator/ (T rhs) const { if (rhs != 0) { return vec2_t<T>(x/rhs, y/rhs); } else { return *this; } }
    inline vec2_t<T>& operator/=(T rhs) { if(rhs != 0) {x /= rhs; y /= rhs; } return *this; }

    inline bool operator==(const vec2_t<T>& rhs) const { return (x==rhs.x && y==rhs.y); }
    inline bool operator!=(const vec2_t<T>& rhs) const { return !(*this == rhs); }

    inline bool operator< (const vec2_t<T>& rhs) const { return (this->magSQ() < rhs.magSQ()); }
    inline bool operator> (const vec2_t<T>& rhs) const { return rhs < *this; }
    inline bool operator<=(const vec2_t<T>& rhs) const { return !(*this > rhs); }
    inline bool operator>=(const vec2_t<T>& rhs) const { return !(*this < rhs); }

    inline T magSQ() const { return ((x*x) + (y*y)); }
    inline T mag()   const { return sqrt(magSQ()); }

    inline T angle() const { return atan2(y, x); }
	inline vec2_t<T>& angle(T angle) { T m = mag(); x = cos(angle) * m; y = sin(angle) * m; return *this; }

	inline static vec2_t<T> fromAngle(T angle) { return rt::vec2_t<T>(cos(angle), sin(angle)); }
};
// implementations
template <class T>
inline std::ostream& operator<<(std::ostream& os, const vec2_t<T>& obj) { return os << "(" << obj.x << ", " << obj.y << ")"; }

// typedefs
typedef vec2_t<short>  vec2s;
typedef vec2_t<int>    vec2i;
typedef vec2_t<float>  vec2f;
typedef vec2_t<double> vec2d;
typedef vec2f          vec2;


// helper functions
// =========================================================
inline vec2i wrap(const vec2i& pos, int cols, int rows) {
    vec2i wrapped(pos);

    if (pos.x < 0) { wrapped.x = cols-1; }
    if (pos.x > cols-1) { wrapped.x = 0; }
    if (pos.y < 0) { wrapped.y = rows-1; }
    if (pos.y > rows-1) { wrapped.y = 0; }

    return wrapped;
}

inline vec2i clamp(const vec2i& pos, int cols, int rows) {
    vec2i clamped(pos);

    if (pos.x < 0) { clamped.x = 0; }
    if (pos.x > cols-1) { clamped.x = cols-1; }
    if (pos.y < 0) { clamped.y = 0; }
    if (pos.y > rows-1) { clamped.y = rows-1; }

    return clamped;
}

inline int idFromPos(int x, int y, int cols) {
    return (y*cols)+x;
}

inline int idFromPos(const vec2i& pos, int cols) {
    return idFromPos(pos.x, pos.y, cols);
}

// map(973, 0, 1023, 0, 255); // returns: 242
inline double map(double x, double in_min, double in_max, double out_min, double out_max)
{
	return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
// =========================================================


} // namespace rt

#endif // VEC2_H
