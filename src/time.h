#ifndef TIME_H
#define TIME_H

#include <stdint.h>

/* Simple structure to hold the current time */
struct time_struct {
    uint8_t hours;
    uint8_t minutes;
    uint8_t seconds;
};

extern volatile struct time_struct current_time;

/* Initialize the software clock (sets the time to 00:00:00) */
void time_init(void);

/* Increment the time by one second (called by the RTC interrupt) */
void increment_time(void);

/* Format the current time as a string "HH:MM:SS" */
void get_time_string(char *buf, int len);

/* Initialize the RTC peripheral to generate a 1Hz tick */
void rtc_init(void);

/* Minimal RTC register definitions for the nRF52 */
typedef struct {
    volatile uint32_t TASKS_START;
    volatile uint32_t TASKS_STOP;
    volatile uint32_t TASKS_CLEAR;
    uint32_t RESERVED0[1];
    volatile uint32_t TASKS_TRIGOVRFLW;
    uint32_t RESERVED1[56];
    volatile uint32_t EVENTS_TICK;
    uint32_t RESERVED2[1];
    volatile uint32_t EVENTS_OVRFLW;
    uint32_t RESERVED3[12];
    volatile uint32_t EVENTS_COMPARE[4];
    uint32_t RESERVED4[172];
    volatile uint32_t INTENSET;
    volatile uint32_t INTENCLR;
    uint32_t RESERVED5[61];
    volatile uint32_t COUNTER;
    volatile uint32_t PRESCALER;
    uint32_t RESERVED6[1];
    volatile uint32_t CC[4];
} RTC_Type;

#define RTC0_BASE 0x4000B000UL
#define RTC0 ((RTC_Type *) RTC0_BASE)

#endif // TIME_H