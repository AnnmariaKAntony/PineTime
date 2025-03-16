#include "Spi.h"
#include "nrf_gpio.h"
#include "nrf_spim.h"
#include "nrf_delay.h"


namespace Pinetime {
    namespace Drivers {
    

        //--------------------------------------------------------------------------
        // SPI Pin Definitions for Datetime Controller (adjust these as needed)
        //--------------------------------------------------------------------------
        static constexpr uint8_t DATETIME_SPI_SCK  = 2; // SPI clock for datetime controller
        static constexpr uint8_t DATETIME_SPI_MOSI = 3; // SPI MOSI for datetime controller
        static constexpr uint8_t DATETIME_SPI_MISO = 4; // SPI MISO (if used; may be unused)

        static constexpr uint8_t DATETIME_CS_PIN   = 25; // Chip Select for datetime controller
        static constexpr uint8_t DATETIME_DC_PIN   = 18; // Data/Command control (if applicable)

        // Use a separate SPI instance (NRF_SPIM1) for the datetime controller.
        #define SPI_INSTANCE NRF_SPIM1

        //--------------------------------------------------------------------------
        // Low-Level SPI Functions (local to this file)
        //--------------------------------------------------------------------------

        static void spi_low_level_init(void) {
        // Configure SPI GPIO pins.
        nrf_gpio_cfg_output(DATETIME_SPI_SCK);
        nrf_gpio_cfg_output(DATETIME_SPI_MOSI);
        nrf_gpio_cfg_input(DATETIME_SPI_MISO, NRF_GPIO_PIN_NOPULL);

        // Assign pins to the SPI peripheral.
        SPI_INSTANCE->PSELSCK  = DATETIME_SPI_SCK;
        SPI_INSTANCE->PSELMOSI = DATETIME_SPI_MOSI;
        SPI_INSTANCE->PSELMISO = DATETIME_SPI_MISO;

        // Set the SPI frequency to 8MHz (adjust if needed).
        SPI_INSTANCE->FREQUENCY = SPIM_FREQUENCY_FREQUENCY_M8;

        // Configure SPI in Mode 0 (CPOL=0, CPHA=0), MSB first.
        SPI_INSTANCE->CONFIG = (SPIM_CONFIG_ORDER_MsbFirst << SPIM_CONFIG_ORDER_Pos) |
                                (SPIM_CONFIG_CPHA_Leading   << SPIM_CONFIG_CPHA_Pos) |
                                (SPIM_CONFIG_CPOL_ActiveHigh << SPIM_CONFIG_CPOL_Pos);

        // Enable the SPI peripheral.
        SPI_INSTANCE->ENABLE = SPIM_ENABLE_ENABLE_Enabled << SPIM_ENABLE_ENABLE_Pos;
        }

        static void spi_low_level_send_byte(uint8_t byte) {
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

        static void spi_low_level_send_buffer(const uint8_t* buffer, uint32_t length) {
        for (uint32_t i = 0; i < length; i++) {
            spi_low_level_send_byte(buffer[i]);
        }
        }

        //--------------------------------------------------------------------------
        // Spi Class Implementation
        //--------------------------------------------------------------------------

        Spi::Spi() {
        // Optionally, initialization code can go here.
        }

        void Spi::init() {
        // Configure control pins.
        nrf_gpio_cfg_output(DATETIME_CS_PIN);
        nrf_gpio_cfg_output(DATETIME_DC_PIN); // Only needed if using a data/command pin

        // Set default states:
        // - CS high: inactive.
        // - DC low: command mode (if applicable).
        nrf_gpio_pin_set(DATETIME_CS_PIN);
        nrf_gpio_pin_clear(DATETIME_DC_PIN);

        // Initialize the SPI hardware.
        spi_low_level_init();

        // Optionally perform a hardware initialization sequence if required.
        nrf_delay_ms(10);
        }

        void Spi::sendByte(uint8_t byte) {
        // Activate communication: pull CS low.
        nrf_gpio_pin_clear(DATETIME_CS_PIN);
        spi_low_level_send_byte(byte);
        // End communication: pull CS high.
        nrf_gpio_pin_set(DATETIME_CS_PIN);
        }

        void Spi::sendBuffer(const uint8_t* buffer, uint32_t length) {
        // Activate communication.
        nrf_gpio_pin_clear(DATETIME_CS_PIN);
        spi_low_level_send_buffer(buffer, length);
        // End communication.
        nrf_gpio_pin_set(DATETIME_CS_PIN);
        }

    }
}