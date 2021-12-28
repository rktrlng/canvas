/**
 * @file color.h
 *
 * @brief The Color header file.
 *
 * - Copyright 2015 Rik Teerling <rik@onandoffables.com>
 *   - Initial commit
 */

#ifndef COLOR_H_
#define COLOR_H_

#include <cstdint>
#include <algorithm>
#include <cmath>

namespace rt {

/// @brief A 24 bit HSV color.
///
/// A struct that defines an HSV Color (Hue, Saturation, Brightness). Each value is a float between 0.0f and 1.0f.
struct HSVColor
{
	/// @brief The Hue component of the color
	float h = 0.0f;
	/// @brief The Saturation component of the color
	float s = 0.0f;
	/// @brief The Lightness/Brightness/Value component of the color
	float v = 1.0f;
	/// @brief The Alpha component of the color
	float a = 1.0f;

	/// @brief constructor
	HSVColor() {
		h = 0.0f;
		s = 0.0f;
		v = 1.0f;
		a = 1.0f;
	}
	/// @brief constructor
	/// @param hue The hue component of the color
	/// @param sat The saturation component of the color
	/// @param val The brightness/lightness/value component of the color
	HSVColor(float hue, float sat, float val, float alpha = 1.0f) {
		h = hue;
		s = sat;
		v = val;
		a = alpha;
	}
};


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
	RGBAColor() {
		r = 255;
		g = 255;
		b = 255;
		a = 255;
	}
	/// @brief constructor
	/// @param red The red component of the color
	/// @param green The green component of the color
	/// @param blue The blue component of the color
	/// @param alpha The alpha component of the color
	RGBAColor(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha) {
		r = red;
		g = green;
		b = blue;
		a = alpha;
	}
	/// @brief constructor
	/// @param red The red component of the color
	/// @param green The green component of the color
	/// @param blue The blue component of the color
	RGBAColor(uint8_t red, uint8_t green, uint8_t blue) {
		r = red;
		g = green;
		b = blue;
		a = 255;
	}
	/// @brief constructor
	/// @param color The color as a 32 bits int
	RGBAColor(uint32_t color) {
		r = color >> 24 & 0xFF;
		g = color >> 16 & 0xFF;
		b = color >> 8 & 0xFF;
		a = color & 0xFF;
	}
	/// @brief get color as a uint32_t
	/// @return uint32_t color as a 32 bits int
	uint32_t asInt() {
		uint32_t color = (r << 24) + (g << 16) + (b << 8) + (a);
		return color;
	}
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


/// @brief HSV <-> RGBA conversion
struct Color
{
	// http://www.easyrgb.com/index.php?X=MATH&H=20#text20
	/// @brief RGBA to HSV conversion
	static HSVColor RGBA2HSV(RGBAColor rgba) {
		float var_R = (float) rgba.r / 255; //RGB from 0 to 255
		float var_G = (float) rgba.g / 255;
		float var_B = (float) rgba.b / 255;
		float var_A = (float) rgba.a / 255;

		float var_Min = std::min( std::min( var_R, var_G), var_B ); // Min. value of RGB
		float var_Max = std::max( std::max( var_R, var_G), var_B ); // Max. value of RGB
		float del_Max = var_Max - var_Min; // Delta RGB value

		float H = 0.0f;
		float S = 0.0f;
		float V = var_Max;
		float A = var_A;

		if ( del_Max == 0 ) { //This is a gray, no chroma...
			H = 0; // HSV results from 0 to 1
			S = 0;
		} else { //Chromatic data...
			S = del_Max / var_Max;

			float del_R = ( ( ( var_Max - var_R ) / 6.0f ) + ( del_Max / 2.0f ) ) / del_Max;
			float del_G = ( ( ( var_Max - var_G ) / 6.0f ) + ( del_Max / 2.0f ) ) / del_Max;
			float del_B = ( ( ( var_Max - var_B ) / 6.0f ) + ( del_Max / 2.0f ) ) / del_Max;

			if      ( var_R == var_Max ) H = del_B - del_G;
			else if ( var_G == var_Max ) H = ( 1.0f / 3.0f ) + del_R - del_B;
			else if ( var_B == var_Max ) H = ( 2.0f / 3.0f ) + del_G - del_R;

			if ( H < 0.0f ) H += 1.0f;
			if ( H > 1.0f ) H -= 1.0f;
		}
		return HSVColor(H, S, V, A);
	}

	// http://www.easyrgb.com/index.php?X=MATH&H=21#text21
	/// @brief HSV to RGBA conversion
	static RGBAColor HSV2RGBA(HSVColor hsv) {
		uint8_t R = 0;
		uint8_t G = 0;
		uint8_t B = 0;
		uint8_t A = hsv.a * 255;
		if ( hsv.s == 0 ) { //HSV from 0 to 1
			R = hsv.v * 255;
			G = hsv.v * 255;
			B = hsv.v * 255;
		} else {
			float var_h = hsv.h * 6;
			if ( var_h >= 6.0f ) { var_h = 0; } //H must be < 1
			int var_i = int( var_h ); //Or ... var_i = floor( var_h )
			float var_1 = hsv.v * ( 1.0f - hsv.s );
			float var_2 = hsv.v * ( 1.0f - hsv.s * ( var_h - var_i ) );
			float var_3 = hsv.v * ( 1.0f - hsv.s * ( 1.0f - ( var_h - var_i ) ) );
			float var_r;
			float var_g;
			float var_b;
			if      ( var_i == 0 ) { var_r = hsv.v ; var_g = var_3 ; var_b = var_1 ; }
			else if ( var_i == 1 ) { var_r = var_2 ; var_g = hsv.v ; var_b = var_1 ; }
			else if ( var_i == 2 ) { var_r = var_1 ; var_g = hsv.v ; var_b = var_3 ; }
			else if ( var_i == 3 ) { var_r = var_1 ; var_g = var_2 ; var_b = hsv.v ; }
			else if ( var_i == 4 ) { var_r = var_3 ; var_g = var_1 ; var_b = hsv.v ; }
			else                   { var_r = hsv.v ; var_g = var_1 ; var_b = var_2 ; }

			R = var_r * 255; //RGB results from 0 to 255
			G = var_g * 255;
			B = var_b * 255;
		}
		return RGBAColor(R, G, B, A);
	}

	/// @brief Rotate RGBA color (use HSV)
	static RGBAColor rotate(RGBAColor rgba, float step) {
		HSVColor hsv = RGBA2HSV(rgba);
		hsv.h += step;
		if (hsv.h > 1.0f) { hsv.h -= 1.0f; }
		if (hsv.h < 0.0f) { hsv.h += 1.0f; }
		return HSV2RGBA(hsv);
	}

	/// @brief lerp from color to another color
	/// @param c1 first RGBAColor
	/// @param c2 second RGBAColor
	/// @param amount between 0 and 1
	/// @brief return RGBAColor lerped color
	static RGBAColor lerpColor(RGBAColor c1, RGBAColor c2, float amount) {
		if (amount < 0) { amount = 0; }
		if (amount > 1) { amount = 1; }

		uint8_t r = floor(c1.r + (c2.r-c1.r)*amount);
		uint8_t g = floor(c1.g + (c2.g-c1.g)*amount);
		uint8_t b = floor(c1.b + (c2.b-c1.b)*amount);
		uint8_t a = floor(c1.a + (c2.a-c1.a)*amount);

		return RGBAColor(r, g, b, a);
	}

	// https://stackoverflow.com/questions/28900598/how-to-combine-two-colors-with-varying-alpha-values
	// https://en.wikipedia.org/wiki/Alpha_compositing#Alpha_blending
	static RGBAColor alphaBlend(RGBAColor top, RGBAColor bottom) {
		// if we want to overlay top(0) over bottom(1) both with some alpha then:

		// uint8_t 0-255 to float 0.0-1.0
		float r0 = top.r / 255.0f;
		float g0 = top.g / 255.0f;
		float b0 = top.b / 255.0f;
		float a0 = top.a / 255.0f;

		float r1 = bottom.r / 255.0f;
		float g1 = bottom.g / 255.0f;
		float b1 = bottom.b / 255.0f;
		float a1 = bottom.a / 255.0f;

		// Note the division by a01 in the formulas for the components of color. It's important.
		float a01 =  (1 - a0) * a1 + a0;
		float r01 = ((1 - a0) * a1 * r1 + a0 * r0) / a01;
		float g01 = ((1 - a0) * a1 * g1 + a0 * g0) / a01;
		float b01 = ((1 - a0) * a1 * b1 + a0 * b0) / a01;

		uint8_t r = r01 * 255;
		uint8_t g = g01 * 255;
		uint8_t b = b01 * 255;
		uint8_t a = a01 * 255;

		return rt::RGBAColor(r, g, b, a);
	}

};

} // namespace rt

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

#endif /* COLOR_H_ */
