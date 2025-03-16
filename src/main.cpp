#include "St7789.h"
// #include "Spi.h"
#include "SpiMaster.h"
#include "time.h"
#include "nrf.h"         // Core nRF definitions
#include "nrf_rtc.h"     // RTC definitions for nRF52
#include <cstring>
#include "components/Gfx.h"

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
  // Setup SPI Master parameters.
  Pinetime::Drivers::SpiMaster::Parameters spiParams;
  spiParams.bitOrder = Pinetime::Drivers::SpiMaster::BitOrder::Msb_Lsb;
  spiParams.mode = Pinetime::Drivers::SpiMaster::Modes::Mode0;
  spiParams.Frequency = Pinetime::Drivers::SpiMaster::Frequencies::Freq8Mhz;
  spiParams.pinSCK   = 2;
  spiParams.pinMOSI  = 3;
  spiParams.pinMISO  = 4;
  spiParams.pinCSN   = 25;

  // Instantiate and initialize the SPI Master.
  Pinetime::Drivers::SpiMaster spiMaster(Pinetime::Drivers::SpiMaster::SpiModule::SPI0, spiParams);
  spiMaster.Init();

  // Instantiate the SPI interface with the SPI Master and CS pin.
//   Pinetime::Drivers::Spi spi(spiMaster, spiParams.pinCSN);
//   spi.Init();

  // Define the pin for the ST7789 display.
  // (Only the Data/Command pin is now needed.)
  uint8_t dataCommandPin = 18;

  // Instantiate the ST7789 driver.
  Pinetime::Drivers::St7789 display(spiMaster, dataCommandPin);
  display.Init();

  // Optionally, initialize timekeeping.
  // time_init();
  // rtc_init();

  while (1) {
    // Prepare a rectangular buffer to fill with a solid white color.
    // constexpr uint16_t bufWidth  = 100;
    // constexpr uint16_t bufHeight = 30;
    // uint16_t buffer[bufWidth * bufHeight];

    // // Fill the buffer with white (0xFFFF).
    // for (uint32_t i = 0; i < bufWidth * bufHeight; i++) {
    //   buffer[i] = 0xFFFF;
    // }

    // // Calculate the starting coordinates (centering the rectangle on a 240x320 display).
    // uint16_t x = (240 - bufWidth) / 2;
    // uint16_t y = (320 - bufHeight) / 2;

    // // Set the drawing area using the new BeginDrawBuffer() function.
    // display.BeginDrawBuffer(x, y, bufWidth, bufHeight);
    // // Send the pixel data using NextDrawBuffer().
    // display.NextDrawBuffer(reinterpret_cast<const uint8_t*>(buffer), sizeof(buffer));

    // // Delay before updating again.
    gfx.FillRectangle(0,0,240,240,0xffff);
    delay(1000000);
  }

  return 0;
}
