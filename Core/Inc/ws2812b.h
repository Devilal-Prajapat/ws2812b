/*
 * ws2812b.h
 *
 *  Created on: Feb 12, 2025
 *      Author: devilalprajapat
 */

#ifndef INC_WS2812B_H_
#define INC_WS2812B_H_

#define rgb(a,b,c) {a,b,c}

#define COLOR_RED        rgb(210, 1, 3)
#define COLOR_BLUE       rgb(10, 61, 245)
#define COLOR_GREEN	     rgb(77, 245, 10)
#define COLOR_WHITE      rgb(255, 255, 255)

extern uint8_t colors[][3];

void reset_colors(void);
void set_pixel_color(uint8_t index, uint8_t r, uint8_t g, uint8_t b);
void update_leds(void);

void set_red_color(void);
void set_blue_color(void);
void set_green_color(void);

#endif /* INC_WS2812B_H_ */
