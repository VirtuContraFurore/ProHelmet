/*
 * oled.h
 *
 *  Created on: Sep 4, 2020
 *      Author: luca
 */

#ifndef DRIVER_OLED_H_
#define DRIVER_OLED_H_

#include <sys/types.h>

/*
 * 0.96 Inch OLED screen driver uses a 4 wire SPI and a PWM.
 *
 * SDA, SCL, CS, DC, RES, BLK.
 *  |    |   |   |    |    |
 * PA7  PA5 PA4 PA3  PB1  PA6
 *  G    R   G   W    P    Y
 *  r    e   r   h    i    e
 *  e    d   e   i    n    l
 *  e        y   t    k    l
 *  n            e         o
 *                         w
 *
 * SPI_1 Mapping: PA4 NSS, PA5 SCK, PA7 MOSI (PA6 MISO disabled / unused)
 *
 * Maybe a PWM on PA6 but first we need to check how the screen actually works
 *
 */

/*
 * Screen dimensions in pixels:
 */
#define OLED_WIDTH			160
#define OLED_HEIGHT			 80
#define OLED_PIXELS			(OLED_WIDTH*OLED_HEIGHT)
#define OLED_NORD_WIDTH		OLED_WIDTH
#define OLED_NORD_HEIGHT	OLED_HEIGHT

/*
 * Data transmission rate over SPI
 */
#define OLED_SPI_BR_45Mhz		0
#define OLED_SPI_BR_22_5Mhz		1
#define OLED_SPI_BR_11_25Mhz	2
#define OLED_SPI_BR_5_635Mhz	3
#define OLED_SPI_BR_2_813Mhz	4
#define OLED_SPI_BR_1_406Mhz	5

/*
 * Configure baudrate:
 * Note: logic analyzer needs a frequency below 5Mhz
 */
#define OLED_SPI_BR		OLED_SPI_BR_45Mhz

/*
 * Oled backlight control
 */
#define OLED_BACKLIGHT_MAX 		256
#define OLED_BACKLIGHT_MIN		  0
#define OLED_BACKLIGHT_OFF		  0

/*
 * Orientation of the screen.
 * NORD is horizontal with the cable in the left side.
 */
typedef enum {
	OLED_NORD = 0,
	OLED_EAST,
	OLED_SOUTH,
	OLED_WEST
} OLED_orientation_t;

#define OLED_ORIENTATION_MODES 4

/*
 * Possible pin out choices
 */
typedef enum {
	OLED_PINOUT_GPIOA // SDA-PA7, SCL-PA5, CS-PA4, DC-PA3, RES-PA11
} OLED_pinout_t;

/*
 * OLED data
 *
 * Note: the frame buffer contains ROWS
 */
typedef struct {
	uint8_t height;
	uint8_t width;
	int16_t backlight;
	OLED_orientation_t orientation;
//	OLED_pinout_t pinout;
	uint8_t rendering_framebuffer;
	uint16_t framebuffer[2][OLED_PIXELS];
} OLED_data_t;


// Exported functions:

/*
 * Initialize the OLED screen
 */
void OLED_InitScreen(OLED_data_t * data, OLED_orientation_t orientation);

/*
 * Change the orientation
 * */
void OLED_RotateScreen(OLED_data_t * data, OLED_orientation_t orientation);

/*
 * Write buffer to screen and swap frame buffers
 * Note: this blocks until the previous DMA / SPI transfer is complete
 */
void OLED_SwapBuffers(OLED_data_t * data);

/*
 * Clear a color
 */
void OLED_Clear(OLED_data_t * data, uint16_t color);

/*
 * Write pixel data to frame buffer.
 * Note: 0 <= x < width and 0 <= y < height
 */
inline void OLED_DrawPixel(OLED_data_t * screen, uint8_t x, uint8_t y, uint16_t color){
	screen->framebuffer[screen->rendering_framebuffer][x + y*screen->width] = color;
}

/*
 * Reads a pixel from the framebuffer (useful for blending)
 */
inline uint16_t OLED_ReadPixel(OLED_data_t * screen, uint8_t x, uint8_t y){
	return screen->framebuffer[screen->rendering_framebuffer][x + y*screen->width];
}

/*
 * Synch the frame writing phase
 */
inline void OLED_Resynch(OLED_data_t * screen){
	OLED_RotateScreen(screen, screen->orientation);
}

/*
 * Set the backlight, backlight must be inside the range max & min
 */
void OLED_SetBacklight(OLED_data_t * screen, int16_t backlight);


#endif /* DRIVER_OLED_H_ */
