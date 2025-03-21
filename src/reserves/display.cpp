#include "display.h"
#include <string.h>
#include "nrf_gpio.h"
#include "nrf_spim.h"
#include "nrf_delay.h"

// Pin Definitions 
static constexpr uint8_t LCD_SPI_SCK    = 2;  // SPI clock
static constexpr uint8_t LCD_SPI_MOSI   = 3;  // SPI MOSI
static constexpr uint8_t LCD_SPI_MISO   = 4;  // SPI MISO (unused in this example)

// Display control pins
static constexpr uint8_t LCD_CS_PIN     = 25; // Chip Select for LCD
static constexpr uint8_t LCD_DC_PIN     = 18; // Data/Command control
static constexpr uint8_t LCD_RESET_PIN  = 26; // Reset pin

#define SPI_INSTANCE NRF_SPIM0

// configure the SPI peripheral (SPIM0) for the display
static void spi_init(void) {
  // configure SPI GPIO pins
  nrf_gpio_cfg_output(LCD_SPI_SCK);
  nrf_gpio_cfg_output(LCD_SPI_MOSI);
  nrf_gpio_cfg_input(LCD_SPI_MISO, NRF_GPIO_PIN_NOPULL);

  // assign the pins to the SPIM0 peripheral
  SPI_INSTANCE->PSELSCK  = LCD_SPI_SCK;
  SPI_INSTANCE->PSELMOSI = LCD_SPI_MOSI;
  SPI_INSTANCE->PSELMISO = LCD_SPI_MISO;

  // set SPI frequency to 8MHz
  SPI_INSTANCE->FREQUENCY = SPIM_FREQUENCY_FREQUENCY_M8;

  // configure SPI as based off InfiniTime: Mode 0 (CPOL = 0, CPHA = 0), MSB
  SPI_INSTANCE->CONFIG = (SPIM_CONFIG_ORDER_MsbFirst << SPIM_CONFIG_ORDER_Pos) |
                         (SPIM_CONFIG_CPHA_Leading   << SPIM_CONFIG_CPHA_Pos) |
                         (SPIM_CONFIG_CPOL_ActiveHigh << SPIM_CONFIG_CPOL_Pos);

  SPI_INSTANCE->ENABLE = SPIM_ENABLE_ENABLE_Enabled << SPIM_ENABLE_ENABLE_Pos;
}

// send a byte over SPI 
static void spi_send_byte(uint8_t byte) {
  static uint8_t tx;
  tx = byte;
  SPI_INSTANCE->TXD.PTR = reinterpret_cast<uint32_t>(&tx);
  SPI_INSTANCE->TXD.MAXCNT = 1;
  SPI_INSTANCE->EVENTS_END = 0;
  SPI_INSTANCE->TASKS_START = 1;
  while (SPI_INSTANCE->EVENTS_END == 0) { } // spin
  SPI_INSTANCE->EVENTS_END = 0;
}

// send a bunch of bytes over SPI
static void spi_send_buffer(const uint8_t* buffer, int len) {
  for (int i = 0; i < len; i++) {
    spi_send_byte(buffer[i]);
  }
}

// initialize the display
void display_init(void) {
  nrf_gpio_cfg_output(LCD_CS_PIN);    
  nrf_gpio_cfg_output(LCD_DC_PIN);     
  nrf_gpio_cfg_output(LCD_RESET_PIN);  

  nrf_gpio_pin_set(LCD_CS_PIN);
  nrf_gpio_pin_clear(LCD_DC_PIN);
  nrf_gpio_pin_set(LCD_RESET_PIN);

  spi_init();

  // hardware reset
  nrf_gpio_pin_clear(LCD_RESET_PIN); 
  nrf_delay_ms(10);                 
  nrf_gpio_pin_set(LCD_RESET_PIN);
  nrf_delay_ms(10);             

  display_clear();
}

// clear the display
void display_clear(void) {
  nrf_gpio_pin_clear(LCD_DC_PIN);
  nrf_gpio_pin_clear(LCD_CS_PIN);   // activates display
  spi_send_byte(0x20);              // 0x20 = clear command
  nrf_gpio_pin_set(LCD_CS_PIN);     // deactivates display
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
