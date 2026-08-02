#ifndef __SPEED_CALC_H__
#define __SPEED_CALC_H__

#include <Arduino.h>
#include <HardwareTimer.h>
#ifndef MICROSEC_FORMAT
#define MICROSEC_FORMAT TimerFormat_t::MICROSEC_FORMAT
#endif
#include "STM32TimerInterrupt_Generic.h"
#include "board_config.h"
#include "io_management.h"

// ------------- GLOBALS -------------

extern volatile uint8_t previousPulses[ARRAY_SIZE];

// ------------- FUNCTIONS -------------

void startSpeedCalculation();

#endif  // __SPEED_CALC_H__
