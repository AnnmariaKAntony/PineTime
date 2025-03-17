#include "St7789.h"
#include "SpiMaster.h"
#include "components/Gfx.h"
#include "DisplayApp/DisplayApp.h"
#include "time.h"         


int main(void) {
    Pinetime::Drivers::SpiMaster::Parameters spiParams;
    spiParams.bitOrder = Pinetime::Drivers::SpiMaster::BitOrder::Msb_Lsb;
    spiParams.mode = Pinetime::Drivers::SpiMaster::Modes::Mode0;
    spiParams.Frequency = Pinetime::Drivers::SpiMaster::Frequencies::Freq8Mhz;
    spiParams.pinSCK   = 2;
    spiParams.pinMOSI  = 3;
    spiParams.pinMISO  = 4;
    spiParams.pinCSN   = 25;

    Pinetime::Drivers::SpiMaster spiMaster(Pinetime::Drivers::SpiMaster::SpiModule::SPI0, spiParams);
    spiMaster.Init();

    uint8_t dataCommandPin = 18;
    Pinetime::Drivers::St7789 lcd(spiMaster, dataCommandPin);
    lcd.Init();

    Pinetime::Components::Gfx gfx(lcd);
    Pinetime::Controllers::DateTime dateTimeController;
    Pinetime::Applications::DisplayApp displayApp(lcd, gfx, dateTimeController);

    displayApp.Start();

    return 0;
}
