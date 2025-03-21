Project: Basic OS implementation for the PineTime hardware
Authors: Annmaria Antony, Sawyer Lai

We attempted to implement basic functionality (displaying time and date) on a PineTime SmartWatch. To do this, we implemented basic time modules (time.h and time.cpp), as well as implemented basic Spi (SpiMaster.cpp, SpiMaster.h) and display drivers (DisplayApp.cpp, DisplayApp.h). Our implementation drew inspiration from the InfiniTime Repo, but instead of implementing a full bootloader from scratch, we opted to flash over a standalone firmware version of the app.

The PineTime's main processor is the nRF52832, an SOC developed by Nordic Semiconductors. We found open-source nrf linker files on the internet to interface with the chip (nrf_common.ld and gcc_nrf52_ld). In addition to this, we implemented a simple startup_nrf52.s assembly file that gets the system running (defines a Reset_Handler that jumps to main). 

To flash the actual app, we used an StLink-V2 to flash the program to the watch directly, using OpenOCD. To make this possible, we created two files to specify flash configuration (openocd-stlink.ocd ensures that OpenOCD can communicate with the nRF5 chip, and flash_bootloader_app.ocd specifies how to flash the firmware to the PineTime). 

1) Run `make` in the root directory
2) Connect computer and watch via StLink-V2 
3) run `openocd -f interface/stlink.cfg -f target/nrf52.cfg -c "program build/src/pinetime-app-1.15.0.elf verify reset exit"` to flash program

Some other thoughts:
    - It was actually SO difficult to even get the original repo to flash to the watch. It took us ~4 hours to set-up the original InfiniTime Repo, and another 4 hrs to figure out how to flash correctly. We didn't anticipate that at all!
    - InfiniTime uses an open-source OS called FreeRTOS that enables a lot of its functionality. Without a fully working OS, it would be very difficult to enable more features that require mutexes, locks, etc.
