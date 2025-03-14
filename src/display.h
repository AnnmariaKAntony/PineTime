#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdint.h>

/* Initialize the display (SPI and GPIOs) */
void display_init(void);

/* Clear the display (send a “clear screen” command) */
void display_clear(void);

/* Draw a string on the display.
   (A real implementation would convert the string into pixel data using a font table.) */
void display_draw_string(const char *str);

#endif // DISPLAY_H
