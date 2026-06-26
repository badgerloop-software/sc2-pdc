
#include "motor_control.h"
#include "IOManagement.h"
#include "speed_calc.h"
#include "canPDC.h"

// cruise control variables
PID *curr_PID;

volatile PDCStates pdcState = PDCStates::OFF;
volatile CRUZ_MODE cruzMode = CRUZ_MODE::OFF;
volatile bool cruiseArmed = false;

// PID interval is in seconds and macro is in microseconds
PID power_PID(POWER_P_PARAM, POWER_I_PARAM, POWER_D_PARAM, PID_UPDATE_INTERVAL);
PID speed_PID(SPEED_P_PARAM, SPEED_I_PARAM, SPEED_D_PARAM, PID_UPDATE_INTERVAL);

volatile float speed_pid_compute = 0.0;
STM32TimerInterrupt state_updater(TIM2);

volatile float motorSpeedSetpoint;

static float mphToRpm(float targetMph) {
  if (targetMph <= 0.0f) {
    return 0.0f;
  }
  return targetMph / (WHEEL_CIRCUMFERENCE * 0.00094696f);
}

static void disengageCruise() {
  cruiseArmed = false;
  cruzMode = CRUZ_MODE::OFF;
  if (pdcState == PDCStates::CRUISE_SPEED) {
    pdcState = PDCStates::FORWARD;
  }
}

// set default state to OFF
void initPDCState() {
  pdcState = PDCStates::OFF;
  cruiseArmed = false;
  motorSpeedSetpoint = 0.0f;

  // initialize Ticker to run the transition method every pid update interval
  // seconds
  state_updater.attachInterruptInterval(IO_UPDATE_PERIOD, transition);
  // set limits of inputs and outputs
  power_PID.setInputLimits(MIN_POWER, MAX_POWER);
  power_PID.setOutputLimits(MIN_OUT, MAX_OUT);
  speed_PID.setInputLimits(MIN_RPM, MAX_RPM);
  speed_PID.setOutputLimits(MIN_OUT, MAX_OUT);
  speed_PID.setMode(AUTO_MODE);

  curr_PID = &power_PID;
  set_eco_mode(true);
}

PDCStates get_state() { return pdcState; }

void transition() {
  if (!cruiseEnabled || !cruiseArmed) {
    cruiseSetPulse = false;
    cruiseResetPulse = false;
  }

  switch (pdcState) {
  case PDCStates::PARK:
    if (!digital_data.park_brake) {
      pdcState = PDCStates::IDLE;
    }
    // OUTPUT: make sure the motor isn't spinning when it's in PARK
    writeAccOut(0.0);
    writeRegenBrake(0.0);
    break;

  case PDCStates::IDLE:
    // Transition to PARK if park brake is pulled
    if (digital_data.park_brake) {
      pdcState = PDCStates::PARK;
      break;
    }

    // OUTPUT: set acc_out pin based on acc_in from the pedal
    // this allows us to accelerate into REVERSE or FORWARD states
    set_direction(forwardAndReverse);
    writeAccOut(acc_in);
    writeRegenBrake(regen_in);

    // car moving fast, transition to locked direction state
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
    // OUTPUT: set acc_out pin based on acc_in from the pedal
    set_direction(REVERSE_VALUE);
    writeAccOut(acc_in);
    writeRegenBrake(regen_in);
    break;

  case PDCStates::FORWARD:
    if (rpm < MIN_MOVING_SPEED) {
      pdcState = PDCStates::IDLE;
      break;
    }

    if (cruiseEnabled && !cruiseArmed) {
      motorSpeedSetpoint = mph;
      cruiseArmed = true;
      cruzMode = CRUZ_MODE::SPEED;
      pdcState = PDCStates::CRUISE_SPEED;
      break;
    }

    set_direction(FORWARD_VALUE);
    writeAccOut(acc_in);
    writeRegenBrake(regen_in);
    break;

  case PDCStates::CRUISE_POWER:
    if (cruzMode != CRUZ_MODE::POWER) {
      pdcState = PDCStates::FORWARD;
      break;
    }
    // TODO: not enough stuff for power right now (10/22)
    // get current motor power
    // set setPoint to target power
    // writeAccOut(compute());
    break;

  case PDCStates::CRUISE_SPEED:
    if (!cruiseEnabled || !cruiseArmed || rpm < MIN_MOVING_SPEED) {
      disengageCruise();
      break;
    }

    if (cruiseSetPulse) {
      motorSpeedSetpoint += 1.0f;
      cruiseSetPulse = false;
    }
    if (cruiseResetPulse) {
      motorSpeedSetpoint -= 1.0f;
      if (motorSpeedSetpoint < 0.0f) {
        motorSpeedSetpoint = 0.0f;
      }
      cruiseResetPulse = false;
    }

    set_direction(FORWARD_VALUE);
    speed_PID.setProcessValue(rpm);
    speed_PID.setSetPoint(mphToRpm(motorSpeedSetpoint));
    speed_pid_compute = speed_PID.compute();
    writeAccOut(speed_pid_compute);
    writeRegenBrake(regen_in);
    break;

  // OFF state as our default
  default:
    if (digital_data.mcu_mc_on) {
      pdcState = PDCStates::PARK;
      break;
    }
    // OUTPUT: make sure the motor isn't spinning when it's OFF
    writeAccOut(0.0);
    writeRegenBrake(0.0);
    // Set to known state since it is our default
    pdcState = PDCStates::OFF;
    break;
  }

  // STATE MACHINE OVERRIDES AND SAFETY CHECKS

  // use if, else if because we should only switch to 1 state.
  // put higher priority checks up top.
  if (!digital_data.mcu_mc_on) {
    // the motor is off, so go into OFF state
    pdcState = PDCStates::OFF;
    writeAccOut(0.0);
    writeRegenBrake(0.0);
  }

  // SAFETY OVERRIDE: Zero outputs and disengage cruise on park or foot brake.
  if (digital_data.park_brake) {
    disengageCruise();
    writeAccOut(0.0);
    writeRegenBrake(0.0);
  }

  if (brake_pressure_telem > BRAKE_SENSOR_THRESHOLD) {
    disengageCruise();
    writeAccOut(0.0);
    writeRegenBrake(0.0);
  }
  // set brakeLED based on analog brake sensor
  digital_data.brake_led = brake_pressure_telem > BRAKE_SENSOR_THRESHOLD;
}
