/*
 * ScrollText.h
 *
 *  Created on: Sep 28, 2020
 *      Author: luca
 */

#ifndef WIDGETS_SCROLLTEXT_H_
#define WIDGETS_SCROLLTEXT_H_

#include "../driver/oled.h"
#include "../driver/graphics.h"

#define SCROLLTEXT_MAX_STRINGS 			20
#define SCROLLTEXT_MAX_STRING_LENGHT 	30

/*
 * ScrollText area
 */
typedef struct ScrollText_data {
	Rect_t pos;
	int16_t interline_spacing;
	uint16_t str_count;
	uint8_t strings[SCROLLTEXT_MAX_STRINGS][SCROLLTEXT_MAX_STRING_LENGHT+1];
} ScrollText_t;


void ScrollText_draw(OLED_data_t * screen, ScrollText_t * scroll);

void ScrollText_addString(ScrollText_t * scroll, uint8_t * str);


#endif /* WIDGETS_SCROLLTEXT_H_ */
