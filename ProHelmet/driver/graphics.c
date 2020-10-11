/*
 * graphics.c
 *
 *  Created on: Sep 8, 2020
 *      Author: luca
 */

#include "graphics.h"
#include "../system.h"

#ifdef SHOW_FPS
#include "../assets/fonts/MonoTypewriter12.h"
#endif /* SHOW_FPS */

#define ABS(x)	((x) > 0) ? (x) : (-(x))

void GR_DrawRect(OLED_data_t * screen, Rect_t rect, Color_t color){
	/* Check if is visible */
	if(rect.pos.x >= screen->width || rect.pos.y >= screen->height ||
			rect.pos.x + rect.w < 0 || rect.pos.y + rect.h < 0)
		return;

	/* Clipping if needed */

	uint8_t mask = 0b0000; //R L B T

	// Left edge
	if(rect.pos.x >= 0)
		mask |= 0b0100;
	// Right edge
	if(rect.pos.x + rect.w <= screen->width)
		mask |= 0b1000;
	// Top edge
	if(rect.pos.y >= 0)
		mask |= 0b0001;
	// Bottom edge
	if(rect.pos.y + rect.h <= screen->height)
		mask |= 0b0010;


	/* Clipping if needed */
	if(rect.pos.x < 0) {
		rect.w += rect.pos.x;
		rect.pos.x = 0;
	}

	if(rect.pos.y < 0) {
		rect.h += rect.pos.y;
		rect.pos.y = 0;
	}

	if(rect.pos.x + rect.w > screen->width)
		rect.w = screen->width - rect.pos.x;

	if(rect.pos.y + rect.h > screen->height)
		rect.h = screen->height - rect.pos.y;

	Point_t p = rect.pos;
	for(; p.x < rect.pos.x + rect.w; p.x++){
			if(mask & 0b0001)
				OLED_DrawPixel(screen, p.x, p.y, color);
			if((mask & 0b0010) && rect.h > 0)
				OLED_DrawPixel(screen, p.x, p.y + rect.h - 1, color);
	}

	p.x = rect.pos.x;
	for(; p.y < rect.pos.y + rect.h; p.y++){
			if(mask & 0b0100)
				OLED_DrawPixel(screen, p.x, p.y, color);
			if((mask & 0b1000) && rect.w > 0)
				OLED_DrawPixel(screen, p.x + rect.w - 1, p.y, color);
	}
}


void GR_FillRect(OLED_data_t * screen, Rect_t rect, Color_t color){
	/* Check if is visible */
	if(rect.pos.x > screen->width || rect.pos.y > screen->height ||
			rect.pos.x + rect.w < 0 || rect.pos.y + rect.h < 0)
		return;

	/* Clipping if needed */
	if(rect.pos.x < 0) {
		rect.w += rect.pos.x;
		rect.pos.x = 0;
	}

	if(rect.pos.y < 0) {
		rect.h += rect.pos.y;
		rect.pos.y = 0;
	}

	if(rect.pos.x + rect.w > screen->width)
		rect.w = screen->width - rect.pos.x;

	if(rect.pos.y + rect.h > screen->height)
		rect.h = screen->height - rect.pos.y;

	Point_t p = rect.pos;
	for(; p.x < rect.pos.x + rect.w; p.x++){
		for(; p.y < rect.pos.y + rect.h; p.y++)
			OLED_DrawPixel(screen, p.x, p.y, color);
		p.y = rect.pos.y; // reset y for the next iteration over x
	}
}

void GR_DrawRect2(OLED_data_t * screen, Rect_t rect, uint8_t thickness, Color_t color){
	GR_DrawRect(screen, rect, color);
	while(--thickness && (rect.w > 0) && (rect.h > 0)){
		rect.pos.x++;
		rect.pos.y++;
		rect.w+=-2;
		rect.h+=-2;
		GR_DrawRect(screen, rect, color);
	}
}

void GR_DrawLine(OLED_data_t * screen, Point_t p1, Point_t p2, Color_t color){
	int32_t deltax = 0, deltay = 0, x = 0, y = 0, xinc1 = 0, xinc2 = 0,
	yinc1 = 0, yinc2 = 0, den = 0, num = 0, numadd = 0, numpixels = 0,
	curpixel = 0;

	deltax = ABS(p2.x - p1.x);        /* The difference between the x's */
	deltay = ABS(p2.y - p1.y);        /* The difference between the y's */
	x = p1.x;                       /* Start x off at the first pixel */
	y = p1.y;                       /* Start y off at the first pixel */

	if (p2.x >= p1.x)                 /* The x-values are increasing */
	{
	  xinc1 = 1;
	  xinc2 = 1;
	}
	else                          /* The x-values are decreasing */
	{
	  xinc1 = -1;
	  xinc2 = -1;
	}

	if (p2.y >= p1.y)                 /* The y-values are increasing */
	{
	  yinc1 = 1;
	  yinc2 = 1;
	}
	else                          /* The y-values are decreasing */
	{
	  yinc1 = -1;
	  yinc2 = -1;
	}

	if (deltax >= deltay)         /* There is at least one x-value for every y-value */
	{
	  xinc1 = 0;                  /* Don't change the x when numerator >= denominator */
	  yinc2 = 0;                  /* Don't change the y for every iteration */
	  den = deltax;
	  num = deltax / 2;
	  numadd = deltay;
	  numpixels = deltax;         /* There are more x-values than y-values */
	}
	else                          /* There is at least one y-value for every x-value */
	{
	  xinc2 = 0;                  /* Don't change the x for every iteration */
	  yinc1 = 0;                  /* Don't change the y when numerator >= denominator */
	  den = deltay;
	  num = deltay / 2;
	  numadd = deltax;
	  numpixels = deltay;         /* There are more y-values than x-values */
	}

	for (curpixel = 0; curpixel <= numpixels; curpixel++)
	{
	  if(GR_IsVisibile(screen, (Point_t) {x, y}))
		  OLED_DrawPixel(screen, x, y, color);   /* Draw the current pixel */
	  num += numadd;              /* Increase the numerator by the top of the fraction */
	  if (num >= den)             /* Check if numerator >= denominator */
	  {
	    num -= den;               /* Calculate the new numerator value */
	    x += xinc1;               /* Change the x as appropriate */
	    y += yinc1;               /* Change the y as appropriate */
	  }
	  x += xinc2;                 /* Change the x as appropriate */
	  y += yinc2;                 /* Change the y as appropriate */
	}
}

void GR_DrawString(OLED_data_t * screen, Point_t pos, char* string, const struct Font* font, Color_t c){
	int orig = pos.x;
	int i = 0;
    while(string[i] != '\0'){

    	/* Manage space char */
        if(string[i] == ' '){
        	pos.x +=font->symbols[(int)'a'-33].width; /* Use the same width of 'a' character. */
        	i++;
        	continue;
        }

        int index = (int)string[i] - 33;

        if(pos.x + font->symbols[index].width >= screen->width){
        	pos.y+= font->size;
        	if(pos.y + font->symbols[index].height >= screen->height)
        		return;
        	pos.x = orig;
        }

        Rect_t rect = {.pos=pos, .w = font->symbols[index].width, .h = font->symbols[index].height};

        GR_DrawChar(screen, rect, font->symbols[index].data, c);

        pos.x += rect.w;
        i++;
    }
}

void GR_DrawString_noWrap(OLED_data_t * screen, Point_t pos, char* string, const struct Font* font, Color_t c){
	int orig = pos.x;
	int i = 0;
    while(string[i] != '\0'){

    	/* Manage space char */
        if(string[i] == ' '){
        	pos.x +=font->symbols[(int)'a'-33].width; /* Use the same width of 'a' character. */
        	i++;
        	continue;
        }

        int index = (int)string[i] - 33;

        Rect_t rect = {.pos=pos, .w = font->symbols[index].width, .h = font->symbols[index].height};

        GR_DrawChar(screen, rect, font->symbols[index].data, c);

        pos.x += rect.w;
        i++;
    }
}

void GR_DrawChar(OLED_data_t * screen, Rect_t rect, const uint8_t * font, Color_t c){
	for(int y = 0; y < rect.h; y++){
		for(int x = 0; x < rect.w; x++){
			if(!GR_IsVisibile(screen, (Point_t) {x+rect.pos.x, y+rect.pos.y}))
				continue;

			uint8_t alpha = font[y * rect.w + x];
			if(alpha == 0)
				continue;
			Color_t color = GR_Blend(c, OLED_ReadPixel(screen, x+rect.pos.x, y+rect.pos.y), alpha);
			OLED_DrawPixel(screen, x+rect.pos.x, y+rect.pos.y, color);
		}
	}
}

void GR_DrawBitmap(OLED_data_t * screen, Point_t pos, struct Bitmap* bitmap){
	const uint16_t* arr = bitmap->bmp;
	const uint8_t* alpha = bitmap->alpha;
	int index = 0;
	int mask = 0x0F;

	for(int y = pos.y; y < pos.y+bitmap->height; y++)
		for(int x = pos.x; x < pos.x+bitmap->width; x++){

			if(GR_IsVisibile(screen, (Point_t) {x, y}) &&
					(alpha==0 || (alpha[index] & mask))) {

				uint8_t a = alpha[index] & mask;
				if(mask == 0x0F)
					a <<= 4;

				Color_t fg = arr[(x-pos.x) + (y-pos.y)*bitmap->width];
				Color_t c;
				if(a == 0xF0)
					c = fg;
				else
					c = GR_FastBlend(fg, OLED_ReadPixel(screen, x, y), a);

				OLED_DrawPixel(screen, x, y, c);
			}

			if(mask == 0x0F)
				mask = 0xF0;
			else {
				mask = 0x0F;
				index++;
			}

		}
}

/*
 * Draw a string using a font.
 */
void GR_DrawNumber(OLED_data_t * screen, Point_t pos, int32_t num, int32_t digits, const struct Font* font, Color_t c){
	if(digits > GR_MAX_DIGITS || digits < 1)
		digits = GR_MAX_DIGITS;

	uint8_t tmp[GR_MAX_DIGITS], string[GR_MAX_DIGITS+2];
	int32_t i;

	int8_t negative = 0;

	if(num < 0){
		negative = 1;
		num = -num;
	}

	for(i = 0; i < GR_MAX_DIGITS; i++){
		tmp[i] = num % 10;
		num /= 10;
		if(num == 0){
			i++;
			break;
		}
	}

	/* Fill spaces */
	int32_t space = digits - i;
	int32_t idx = 0;

	for(; idx < space-1; idx++)
		string[idx] = ' ';

	if(idx < space)
		if(negative)
			string[idx++] = '-';
		else
			string[idx++] = ' ';

	for(; idx < GR_MAX_DIGITS && i > 0; idx++){
		i--;
		string[idx] = '0' + tmp[i];
	}

	string[idx] = 0;

	GR_DrawString(screen, pos, string, font, c);
}

#ifdef SHOW_FPS
static int32_t fps_count = FPS_AVERAGE;
static int32_t fps = 0;
static time_ms_t fps_past_time = 0;
#endif /* SHOW FPS */

#ifdef RESYNCH_SPI_MEMWRITE_ms
static time_ms_t synch_past_time = 0;
#endif /* RESYNCH_SPI_MEMWRITE_ms */

void GR_SwapBuffers(OLED_data_t * screen){

	/* Show FPS count */
#ifdef SHOW_FPS
	if(fps_count == 0){
		time_ms_t curr_time = SYS_time_millis;
		time_ms_t diff = curr_time - fps_past_time;
		fps_past_time = curr_time;
		fps = 1000 * FPS_AVERAGE / diff;
		fps_count = FPS_AVERAGE;
	}
	fps_count--;
	GR_DrawString(screen, (Point_t) {0, 0}, "fps:" , &font_MonoTypewriter12px, Color_RED);
	GR_DrawNumber(screen, (Point_t) {font_MonoTypewriter12px.symbols[0].width*4, 0}, fps, 3, &font_MonoTypewriter12px, FPS_COLOR);
#endif /* SHOW_FPS */

	/* Re-synch screen */
#ifdef RESYNCH_SPI_MEMWRITE_ms
	time_ms_t curr_time = SYS_time_millis;
	time_ms_t diff = curr_time - synch_past_time;
	if(diff > RESYNCH_SPI_MEMWRITE_ms){
		synch_past_time = curr_time;
		OLED_Resynch(screen);
	}
#endif /* RESYNCH_SPI_MEMWRITE_ms */

	OLED_SwapBuffers(screen);
}
