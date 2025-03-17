#pragma once

#include "../St7789.h"
#include "../SpiMaster.h"
#include "../components/Gfx.h"
// #include <queue.h>
#include "../time.h"
#include "Fonts/lcdfont14.h"
#include "Screens/Clock.h"

extern const FONT_INFO lCD_70ptFontInfo;

namespace Pinetime {
    namespace Applications {
        class DisplayApp {
            public:
                DisplayApp(Pinetime::Drivers::St7789& lcd,
                    Pinetime::Components::Gfx& gfx,
                    // Pinetime::Drivers::Cst816S&,
                    // Controllers::Battery &batteryController,
                    // Controllers::Ble &bleController,
                    Controllers::DateTime& dateTimeController);

                void Start();
                
            private:
                static void Process(void* instance);
                void InitHw();
                Pinetime::Drivers::St7789& lcd;
                Pinetime::Components::Gfx& gfx;
                const FONT_INFO largeFont {lCD_70ptFontInfo.height, lCD_70ptFontInfo.startChar, lCD_70ptFontInfo.endChar, lCD_70ptFontInfo.spacePixels, lCD_70ptFontInfo.charInfo, lCD_70ptFontInfo.data};
                const FONT_INFO smallFont {lCD_14ptFontInfo.height, lCD_14ptFontInfo.startChar, lCD_14ptFontInfo.endChar, lCD_14ptFontInfo.spacePixels, lCD_14ptFontInfo.charInfo, lCD_14ptFontInfo.data};
                void Refresh();

                Pinetime::Controllers::DateTime& dateTimeController;

                Screens::Clock clockScreen;
                Screens::Screen* currentScreen = nullptr;
                void RunningState();
                void IdleState();

                static constexpr uint8_t pinLcdBacklight1 = 14;
                static constexpr uint8_t pinLcdBacklight2 = 22;
                static constexpr uint8_t pinLcdBacklight3 = 23;
        };
    }
}