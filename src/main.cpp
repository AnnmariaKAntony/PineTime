#include "St7789.h"
#include "time.h"
#include "nrf.h"         // Core nRF definitions
#include "nrf_rtc.h"     // RTC definitions for nRF52
#include "Spi.h"         // Your SPI interface header
#include <cstring>

// Simple busy-loop delay (not power-efficient)
static void delay(volatile uint32_t count) {
    while(count--) { __asm("nop"); }
}

// RTC0 Interrupt Handler.
// Make sure this function is linked in your interrupt vector table.
extern "C" void RTC0_IRQHandler(void) {
    if (NRF_RTC0->EVENTS_COMPARE[0]) {
        NRF_RTC0->EVENTS_COMPARE[0] = 0;  // Clear the event flag
        increment_time();                // Update time tracking
        NRF_RTC0->CC[0] += 1;             // Schedule the next tick
    }
}

int main(void) {
    // Instantiate the SPI interface.
    Pinetime::Drivers::Spi spi;

    // Define the pins for the ST7789 display.
    // Adjust these pin numbers as required by your hardware.
    uint8_t dataCommandPin = 18; // Data/Command control pin.
    uint8_t resetPin       = 26; // Reset pin.

    // Instantiate the ST7789 driver.
    // Pass spi (not &spi) since the constructor expects a reference.
    Pinetime::Drivers::St7789 display(spi, dataCommandPin, resetPin);

    // Initialize the display.
    display.Init();

    // Initialize timekeeping.
    time_init();
    rtc_init();

    while (1) {
        char time_str[16];
        get_time_string(time_str, sizeof(time_str));
        
        // For a minimal example, we do not render the text.
        // Instead, we fill a small rectangular area with a solid color (e.g., white)
        // as a placeholder for where the time would be displayed.
        constexpr uint16_t bufWidth  = 100;
        constexpr uint16_t bufHeight = 30;
        uint16_t buffer[bufWidth * bufHeight];
        
        // Fill the buffer with white (0xFFFF).
        for (uint32_t i = 0; i < bufWidth * bufHeight; i++) {
            buffer[i] = 0xFFFF;
        }
        
        // Position the rectangle roughly at the center of a 240x320 display.
        uint16_t x = (240 - bufWidth) / 2;
        uint16_t y = (320 - bufHeight) / 2;
        
        // Draw the rectangle to the display.
        display.DrawBuffer(x, y, bufWidth, bufHeight, reinterpret_cast<const uint8_t*>(buffer), sizeof(buffer));

        // Delay before the next update.
        delay(1000000);
    }
    
    return 0;
}
