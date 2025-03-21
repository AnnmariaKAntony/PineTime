#pragma once
#include <cstdint>
#include <nrf_font.h>
#include "../BufferProvider.h"


namespace Pinetime {
  namespace Drivers {
    class St7789;
  }
  namespace Components {
    class Gfx : public Pinetime::Drivers::BufferProvider {
      public:
        explicit Gfx(Drivers::St7789& lcd);
        void Init();
        void ClearScreen();
        void DrawString(uint8_t x, uint8_t y, uint16_t color, const char* text, const FONT_INFO *p_font, bool wrap);
        void DrawChar(const FONT_INFO *font, uint8_t c, uint8_t *x, uint8_t y, uint16_t color);
        void FillRectangle(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint16_t color);

        void Sleep();
        void Wakeup();
        bool GetNextBuffer(uint8_t **buffer, size_t &size) override;

      private:
        static constexpr uint8_t width = 240;
        static constexpr uint8_t height = 240;

        enum class Action { None, FillRectangle, DrawChar};
        struct State {
          State() : busy{false}, action{Action::None}, remainingIterations{0}, currentIteration{0} {}
          volatile bool busy;
          volatile Action action;
          volatile uint16_t remainingIterations;
          volatile uint16_t currentIteration;
          volatile FONT_INFO *font;
          volatile uint16_t color;
          volatile uint8_t character;
        };

        volatile State state;

        uint16_t buffer[width]; // 1 line buffer
        Drivers::St7789& lcd;

        void pixel_draw(uint8_t x, uint8_t y, uint16_t color);
        void SetBackgroundColor(uint16_t color);
    };
  }
}