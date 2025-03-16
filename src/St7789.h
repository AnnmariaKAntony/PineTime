#ifndef ST7789_H
#define ST7789_H

#include <cstddef>
#include <cstdint>
#include <functional>

namespace Pinetime {
namespace Drivers {

  // Forward declaration of your SPI interface class.
  class Spi;

  class St7789 {
  public:
    /// Construct the ST7789 driver with a given SPI interface, data/command pin, and reset pin.
    St7789(Spi& spi, uint8_t pinDataCommand, uint8_t pinReset);
    
    /// Initialize the display.
    void Init();
    
    /// Draw a buffer to a specified rectangle.
    void DrawBuffer(uint16_t x, uint16_t y, uint16_t width, uint16_t height,
                    const uint8_t* data, size_t size);

    // Optional destructor.
    ~St7789() = default;

  private:
    Spi& spi;
    uint8_t pinDataCommand;
    uint8_t pinReset;

    // Minimal command set needed.
    enum class Command : uint8_t {
      SoftwareReset         = 0x01,
      SleepOut              = 0x11,
      PixelFormat           = 0x3A,
      MemoryDataAccessControl = 0x36,
      WriteToRam            = 0x2C,
      DisplayOn             = 0x29,
      ColumnAddressSet      = 0x2A,
      RowAddressSet         = 0x2B
    };

    // Constants for display dimensions.
    static constexpr uint16_t Width  = 240;
    static constexpr uint16_t Height = 320;

    // Helper functions for initialization.
    void HardwareReset();
    void SoftwareReset();
    void SleepOut();
    void PixelFormatSetup();
    void MemoryDataAccessControlSetup();
    void DisplayOn();

    // Set the drawing window.
    void SetAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);

    // SPI transaction helpers.
    void WriteCommand(uint8_t cmd);
    void WriteCommand(const uint8_t* data, size_t size);
    void WriteData(uint8_t data);
    void WriteData(const uint8_t* data, size_t size);
    void WriteSpi(const uint8_t* data, size_t size,
                  const std::function<void()>& preTransactionHook);
  };

} // namespace Drivers
} // namespace Pinetime

#endif // ST7789_H
