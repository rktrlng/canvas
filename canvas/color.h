#ifndef COLOR_H_
#define COLOR_H_

#include <cstdint>
#include <algorithm>
#include <cmath>

namespace rt {

/// @brief A 32 bit RGBA color.
///
/// A struct that defines an RGBA Color. Each value is a uint8_t (0-255).
struct RGBAColor
{
	/// @brief The red component of the color
	uint8_t r = 255;
	/// @brief The green component of the color
	uint8_t g = 255;
	/// @brief The blue component of the color
	uint8_t b = 255;
	/// @brief The alpha component of the color
	uint8_t a = 255;

	/// @brief constructor
	RGBAColor();
	/// @brief constructor grayscale color with alpha
	/// @param value The gray component of the color
	/// @param alpha The alpha component of the color
	RGBAColor(uint8_t value, uint8_t alpha = 255);
	/// @brief constructor RGBA color
	/// @param red The red component of the color
	/// @param green The green component of the color
	/// @param blue The blue component of the color
	/// @param alpha The alpha component of the color
	RGBAColor(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha = 255);
	/// @param color constructor for color as 32 bits int
	RGBAColor(uint32_t color);
	/// @brief get color as a uint32_t
	/// @return uint32_t color as a 32 bits int
	uint32_t asInt();
	/// @brief == operator overloader
	/// @param rhs the color to compare against
	/// @return bool equal or not
	inline bool operator==(const RGBAColor& rhs) {
		return ( r == rhs.r && g == rhs.g && b == rhs.b && a == rhs.a );
	}
	/// @brief != operator overloader
	/// @param rhs the color to compare against
	/// @return bool equal or not
	inline bool operator!=(const RGBAColor& rhs) {
		return !(*this == rhs);
	}
};

/// @brief A 24 bit HSVA color.
///
/// A struct that defines an HSV Color (Hue, Saturation, Brightness). Each value is a float between 0.0f and 1.0f.
struct HSVAColor
{
	/// @brief The Hue component of the color
	float h = 0.0f;
	/// @brief The Saturation component of the color
	float s = 0.0f;
	/// @brief The Lightness/Brightness/Value component of the color
	float v = 1.0f;
	/// @brief The alpha component of the color
	float a = 1.0f;

	/// @brief constructor
	HSVAColor();
	/// @brief constructor
	/// @param hue The hue component of the color
	/// @param sat The saturation component of the color
	/// @param val The brightness/lightness/value component of the color
	/// @param alpha The alpha component of the color
	HSVAColor(float hue, float sat, float val, float alpha = 1.0f);
};


struct Color {
	// http://www.easyrgb.com/index.php?X=MATH&H=20#text20
	/// @brief RGBA to HSV conversion
	HSVAColor RGBA2HSVA(RGBAColor rgba);

	// http://www.easyrgb.com/index.php?X=MATH&H=21#text21
	/// @brief HSV to RGBA conversion
	RGBAColor HSVA2RGBA(HSVAColor hsva);

	/// @brief Rotate RGBA color (use HSVA)
	RGBAColor rotate(RGBAColor rgba, float step);

	// https://stackoverflow.com/questions/28900598/how-to-combine-two-colors-with-varying-alpha-values
	// https://stackoverflow.com/questions/2030471/alpha-blending-a-red-blue-and-green-image-to-produce-an-image-tinted-to-any-rg/2030560#2030560
	// https://en.wikipedia.org/wiki/Alpha_compositing#Alpha_blending
	RGBAColor alphaBlend(RGBAColor a, RGBAColor b);

	/// @brief lerp from color to another color
	/// @param c1 first RGBAColor
	/// @param c2 second RGBAColor
	/// @param amount between 0 and 1
	/// @brief return RGBAColor lerped color
	RGBAColor lerpColor(RGBAColor c1, RGBAColor c2, float amount);
};

#define BLACK   RGBAColor(0,   0,   0,   255) ///< @brief color black
#define GRAY    RGBAColor(127, 127, 127, 255) ///< @brief color gray
#define RED     RGBAColor(255, 0,   0,   255) ///< @brief color red
#define ORANGE  RGBAColor(255, 127, 0,   255) ///< @brief color orange
#define YELLOW  RGBAColor(255, 255, 0,   255) ///< @brief color yellow
#define GREEN   RGBAColor(0,   255, 0,   255) ///< @brief color green
#define CYAN    RGBAColor(0,   255, 255, 255) ///< @brief color cyan
#define BLUE    RGBAColor(0,   0,   255, 255) ///< @brief color blue
#define MAGENTA RGBAColor(255, 0,   255, 255) ///< @brief color magenta
#define PINK    RGBAColor(255, 127, 255, 255) ///< @brief color pink
#define WHITE   RGBAColor(255, 255, 255, 255) ///< @brief color white

} // namespace rt

#endif /* COLOR_H_ */
