#include "DisplayApp.h"
#include <nrf_log.h>

#define BOARD_PINETIME_DEVKIT1
// #include <boards.h>

#include <nrf_gpio.h>
#include <nrf_font.h>
#include <hal/nrf_rtc.h>
#include "../components/Gfx.h"
#include <chrono>
#include <string>

using namespace Pinetime::Applications;

DisplayApp::DisplayApp(Pinetime::Drivers::St7789& lcd,
        Pinetime::Components::Gfx& gfx,
        Controllers::DateTime &dateTimeController) :
    lcd{lcd},
    gfx{gfx},
    dateTimeController{dateTimeController},
    clockScreen{gfx} 
{
    currentScreen = &clockScreen;
}

void DisplayApp::Start() {
    NRF_LOG_INFO("DisplayApp started!");
    InitHw();
    
    while (true) {
        Refresh();
        
        // Add a simple busy-wait delay to avoid looping too fast.
        // You might want to replace this with a more appropriate timing function.
        for (volatile uint32_t i = 0; i < 1000000; i++) {
            __asm("nop");
        }
    }
}


void DisplayApp::InitHw() {
    nrf_gpio_cfg_output(pinLcdBacklight1);
    nrf_gpio_cfg_output(pinLcdBacklight2);
    nrf_gpio_cfg_output(pinLcdBacklight3);
    nrf_gpio_pin_set(pinLcdBacklight1);
    nrf_gpio_pin_set(pinLcdBacklight2);
    nrf_gpio_pin_set(pinLcdBacklight3);
  
    // Refresh the current screen immediately.
    currentScreen->Refresh(true);
}

uint32_t acc = 0;
uint32_t count = 0;
bool toggle = true;

void DisplayApp::Refresh() {
    // sawyer note: just trying to get a rectangle of white filled in
    gfx.FillRectangle(0, 0, 240, 240, 0xFFFF);  // White

    // uint32_t before = nrf_rtc_counter_get(NRF_RTC1);
    
    // // Toggle between two colors (black and white) for demonstration.
    // gfx.FillRectangle(0, 0, 240, 240, 0xFFFF);  // White
    // // if (toggle) {
    // //     gfx.FillRectangle(0, 0, 240, 240, 0x0000);  // Black
    // // } else {
    // //     gfx.FillRectangle(0, 0, 240, 240, 0xFFFF);  // White
    // // }
    
    // uint32_t after = nrf_rtc_counter_get(NRF_RTC1);
    // acc += (after - before);
    
    // if ((count % 10) == 0) {
    //     NRF_LOG_INFO("DRAW : %d ms", (uint32_t)(acc / 10));
    //     acc = 0;
    // }
    
    // count++;
    // toggle = !toggle;c
}

void DisplayApp::RunningState() {
    // Example state: update the clock screen with the current date/time.
    clockScreen.SetCurrentDateTime(std::chrono::time_point_cast<std::chrono::milliseconds>(dateTimeController.CurrentDateTime()));
    
    
    if (currentScreen != nullptr) {
        currentScreen->Refresh(true);
    }
    
    // If you had more than one screen, you could toggle between them here.
}

void DisplayApp::IdleState() {
    // Implement idle behavior if needed (e.g., dim the display).
}
