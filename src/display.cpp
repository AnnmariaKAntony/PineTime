#include "display.h"
#include <string.h>
#include "nrf_gpio.h"
#include "nrf_spim.h"
#include "nrf_delay.h"

//--------------------------------------------------------------------------
// Pin Definitions (adjust these as needed)
//--------------------------------------------------------------------------

// SPI pins
static constexpr uint8_t LCD_SPI_SCK    = 2;  // SPI clock
static constexpr uint8_t LCD_SPI_MOSI   = 3;  // SPI MOSI
static constexpr uint8_t LCD_SPI_MISO   = 4;  // SPI MISO (unused in this example)

// Display control pins
static constexpr uint8_t LCD_CS_PIN     = 25; // Chip Select for LCD
static constexpr uint8_t LCD_DC_PIN     = 18; // Data/Command control
static constexpr uint8_t LCD_RESET_PIN  = 26; // Reset pin

// Use SPIM0 for SPI transfers.
#define SPI_INSTANCE NRF_SPIM0

//--------------------------------------------------------------------------
// Low-Level SPI Functions
//--------------------------------------------------------------------------

// Configure the SPI peripheral (SPIM0) for the display.
static void spi_init(void) {
  // Configure SPI GPIO pins.
  nrf_gpio_cfg_output(LCD_SPI_SCK);
  nrf_gpio_cfg_output(LCD_SPI_MOSI);
  nrf_gpio_cfg_input(LCD_SPI_MISO, NRF_GPIO_PIN_NOPULL);

  // Assign pins to the SPIM0 peripheral.
  SPI_INSTANCE->PSELSCK  = LCD_SPI_SCK;
  SPI_INSTANCE->PSELMOSI = LCD_SPI_MOSI;
  SPI_INSTANCE->PSELMISO = LCD_SPI_MISO;

  // Set SPI frequency to 8MHz (adjust if needed).
  SPI_INSTANCE->FREQUENCY = SPIM_FREQUENCY_FREQUENCY_M8;

  // Configure SPI: Mode 0 (CPOL = 0, CPHA = 0), MSB first.
  SPI_INSTANCE->CONFIG = (SPIM_CONFIG_ORDER_MsbFirst << SPIM_CONFIG_ORDER_Pos) |
                         (SPIM_CONFIG_CPHA_Leading   << SPIM_CONFIG_CPHA_Pos) |
                         (SPIM_CONFIG_CPOL_ActiveHigh << SPIM_CONFIG_CPOL_Pos);

  // Enable the SPI peripheral.
  SPI_INSTANCE->ENABLE = SPIM_ENABLE_ENABLE_Enabled << SPIM_ENABLE_ENABLE_Pos;
}

// Send a single byte over SPI (blocking).
static void spi_send_byte(uint8_t byte) {
  // Use a static variable to ensure the byte is in RAM.
  static uint8_t tx;
  tx = byte;
  SPI_INSTANCE->TXD.PTR = reinterpret_cast<uint32_t>(&tx);
  SPI_INSTANCE->TXD.MAXCNT = 1;
  SPI_INSTANCE->EVENTS_END = 0;
  SPI_INSTANCE->TASKS_START = 1;
  while (SPI_INSTANCE->EVENTS_END == 0) {
    // Poll until transmission is complete.
  }
  // Clear the event flag.
  SPI_INSTANCE->EVENTS_END = 0;
}

// Send a buffer of bytes over SPI (blocking).
static void spi_send_buffer(const uint8_t* buffer, int len) {
  for (int i = 0; i < len; i++) {
    spi_send_byte(buffer[i]);
  }
}

//--------------------------------------------------------------------------
// Display Functions
//--------------------------------------------------------------------------

// Initialize the display hardware.
void display_init(void) {
  // Configure the control pins as outputs.
  nrf_gpio_cfg_output(LCD_CS_PIN);    // Chip Select
  nrf_gpio_cfg_output(LCD_DC_PIN);     // Data/Command
  nrf_gpio_cfg_output(LCD_RESET_PIN);  // Reset

  // Set default pin states:
  // - CS high: inactive.
  // - DC low: command mode.
  // - RESET high: inactive.
  nrf_gpio_pin_set(LCD_CS_PIN);
  nrf_gpio_pin_clear(LCD_DC_PIN);
  nrf_gpio_pin_set(LCD_RESET_PIN);

  // Initialize the SPI peripheral.
  spi_init();

  // Perform a hardware reset:
  nrf_gpio_pin_clear(LCD_RESET_PIN); // Assert reset.
  nrf_delay_ms(10);                  // Wait 10 ms.
  nrf_gpio_pin_set(LCD_RESET_PIN);    // Deassert reset.
  nrf_delay_ms(10);                  // Allow time for initialization.

  // Optionally clear the display.
  display_clear();
}

// Clear the display by sending a clear-screen command.
void display_clear(void) {
  // Put the display into command mode.
  nrf_gpio_pin_clear(LCD_DC_PIN);
  // Activate the display by pulling CS low.
  nrf_gpio_pin_clear(LCD_CS_PIN);

  // Send the clear command. (0x20 is used as an example; update if necessary.)
  spi_send_byte(0x20);

  // Deactivate the display.
  nrf_gpio_pin_set(LCD_CS_PIN);
}

// Draw a string on the display.
// (A full implementation would convert characters to pixels using a font.
//  This simple example sends the raw string bytes over SPI.)
void display_draw_string(const char* str) {
  // Set the display to data mode.
  nrf_gpio_pin_set(LCD_DC_PIN);
  // Activate the display.
  nrf_gpio_pin_clear(LCD_CS_PIN);

  // Send the string data over SPI.
  spi_send_buffer(reinterpret_cast<const uint8_t*>(str), strlen(str));

  // Deactivate the display.
  nrf_gpio_pin_set(LCD_CS_PIN);
}
