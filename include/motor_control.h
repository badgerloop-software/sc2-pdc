#ifndef __MOTOR_CONTROL_H__
#define __MOTOR_CONTROL_H__

#include <Arduino.h>
#include <HardwareTimer.h>
#ifndef MICROSEC_FORMAT
#define MICROSEC_FORMAT TimerFormat_t::MICROSEC_FORMAT
#endif
#include "STM32TimerInterrupt_Generic.h"
#include "board_config.h"
#include "can_pdc.h"
#include "io_management.h"

// ------------- FUNCTIONS -------------

void initPDCState();
void transition();
PDCStates get_state();

#endif  // __MOTOR_CONTROL_H__
