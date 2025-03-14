#include "display.h"
#include <string.h>

/* Placeholder function to send a byte via SPI.
   You must implement the actual SPI transfer based on your MCU registers and wiring. */
static void spi_send_byte(uint8_t byte) {
    // TODO: Wait for SPI TX ready and send the byte
}

/* Send a buffer over SPI byte-by-byte */
static void spi_send_buffer(const uint8_t *buf, int len) {
    for (int i = 0; i < len; i++) {
        spi_send_byte(buf[i]);
    }
}

void display_init(void) {
    /* 
     * Initialize SPI peripheral and configure any required GPIOs
     * (chip-select, data/command, reset, etc.) based on your hardware setup.
     */
}

void display_clear(void) {
    /* 
     * Send a clear command sequence to the display.
     * For example, a Sharp Memory LCD might require a specific command.
     */
    // Example (dummy command): spi_send_byte(0x20);
}

void display_draw_string(const char *str) {
    /* 
     * In a complete implementation, you’d render the characters from the string into a framebuffer.
     * Here, we simply send the string bytes over SPI.
     */
    spi_send_buffer((const uint8_t *)str, strlen(str));
}
