#include "display.h"
#include "time.h"

/* Simple busy-loop delay (not power-efficient) */
static void delay(volatile uint32_t count) {
    while(count--) { __asm("nop"); }
}

/* RTC0 Interrupt Handler.
 * This function must be linked in the interrupt vector table (typically via your startup file).
 */
void RTC0_IRQHandler(void) {
    if (RTC0->EVENTS_COMPARE[0]) {
        RTC0->EVENTS_COMPARE[0] = 0;  // Clear the event flag
        increment_time();             // Update our time structure
        RTC0->CC[0] += 1;             // Schedule the next 1-second tick
    }
}

int main(void) {
    /* Initialize display and timekeeping */
    display_init();
    time_init();
    rtc_init();

    while (1) {
        char time_str[16];
        get_time_string(time_str, sizeof(time_str));
        
        display_clear();
        display_draw_string(time_str);
        
        /* Delay to reduce redraw frequency */
        delay(1000000);
    }
    
    return 0;
}
