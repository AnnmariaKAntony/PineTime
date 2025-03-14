#ifndef DISPLAY_H
#define DISPLAY_H

#include <cstdint>

/// Initializes the display: configures SPI and control GPIOs, resets the display.
void display_init(void);

/// Clears the display by sending a clear command.
void display_clear(void);

/// Draws a string on the display (a simple implementation that sends the string via SPI).
void display_draw_string(const char* str);

#endif // DISPLAY_H
