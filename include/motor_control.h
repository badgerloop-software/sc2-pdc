#ifndef __MOTOR_CONTROL_H__
#define __MOTOR_CONTROL_H__

#include "Arduino.h"
#include "const.h"
#include "IOManagement.h"
#include "PID.h"
#include "STM32TimerInterrupt_Generic.h"
#include "canPDC.h"

enum class PDCStates : uint8_t {
    OFF,
    PARK,
    IDLE,
    FORWARD,
    REVERSE,
    CRUISE_POWER,
    CRUISE_SPEED
};

enum class CRUZ_MODE : uint8_t {
    OFF,
    SPEED,
    POWER,
};


void initPDCState();
void transition();

//Speed variables
extern volatile float rpm;
extern volatile float motorSpeedSetpoint;
extern volatile CRUZ_MODE cruzMode;
extern volatile PDCStates pdcState;

#endif