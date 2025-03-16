#include "DisplayApp.h"
#include <nrf_log.h>
#include <boards.h>
#include <nrf_font.h>
#include <hal/nrf_rtc.h>
#include "../components/Gfx.h"
#include <queue.h>
#include <../time.h>
#include <chrono>
#include <string>

using namespace Pinetime::Applications;

DisplayApp::DisplayApp(Pinetime::Drivers::St7789& lcd,
        Pinetime::Components::Gfx& gfx,
        Controllers::DateTime &dateTimeController) :
    lcd{lcd},
    gfx{gfx},
    dateTimeController{dateTimeController},
    clockScreen{gfx},
{
    currentScreen = &clockScreen;
}

void DisplayApp::Start() {
    // if (pdPASS != xTaskCreate(DisplayApp::Process, "DisplayApp", 256, this, 0, &taskHandle))
    //   APP_ERROR_HANDLER(NRF_ERROR_NO_MEM);
}

void DisplayApp::Process(void *instance) {
    auto *app = static_cast<DisplayApp *>(instance);
    NRF_LOG_INFO("DisplayApp task started!");
    app->InitHw();
    while (1) {
        app->Refresh();
    }
}

void DisplayApp::InitHw() {
    nrf_gpio_cfg_output(pinLcdBacklight1);
    nrf_gpio_cfg_output(pinLcdBacklight2);
    nrf_gpio_cfg_output(pinLcdBacklight3);
    nrf_gpio_pin_clear(pinLcdBacklight1);
    nrf_gpio_pin_clear(pinLcdBacklight2);
    nrf_gpio_pin_clear(pinLcdBacklight3);
  
    currentScreen->Refresh(true);
}

uint32_t acc = 0;
uint32_t count = 0;
bool toggle = true;

void DisplayApp::Refresh() {
    uint32_t before = nrf_rtc_counter_get(portNRF_RTC_REG);
    if(toggle) {
        gfx.FillRectangle(0,0,240,240,0x0000);
    } else {
        gfx.FillRectangle(0,0,240,240,0xffff);
    }
    uint32_t after = nrf_rtc_counter_get(portNRF_RTC_REG);

    acc += (after - before);
    if((count % 10) == 0) {
        NRF_LOG_INFO("DRAW : %d ms", (uint32_t)(acc/10));
        acc = 0;
    }
    count++;
    toggle = !toggle;

}

void DisplayApp::RunningState() {
    clockScreen.SetCurrentDateTime(dateTimeController.CurrentDateTime());
  
  //  if(currentScreen != nullptr) {
  //    currentScreen->Refresh(false);
  //  }
  
    if(currentScreen != nullptr) {
      currentScreen->Refresh(true);
    }
  
    if(screenState) {
      currentScreen = &clockScreen;
    } else {
      currentScreen = &messageScreen;
    }
    screenState = !screenState;
  }

void DisplayApp::IdleState() {

}


