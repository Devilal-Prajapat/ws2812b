/*
 * ws2812b.c
 *
 *  Created on: Feb 12, 2025
 *      Author: devilalprajapat
 */
#include "main.h"
#include "ws2812b.h"

#define HIGH_PULSE   26
#define LOW_PULSE    13

#define LED_COUNT    1  // Number of LEDs
#define BIT_PER_LED  24
#define BUFFER_SIZE (LED_COUNT * BIT_PER_LED) + 50

#define rgb(a,b,c) {a,b,c}

extern TIM_HandleTypeDef htim3;

// TIM3 PWM DMA buffer
uint16_t pwm_buffer[BUFFER_SIZE] = {0};
uint8_t datasentflag = 0;


void set_pixel_color(uint8_t index, uint8_t r, uint8_t g, uint8_t b) {
	uint32_t color = (g << 16) | (r << 8) | b;  // WS2812 expects GRB format
	for (int i = 0; i < 24; i++) {
		if (color & (1 << (23 - i))) {
			pwm_buffer[index * 24 + i] = HIGH_PULSE; // High pulse (T1H)
		} else {
			pwm_buffer[index * 24 + i] = LOW_PULSE; // Low pulse (T0H)
		}
	}
}

void update_leds(void)
{
	HAL_TIM_PWM_Start_DMA(&htim3, TIM_CHANNEL_1, (uint32_t*) pwm_buffer, BUFFER_SIZE);
	while (!datasentflag) {
	};
	datasentflag = 0;
}

void WS2812_Send(uint8_t r, uint8_t g, uint8_t b){
	uint32_t indx = 0;
	uint32_t color = (g << 16) | (r << 8) | b;  // WS2812 expects GRB format
	for (int i = 0; i < LED_COUNT; i++) {
		for (int i = 23; i >= 0; i--) {
			if (color & (1 << i)) {
				pwm_buffer[indx] = HIGH_PULSE;  // 2/3 of 90
			} else {
				pwm_buffer[indx] = LOW_PULSE;  // 1/3 of 90
			}
			indx++;
		}
	}
	for (int i = 0; i < 50; i++) {
		pwm_buffer[indx] = 0;
		indx++;
	}
	HAL_TIM_PWM_Start_DMA(&htim3, TIM_CHANNEL_1, (uint32_t*) pwm_buffer, indx);
	while (!datasentflag) {
	};
	datasentflag = 0;
}

void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim) {
	HAL_TIM_PWM_Stop_DMA(&htim3, TIM_CHANNEL_1);
	datasentflag = 1;
}

void reset_colors()
{
	for (uint8_t i = 0; i<LED_COUNT;i++){
		set_pixel_color(i,0,0,0);
	}
	update_leds();
}

void set_red_color(void)
{
	reset_colors();
	for (uint8_t i = 0; i<LED_COUNT;i++){
		uint8_t color[] = COLOR_RED;
		set_pixel_color(i,color[0],color[1],color[2]);
	}
	update_leds();
}

void set_blue_color(void)
{
	 reset_colors();
	for (uint8_t i = 0; i<LED_COUNT;i++){
		uint8_t color[] = COLOR_BLUE;
		set_pixel_color(i,color[0],color[1],color[2]);
	}
	update_leds();
}

void set_green_color(void)
{
	reset_colors();
	for (uint8_t i = 0; i<LED_COUNT;i++){
		uint8_t color[] = COLOR_GREEN;
		set_pixel_color(i,color[0],color[1],color[2]);
	}
	update_leds();
}

#if 0
// max 1.25 us  for 40 value
uint8_t colors[][3] = { rgb(183, 39, 39), rgb(183, 181, 39), rgb(92, 183, 39),\
		rgb(39, 171, 183), rgb(39, 82, 183), rgb(164, 39, 183), rgb(183, 39,\
				128), rgb(183, 39, 61), rgb(249, 240, 242), rgb(6, 2, 112), rgb(\
				255, 222, 89), };

void delay_us(uint32_t us) {
	TIM3->CNT = 0;
	while (TIM3->CNT < us)
		;
}

void ws2812_send_bit(uint8_t bit) {
	if (bit) {
		WS2812_PORT->BSRR = WS2812_PIN;   // Set pin HIGH
		delay_us(26);                 // ~0.8µs (T1H)
		WS2812_PORT->BSRR = (WS2812_PIN << 16); // Set pin LOW
		delay_us(13);                 // ~0.45µs (T1L)
	} else {
		WS2812_PORT->BSRR = WS2812_PIN;   // Set pin HIGH
		delay_us(13);                 // ~0.4µs (T0H)
		WS2812_PORT->BSRR = (WS2812_PIN << 16); // Set pin LOW
		delay_us(26);                 // ~0.85µs (T0L)
	}
}

void reset() {
	WS2812_PORT->BSRR = WS2812_PIN;
	delay_us(2);
	WS2812_PORT->BSRR = (WS2812_PIN << 16);
	for (int i = 0; i < 50; i++) {
		delay_us(39);
	}
	WS2812_PORT->BSRR = WS2812_PIN;
}

void ws2812_send_byte(uint8_t byte) {
	for (int i = 0; i < 8; i++) {
		ws2812_send_bit(byte & (1 << (7 - i)));
	}
}

void ws2812_send_color(uint8_t r, uint8_t g, uint8_t b) {
	ws2812_send_byte(g);  // WS2812 expects GRB format
	ws2812_send_byte(r);
	ws2812_send_byte(b);
}

void ws2812_show(uint8_t colors[3], int led_count) {
	__disable_irq(); // Disable interrupts to ensure timing accuracy
	for (int i = 0; i < led_count; i++) {
		ws2812_send_color(colors[0], colors[1], colors[2]);
	}
	__enable_irq(); // Re-enable interrupts
	reset();
	HAL_Delay(1); // Reset pulse (>50µs)
}
#endif
