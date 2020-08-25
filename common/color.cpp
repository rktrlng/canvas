#include <common/color.h>

namespace rt {

// ###############################################
// RGBA Color
// ###############################################
RGBAColor::RGBAColor() :
	r(255), g(255), b(255), a(255) { }

RGBAColor::RGBAColor(uint8_t value) :
	r(value), g(value), b(value), a(255) { }

RGBAColor::RGBAColor(uint8_t value, uint8_t alpha) :
	r(value), g(value), b(value), a(alpha) { }

RGBAColor::RGBAColor(uint8_t red, uint8_t green, uint8_t blue) :
	r(red), g(green), b(blue), a(255) { }

RGBAColor::RGBAColor(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha) :
	r(red), g(green), b(blue), a(alpha) { }

RGBAColor::RGBAColor(uint32_t color) {
	r = color >> 24 & 0xFF;
	g = color >> 16 & 0xFF;
	b = color >> 8 & 0xFF;
	a = color & 0xFF;
}
uint32_t RGBAColor::asInt() {
	uint32_t color = (r << 24) + (g << 16) + (b << 8) + (a);
	return color;
}



// ###############################################
// HSV Color
// ###############################################
HSVAColor::HSVAColor() :
	h(0.0f), s(0.0f), v(1.0f), a(1.0f) { }
HSVAColor::HSVAColor(float hue, float sat, float val, float alpha) :
	h(hue), s(sat), v(val), a(alpha) { }


// ###############################################
// Color
// ###############################################
// http://www.easyrgb.com/index.php?X=MATH&H=20#text20
/// @brief RGBA to HSV conversion
HSVAColor Color::RGBA2HSVA(RGBAColor rgba) {
	float var_R = (float) rgba.r / 255; //RGB from 0 to 255
	float var_G = (float) rgba.g / 255;
	float var_B = (float) rgba.b / 255;

	float var_Min = std::min( std::min( var_R, var_G), var_B ); // Min. value of RGB
	float var_Max = std::max( std::max( var_R, var_G), var_B ); // Max. value of RGB
	float del_Max = var_Max - var_Min; // Delta RGB value

	float H = 0.0f;
	float S = 0.0f;
	float V = var_Max;
	float A = (float) rgba.a / 255;

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
	return HSVAColor(H, S, V, A);
}

RGBAColor Color::HSVA2RGBA(HSVAColor hsva) {
	uint8_t R = 0;
	uint8_t G = 0;
	uint8_t B = 0;
	uint8_t A = hsva.a * 255;
	if ( hsva.s == 0 ) { //HSV from 0 to 1
		R = hsva.v * 255;
		G = hsva.v * 255;
		B = hsva.v * 255;
	} else {
		float var_h = hsva.h * 6;
		if ( var_h >= 6.0f ) { var_h = 0; } //H must be < 1
		int var_i = int( var_h ); //Or ... var_i = floor( var_h )
		float var_1 = hsva.v * ( 1.0f - hsva.s );
		float var_2 = hsva.v * ( 1.0f - hsva.s * ( var_h - var_i ) );
		float var_3 = hsva.v * ( 1.0f - hsva.s * ( 1.0f - ( var_h - var_i ) ) );
		float var_r;
		float var_g;
		float var_b;
		if      ( var_i == 0 ) { var_r = hsva.v ; var_g = var_3  ; var_b = var_1  ; }
		else if ( var_i == 1 ) { var_r = var_2  ; var_g = hsva.v ; var_b = var_1  ; }
		else if ( var_i == 2 ) { var_r = var_1  ; var_g = hsva.v ; var_b = var_3  ; }
		else if ( var_i == 3 ) { var_r = var_1  ; var_g = var_2  ; var_b = hsva.v ; }
		else if ( var_i == 4 ) { var_r = var_3  ; var_g = var_1  ; var_b = hsva.v ; }
		else                   { var_r = hsva.v ; var_g = var_1  ; var_b = var_2  ; }

		R = var_r * 255; //RGB results from 0 to 255
		G = var_g * 255;
		B = var_b * 255;
	}
	return RGBAColor(R, G, B, A);
}

// https://stackoverflow.com/questions/28900598/how-to-combine-two-colors-with-varying-alpha-values
// https://stackoverflow.com/questions/2030471/alpha-blending-a-red-blue-and-green-image-to-produce-an-image-tinted-to-any-rg/2030560#2030560
// https://en.wikipedia.org/wiki/Alpha_compositing#Alpha_blending
RGBAColor Color::alphaBlend(RGBAColor a, RGBAColor b) {
	RGBAColor out;
	/// TODO implement

	return out;
}

RGBAColor Color::rotate(RGBAColor rgba, float step) {
	HSVAColor hsva = RGBA2HSVA(rgba);
	hsva.h += step;
	if (hsva.h > 1.0f) { hsva.h -= 1.0f; }
	if (hsva.h < 0.0f) { hsva.h += 1.0f; }
	return HSVA2RGBA(hsva);
}

RGBAColor Color::lerpColor(RGBAColor c1, RGBAColor c2, float amount) {
	if (amount < 0) { amount = 0; }
	if (amount > 1) { amount = 1; }

	uint8_t r = floor(c1.r + (c2.r-c1.r)*amount);
	uint8_t g = floor(c1.g + (c2.g-c1.g)*amount);
	uint8_t b = floor(c1.b + (c2.b-c1.b)*amount);
	uint8_t a = floor(c1.a + (c2.a-c1.a)*amount);

	return RGBAColor(r, g, b, a);
}


} // namespace rt

