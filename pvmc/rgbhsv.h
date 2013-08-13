/*
 * RGB <-> HSV defs. : http://www.acm.org/jgt/papers/SmithLyons96/hsv_rgb.html
 *
 * Wed Jan 12 17:10:21 2005
*/

/*********************************************************************************/

#define RGBHSV_UNDEFINED -1

/*********************************************************************************/
	
// Theoretically, hue 0 (pure red) is identical to hue 6 in these transforms. Pure

// red always maps to 6 in this implementation. Therefore UNDEFINED can be

// defined as 0 in situations where only unsigned numbers are desired.

typedef struct {float R, G, B;} RGBType;

typedef struct {float H, S, V;} HSVType;

/*********************************************************************************/

// conversion function calls themselves

HSVType RGB_to_HSV( RGBType RGB );
RGBType HSV_to_RGB( HSVType HSV );

/*********************************************************************************/
