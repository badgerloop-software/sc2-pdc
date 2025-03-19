
#include "motor_control.h"

volatile PDCStates pdcState = PDCStates::OFF;
volatile CRUZ_MODE cruzMode = CRUZ_MODE::OFF;

//PID interval is in seconds and macro is in microseconds
PID power_PID(POWER_D_PARAM, POWER_I_PARAM, POWER_P_PARAM, PID_UPDATE_INTERVAL);
PID speed_PID(SPEED_D_PARAM, SPEED_I_PARAM, SPEED_P_PARAM, PID_UPDATE_INTERVAL);

volatile float speed_pid_compute = 0.0;
STM32TimerInterrupt state_updater(TIM7);

volatile float motorSpeedSetpoint;

// set default state to OFF
void initPDCState(){ 
    pdcState = PDCStates::OFF;

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
    switch (pdcState) {
        case PDCStates::PARK:
            if (!digital_data.park_brake) {
                pdcState = PDCStates::IDLE;
            }
            // OUTPUT: make sure the motor isn't spinning when it's in PARK
            writeAccOut(0.0);
            break;

        case PDCStates::IDLE:
            // OUTPUT: set acc_out pin based on acc_in from the pedal
            // this allows us to accelerate into REVERSE or FORWARD states
            writeAccOut(acc_in);
            set_direction(digital_data.direction);

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
            writeAccOut(acc_in);
            break;

        case PDCStates::FORWARD:
            if (rpm < MIN_MOVING_SPEED) {
                pdcState = PDCStates::IDLE;

            // Cruise PID will not be tuned for comp. Disable
            } /* else if (digital_data.cruiseEnabled) {
                if (cruzMode == CRUZ_MODE::POWER) {
                    curr_PID = &power_PID;
                    state = PDCStates::CRUISE_POWER;
                } else if (cruzMode == CRUZ_MODE::SPEED) {
                    curr_PID = &speed_PID;
                    state = PDCStates::CRUISE_SPEED;
                }
            } */
            
            // OUTPUT: set acc_out pin based on acc_in from the pedal
            writeAccOut(acc_in);
            break;

        case PDCStates::CRUISE_POWER:
            if (cruzMode != CRUZ_MODE::POWER) {
                pdcState = PDCStates::FORWARD;
                break;
            } 
            // TODO: not enough stuff for power right now (10/22)
            // get current motor power
            // set setPoint to target power
            // setAccOut(compute());
            break;

        case PDCStates::CRUISE_SPEED:
            if (cruzMode != CRUZ_MODE::SPEED) {
                pdcState = PDCStates::FORWARD;
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
                pdcState = PDCStates::PARK;
                break;
            }
            // OUTPUT: make sure the motor isn't spinning when it's OFF
            writeAccOut(0.0);
            // Set to known state since it is our default
            pdcState = PDCStates::OFF;
            break;
    }

    // STATE MACHINE OVERRIDES AND SAFETY CHECKS

    // use if, else if because we should only switch to 1 state.
    // put higher priority checks up top.
    if (!mc_on) {
        // the motor is off, so go into OFF state
        pdcState = PDCStates::OFF;
        writeAccOut(0.0);
    } else if (digital_data.park_brake) {
        pdcState = PDCStates::PARK;
        writeAccOut(0.0);
    }
    
    // set motor output to 0 if foot brake is being pressed
    if (brake_pressure_telem > BRAKE_SENSOR_THRESHOLD) {
        writeAccOut(0.0);
    }
    // set brakeLED based on analog brake sensor 
    brakeLED = brake_pressure_telem > BRAKE_SENSOR_THRESHOLD;
}   
