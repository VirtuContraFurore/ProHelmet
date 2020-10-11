/*
 * graphics.h
 *
 *  Created on: Sep 8, 2020
 *      Author: luca
 */

#ifndef DRIVER_GRAPHICS_H_
#define DRIVER_GRAPHICS_H_

#include <sys/types.h>

#include "oled.h"

/**
 * Colors are RGB_565
 */
typedef uint16_t Color_t;

#define Color_BLACK 		((Color_t) GR_ConvertColor565(0  ,  0,  0))
#define Color_WHITE			((Color_t) GR_ConvertColor565(255,255,255))
#define Color_RED			((Color_t) GR_ConvertColor565(255,  0,  0))
#define Color_GREEN			((Color_t) GR_ConvertColor565(  0,255,  0))
#define Color_BLUE			((Color_t) GR_ConvertColor565(  0,  0,255))
#define Color_ORANGE 		((Color_t) GR_ConvertColor565(255, 69,  0))
#define Color_YELLOW		((Color_t) GR_ConvertColor565(255,255,  0))
#define Color_LIGHT_BLUE 	((Color_t) GR_ConvertColor565(  0,191,255))
#define Color_PURPLE		((Color_t) GR_ConvertColor565(255,  0,255))

/*
 * Max digits used to display numbers
 */
#define GR_MAX_DIGITS	10

/*
 * FPS statistics
 */
#define SHOW_FPS
#define FPS_AVERAGE 50
#define FPS_COLOR	Color_RED

/*
 * Set period (in ms) for re-synching the memory transaction over spi
 */
#define RESYNCH_SPI_MEMWRITE_ms 2000

typedef struct Point{
	int16_t x;
	int16_t y;
} Point_t;

typedef struct Rect{
	Point_t pos;
	int16_t w;
	int16_t h;
} Rect_t;

/*
 * Bitmap: if alpha is an invalid pointer (zero) there is no alpha channel
 */
typedef struct Bitmap {
    const Color_t* bmp;
    const uint8_t* alpha;
    int16_t width;
    int16_t height;
} Bitmap_t;

/*
 * Font data
 */
typedef struct Character{
  const uint8_t *data;
  int16_t width, height;
} Character_t;

typedef struct Font {
    const struct Character *symbols;
    int16_t size;
} Font_t;

/*
 * Creates a 565 rbg color
 */
inline Color_t GR_ConvertColor565(uint8_t red, uint8_t green, uint8_t blue){
	return ((red & 0xF8) << 8) | ((green & 0xFC) << 3) | ((blue & 0xF8) >> 3);
}

/*
 * Converts back to RGB
 */
inline void GR_ConvertColorRGB(Color_t color, uint8_t rgb[]){
	rgb[0] = (color >> 8) & 0xF8;
	rgb[1] = (color >> 3) & 0xFC;
	rgb[2] = (color << 3) & 0xF8;
}

/*
 * Returns 1 if the point is on screen (and thus the corresponding pixel can be drawn)
 */
inline uint8_t GR_IsVisibile(OLED_data_t * screen, Point_t p){
	return (p.x >= 0) && (p.y >= 0) && (p.x < screen->width) && (p.y < screen->height);
}

/*
 * Blend colors with 8-bit alpha channel
 */
inline Color_t GR_Blend(Color_t foreground, Color_t background, uint8_t alpha){
	uint8_t fg[3], bg[3], rs[3];
	GR_ConvertColorRGB(foreground, fg);
	GR_ConvertColorRGB(background, bg);
	rs[0] = bg[0] + alpha * (fg[0] - bg[0]) / 256;
	rs[1] = bg[1] + alpha * (fg[1] - bg[1]) / 256;
	rs[2] = bg[2] + alpha * (fg[2] - bg[2]) / 256;
	return GR_ConvertColor565(rs[0], rs[1], rs[2]);
}


/*
 * Blend colors with 5-bit alpha channel
 */
inline Color_t GR_FastBlend(Color_t foreground, Color_t background, uint8_t alpha){
	uint32_t bg = background;
	uint32_t fg = foreground;
    alpha = ( alpha + 4 ) >> 3;
    bg = (bg | (bg << 16)) & 0b00000111111000001111100000011111;
    fg = (fg | (fg << 16)) & 0b00000111111000001111100000011111;
    uint32_t result = ((((fg - bg) * alpha) >> 5) + bg) & 0b00000111111000001111100000011111;
    return (uint16_t)((result >> 16) | result);
}

/*
 * Swap rendering buffers.
 * Embeds optional fps count
 */
void GR_SwapBuffers(OLED_data_t * screen);

/*
 * Draw a rectangle of 1 pixel width
 */
void GR_DrawRect(OLED_data_t * screen, Rect_t rect, Color_t color);

/*
 * Fill a rectangle
 */
void GR_FillRect(OLED_data_t * screen, Rect_t rect, Color_t color);

/*
 * Fill a rectangle of variable thickness
 */
void GR_DrawRect2(OLED_data_t * screen, Rect_t rect, uint8_t thickness, Color_t color);

/*
 * Draw a line
 */
void GR_DrawLine(OLED_data_t * screen, Point_t p1, Point_t p2, Color_t color);

/*
 * Draw a string using a font.
 */
void GR_DrawString(OLED_data_t * screen, Point_t pos, char* string, const struct Font* font, Color_t c);

/*
 * Draw a string using a font.
 */
void GR_DrawString_noWrap(OLED_data_t * screen, Point_t pos, char* string, const struct Font* font, Color_t c);

/*
 * Draw a number using a font.
 * digits is the number spacing
 */
void GR_DrawNumber(OLED_data_t * screen, Point_t pos, int32_t num, int32_t digits, const struct Font* font, Color_t c);

/*
 * Draw a char
 */
void GR_DrawChar(OLED_data_t * screen, Rect_t rect, const uint8_t * font, Color_t c);

/*
 * Draw a bmp
 */
void GR_DrawBitmap(OLED_data_t * screen, Point_t pos, struct Bitmap* bitmap);

#endif /* DRIVER_GRAPHICS_H_ */
