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
void transition();
MCCStates get_state();

//Speed variables
extern volatile float rpm;
extern volatile float motorSpeedSetpoint;

// cruise control variables
PID *curr_PID;

#endif