#ifndef SPI_H
#define SPI_H

#include <cstdint>

namespace Pinetime {
namespace Drivers {

class Spi {
public:
  /// Constructor.
  Spi();

  /// Initialize the SPI peripheral.
  void init();

  /// Send a single byte over SPI.
  void sendByte(uint8_t byte);

  /// Send a buffer of data over SPI.
  void sendBuffer(const uint8_t* buffer, uint32_t length);
};

} // namespace Drivers
} // namespace Pinetime

#endif // SPI_H
