
#include "motor_control.h"

volatile MCCStates mccState = MCCStates::OFF;
volatile CRUZ_MODE cruzMode = CRUZ_MODE::OFF;

//PID interval is in seconds and macro is in microseconds
PID power_PID(POWER_D_PARAM, POWER_I_PARAM, POWER_P_PARAM, PID_UPDATE_INTERVAL);
PID speed_PID(SPEED_D_PARAM, SPEED_I_PARAM, SPEED_P_PARAM, PID_UPDATE_INTERVAL);

volatile float speed_pid_compute = 0.0;
STM32TimerInterrupt state_updater(TIM7);

volatile bool brakeLed = false;
volatile bool forwardAndReverse = false;
volatile bool mc_on = false;

volatile float rpm;
volatile float motorSpeedSetpoint;

// set default state to OFF
void MCCState(){ 
    mccState = MCCStates::OFF;

    // initialize Ticker to run the transition method every pid update interval seconds
    state_updater.attachInterruptInterval(PID_UPDATE_INTERVAL, transition);
    // set limits of inputs and outputs
    power_PID.setInputLimits(MIN_POWER, MAX_POWER);
    power_PID.setOutputLimits(MIN_OUT, MAX_OUT);
    speed_PID.setInputLimits(MIN_RPM, MAX_RPM);
    speed_PID.setOutputLimits(MIN_OUT, MAX_OUT);
    speed_PID.setMode(AUTO_MODE);
    
    curr_PID = &power_PID;
}

void transition() {
    switch (mccState) {
        case MCCStates::PARK:
            if (!digital_data.park_brake) {
                mccState = MCCStates::IDLE;
            }
            // OUTPUT: make sure the motor isn't spinning when it's in PARK
            writeAccOut(0.0);
            break;

        case MCCStates::IDLE:
            // OUTPUT: set acc_out pin based on acc_in from the pedal
            // this allows us to accelerate into REVERSE or FORWARD states
            writeAccOut(acc_in);
            set_direction(digital_data.direction);

            // car moving fast, transition to locked direction state
            if (rpm >= MIN_MOVING_SPEED) {
                if (forwardAndReverse == FORWARD_VALUE) {
                    mccState = MCCStates::FORWARD;
                } else {
                    mccState = MCCStates::REVERSE;
                }
            } 
            break;

        case MCCStates::REVERSE:
            if (rpm < MIN_MOVING_SPEED) {
                mccState = MCCStates::IDLE;
            }
            // OUTPUT: set acc_out pin based on acc_in from the pedal
            writeAccOut(acc_in);
            break;

        case MCCStates::FORWARD:
            if (rpm < MIN_MOVING_SPEED) {
                mccState = MCCStates::IDLE;

            // Cruise PID will not be tuned for comp. Disable
            } /* else if (digital_data.cruiseEnabled) {
                if (cruzMode == CRUZ_MODE::POWER) {
                    curr_PID = &power_PID;
                    state = MCCStates::CRUISE_POWER;
                } else if (cruzMode == CRUZ_MODE::SPEED) {
                    curr_PID = &speed_PID;
                    state = MCCStates::CRUISE_SPEED;
                }
            } */
            
            // OUTPUT: set acc_out pin based on acc_in from the pedal
            writeAccOut(acc_in);
            break;

        case MCCStates::CRUISE_POWER:
            if (cruzMode != CRUZ_MODE::POWER) {
                mccState = MCCStates::FORWARD;
                break;
            } 
            // TODO: not enough stuff for power right now (10/22)
            // get current motor power
            // set setPoint to target power
            // setAccOut(compute());
            break;

        case MCCStates::CRUISE_SPEED:
            if (cruzMode != CRUZ_MODE::SPEED) {
                mccState = MCCStates::FORWARD;
                break;
            } 
            // set current rpm for speed_PID
            speed_PID.setProcessValue(rpm);
            // set target for speed_PID
            speed_PID.setSetPoint(motorSpeedSetpoint);
            speed_pid_compute = speed_PID.compute();
            // set accelerator 
            writeAccOut(speed_pid_compute);
            break;

        // OFF state as our default
        default:
            if (mc_on) {
                mccState = MCCStates::PARK;
                break;
            }
            // OUTPUT: make sure the motor isn't spinning when it's OFF
            writeAccOut(0.0);
            // Set to known state since it is our default
            mccState = MCCStates::OFF;
            break;
    }

    // STATE MACHINE OVERRIDES AND SAFETY CHECKS

    // use if, else if because we should only switch to 1 state.
    // put higher priority checks up top.
    if (!mc_on) {
        // the motor is off, so go into OFF state
        mccState = MCCStates::OFF;
        writeAccOut(0.0);
    } else if (digital_data.park_brake) {
        mccState = MCCStates::PARK;
        writeAccOut(0.0);
    }
    
    // set motor output to 0 if foot brake is being pressed
    if (brake_pressure_telem > BRAKE_SENSOR_THRESHOLD) {
        writeAccOut(0.0);
    }
    // set brakeLED based on analog brake sensor 
    brakeLED = brake_pressure_telem > BRAKE_SENSOR_THRESHOLD;
}   
