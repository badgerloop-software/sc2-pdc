// motor_control: drive state machine on TIM2
// Inputs: park_brake, forwardAndReverse (CAN), acc_in, regen_in, rpm, brake_pressed
// Outputs: MCU direction pin, accel DAC, regen DAC
#include "motor_control.h"
#include "IOManagement.h"
#include "canPDC.h"

volatile PDCStates pdcState = PDCStates::OFF;
STM32TimerInterrupt state_updater(TIM2);

void initPDCState() {
  pdcState = PDCStates::PARK;
  state_updater.attachInterruptInterval(IO_UPDATE_PERIOD, transition);
  set_eco_mode(true);
  set_direction(FORWARD_VALUE);
}

PDCStates get_state() { return pdcState; }

void transition() {
  // PARK -> IDLE when park is false (sensor removed, so this happens at once)
  // IDLE -> FORWARD/REVERSE when rpm is high enough
  // FORWARD/REVERSE -> IDLE when rpm falls below MIN_MOVING_SPEED
  // Park or pedal brake forces accel and regen to zero
  // TODO: make safer for car 2.5
  switch (pdcState) {
  case PDCStates::PARK:
    if (!digital_data.park_brake) {
      pdcState = PDCStates::IDLE;
    }
    writeAccOut(0.0);
    writeRegenBrake(0.0);
    break;

  case PDCStates::IDLE:
    if (digital_data.park_brake) {
      pdcState = PDCStates::PARK;
      break;
    }

    set_direction(forwardAndReverse);
    writeAccOut(acc_in);
    writeRegenBrake(regen_in);

    if (rpm >= MIN_MOVING_SPEED) {
      if (forwardAndReverse == FORWARD_VALUE) {
        pdcState = PDCStates::FORWARD;
      } else {
        pdcState = PDCStates::REVERSE;
      }
    }
    break;

  case PDCStates::REVERSE:
    if (rpm < MIN_MOVING_SPEED) {
      pdcState = PDCStates::IDLE;
    }
    set_direction(REVERSE_VALUE);
    writeAccOut(acc_in);
    writeRegenBrake(regen_in);
    break;

  case PDCStates::FORWARD:
    if (rpm < MIN_MOVING_SPEED) {
      pdcState = PDCStates::IDLE;
      break;
    }

    set_direction(FORWARD_VALUE);
    writeAccOut(acc_in);
    writeRegenBrake(regen_in);
    break;

  default:
    pdcState = PDCStates::PARK;
    break;
  }

  if (digital_data.park_brake) {
    writeAccOut(0.0);
    writeRegenBrake(0.0);
  }

  if (brake_pressed) {
    writeAccOut(0.0);
    writeRegenBrake(0.0);
  }
}
