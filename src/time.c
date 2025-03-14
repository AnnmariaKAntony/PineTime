#include "time.h"
#include <stdio.h>

/* Global time structure */
volatile struct time_struct current_time;

void time_init(void) {
    current_time.hours = 0;
    current_time.minutes = 0;
    current_time.seconds = 0;
}

void increment_time(void) {
    current_time.seconds++;
    if (current_time.seconds >= 60) {
        current_time.seconds = 0;
        current_time.minutes++;
        if (current_time.minutes >= 60) {
            current_time.minutes = 0;
            current_time.hours++;
            if (current_time.hours >= 24) {
                current_time.hours = 0;
            }
        }
    }
}

void get_time_string(char *buf, int len) {
    snprintf(buf, len, "%02d:%02d:%02d", current_time.hours, current_time.minutes, current_time.seconds);
}

/* Minimal NVIC helper to enable interrupts */
#define NVIC_ISER ((volatile uint32_t*)0xE000E100)
static void NVIC_EnableIRQ(int irq) {
    NVIC_ISER[irq >> 5] = (1 << (irq & 0x1F));
}

void rtc_init(void) {
    /* Stop and clear the RTC */
    RTC0->TASKS_STOP = 1;
    RTC0->TASKS_CLEAR = 1;
    
    /* Set prescaler to 32767 so that the RTC runs at 1Hz */
    RTC0->PRESCALER = 32767;
    
    /* Set compare register 0 for the first tick (after 1 second) */
    RTC0->CC[0] = 1;
    
    /* Clear any pending compare event */
    RTC0->EVENTS_COMPARE[0] = 0;
    
    /* Enable interrupt for compare register 0.
       (For nRF52, the bit for COMPARE[0] is at position 16.) */
    RTC0->INTENSET = (1 << 16);
    
    /* Enable the RTC0 interrupt in the NVIC (IRQ number for RTC0 is 2) */
    NVIC_EnableIRQ(2);
    
    /* Start the RTC */
    RTC0->TASKS_START = 1;
}
