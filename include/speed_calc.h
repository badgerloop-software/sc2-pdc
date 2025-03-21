#ifndef __SPEED_CALC_H__
#define __SPEED_CALC_H__

#include "Arduino.h"
#include "const.h"
#include "IOManagement.h"
#include "STM32TimerInterrupt_Generic.h"

#define SPEED_CALC_INTERVAL_US 50000 // us
#define ARRAY_SIZE 20 
#define PULSES_PER_REV 48
#define WHEEL_CIRCUMFERENCE 69 // inches

#define MCU_SPEED_SIG PA8

void startSpeedCalculation();


// Starts speed (RPM and MPH) calculations at specified interval
extern volatile float rpm;
extern volatile float mph;
extern volatile unsigned int previousPulses[ARRAY_SIZE];

#endif