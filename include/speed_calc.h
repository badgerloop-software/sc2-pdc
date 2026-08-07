#ifndef __SPEED_CALC_H__
#define __SPEED_CALC_H__

#include "Arduino.h"
#include "IOManagement.h"
#include "STM32TimerInterrupt_Generic.h"
#include "const.h"

// MCU_SPEED_SIG is defined in IOManagement.h (PA8)
#define SPEED_CALC_INTERVAL_US 50000 // us
#define ARRAY_SIZE 20
#define PULSES_PER_REV 48
#define WHEEL_CIRCUMFERENCE 69 // inches

// Writes rpm and mph (declared in IOManagement.h) from MCU_SPEED_SIG pulses
void startSpeedCalculation();

#endif
