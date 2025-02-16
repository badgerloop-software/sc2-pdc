#ifndef __MOTOR_CONTROL_H__
#define __MOTOR_CONTROL_H__

#include "Arduino.h"
#include "const.h"
#include "digitalPins.h"
#include "analogPins.h"
#include "PID.h"
#include "STM32TimerInterrupt_Generic.h"
#include "canPDC.h"

void MCCState();
MCCStates get_state();

// cruise control variables
PID *curr_PID;
PID power_PID, speed_PID;

#endif