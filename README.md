# DocetOS

DocetOS is a small real-time operating system for the STM32F303 Nucleo Device. It was initially written by Dr. Andy Promfret for a Systems Programming for ARM module at the University of York. Modifications were added by me for said module.

Its features are:

- Fixed-priority memeory scheduler.
- Recursive mutexes.
- Counting semaphores.
- Memory pool.
- Inter-task communication system.

## Required IDE
To build this project and flash to the device, Keil uVision is required. The free version will suffice.

## Required Packs
- ARM::CMSIS 5.7.0
- Keil::STM32Fxx_DFP 2.1.0

