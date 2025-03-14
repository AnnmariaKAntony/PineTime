#include "display.h"
#include "time.h"
#include "nrf.h"         // Core nRF definitions
#include "nrf_rtc.h"     // RTC definitions for nRF52

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
    // Initialize display and timekeeping.
    display_init();
    time_init();
    rtc_init();

    while (1) {
        char time_str[16];
        get_time_string(time_str, sizeof(time_str));
        
        display_clear();
        display_draw_string(time_str);
        
        // Delay to reduce redraw frequency.
        delay(1000000);
    }
    
    return 0;
}
