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
typedef vec2_t<short> vec2s;
typedef vec2_t<int> vec2i;
typedef vec2_t<float> vec2f;
typedef vec2_t<double> vec2d;
typedef vec2f vec2;

} // namespace rt

#endif // VEC2_H
