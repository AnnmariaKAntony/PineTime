#include "St7789.h"
#include "Spi.h"          
#include <hal/nrf_gpio.h>
#include <nrf_delay.h>
#include <cstring>

using namespace Pinetime::Drivers;

St7789::St7789(Spi& spi, uint8_t pinDataCommand, uint8_t pinReset)
  : spi(spi), pinDataCommand(pinDataCommand), pinReset(pinReset) {
}

void St7789::Init() {
  // Configure control pins as outputs.
  nrf_gpio_cfg_output(pinDataCommand);
  nrf_gpio_cfg_output(pinReset);

  // Set default states:
  // - Data/Command pin low (command mode) by default.
  // - Reset pin high (inactive) by default.
  nrf_gpio_pin_set(pinReset);

  // Perform a hardware reset.
  HardwareReset();

  // Perform a software reset.
  SoftwareReset();

  // Exit sleep mode.
  SleepOut();

  // Set pixel format to 16-bit per pixel (65K colors).
  PixelFormatSetup();

  // Configure memory data access control (orientation, RGB/BGR order).
  MemoryDataAccessControlSetup();

  // Set the full display as the drawing window.
  SetAddrWindow(0, 0, Width - 1, Height - 1);

  // Turn the display on.
  DisplayOn();
}

void St7789::HardwareReset() {
  nrf_gpio_pin_clear(pinReset);
  nrf_delay_ms(10);
  nrf_gpio_pin_set(pinReset);
  nrf_delay_ms(10);
}

void St7789::SoftwareReset() {
  WriteCommand(static_cast<uint8_t>(Command::SoftwareReset));
  // Wait for the reset to complete.
  nrf_delay_ms(150);
}

void St7789::SleepOut() {
  WriteCommand(static_cast<uint8_t>(Command::SleepOut));
  // Allow time for the oscillator to stabilize.
  nrf_delay_ms(120);
}

void St7789::PixelFormatSetup() {
  WriteCommand(static_cast<uint8_t>(Command::PixelFormat));
  // 0x55 corresponds to 16 bits per pixel.
  uint8_t fmt = 0x55;
  WriteData(&fmt, 1);
}

void St7789::MemoryDataAccessControlSetup() {
  WriteCommand(static_cast<uint8_t>(Command::MemoryDataAccessControl));
  // Default: no mirror or rotation, RGB order.
  uint8_t madctl = 0x00;
  WriteData(&madctl, 1);
}

void St7789::DisplayOn() {
  WriteCommand(static_cast<uint8_t>(Command::DisplayOn));
  nrf_delay_ms(100);
}

void St7789::SetAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
  // Set column address.
  WriteCommand(static_cast<uint8_t>(Command::ColumnAddressSet));
  uint8_t colArgs[4] = {
    static_cast<uint8_t>(x0 >> 8),
    static_cast<uint8_t>(x0 & 0xFF),
    static_cast<uint8_t>(x1 >> 8),
    static_cast<uint8_t>(x1 & 0xFF)
  };
  WriteData(colArgs, sizeof(colArgs));

  // Set row address.
  WriteCommand(static_cast<uint8_t>(Command::RowAddressSet));
  uint8_t rowArgs[4] = {
    static_cast<uint8_t>(y0 >> 8),
    static_cast<uint8_t>(y0 & 0xFF),
    static_cast<uint8_t>(y1 >> 8),
    static_cast<uint8_t>(y1 & 0xFF)
  };
  WriteData(rowArgs, sizeof(rowArgs));
}

void St7789::DrawBuffer(uint16_t x, uint16_t y, uint16_t width, uint16_t height,
                          const uint8_t* data, size_t size) {
  // Set the drawing window to the specified region.
  SetAddrWindow(x, y, x + width - 1, y + height - 1);
  // Begin writing pixel data.
  WriteCommand(static_cast<uint8_t>(Command::WriteToRam));
  WriteData(data, size);
}

void St7789::WriteCommand(uint8_t cmd) {
  WriteCommand(&cmd, 1);
}

void St7789::WriteCommand(const uint8_t* data, size_t size) {
  WriteSpi(data, size, [this]() {
    // Command mode: pull the data/command pin low.
    nrf_gpio_pin_clear(pinDataCommand);
  });
}

void St7789::WriteData(uint8_t data) {
  WriteData(&data, 1);
}

void St7789::WriteData(const uint8_t* data, size_t size) {
  WriteSpi(data, size, [this]() {
    // Data mode: set the data/command pin high.
    nrf_gpio_pin_set(pinDataCommand);
  });
}

void St7789::WriteSpi(const uint8_t* data, size_t size,
                       const std::function<void()>& preTransactionHook) {
  preTransactionHook();
  spi.sendBuffer(data, size);
}
