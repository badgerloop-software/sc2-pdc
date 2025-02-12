
#include "motor_control.h"

MCCStates state;

volatile MCCStates mccState = MCCStates::OFF;
volatile float speed_pid_compute = 0.0;
STM32TimerInterrupt state_updater(TIM7);

// set default state to OFF
void MCCState(){ 
    state = MCCStates::OFF;

    //PID interval is in seconds and macro is in microseconds
    power_PID = PID(POWER_D_PARAM, POWER_I_PARAM, POWER_P_PARAM, PID_UPDATE_INTERVAL);

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
    switch (state) {
        case MCCStates::PARK:
            if (!digital_data.park_brake) {
                state = MCCStates::IDLE;
            }
            // OUTPUT: make sure the motor isn't spinning when it's in PARK
            writeAccOut(0.0);
            break;

        case MCCStates::IDLE:
            // OUTPUT: set acc_out pin based on acc_in from the pedal
            // this allows us to accelerate into REVERSE or FORWARD states
            writeAccOut(acceleratorPedal);
            set_direction(digital_data.direction);

            // car moving fast, transition to locked direction state
            if (rpm >= MIN_MOVING_SPEED) {
                if (digital_data.forwardAndReverse == FORWARD_VALUE) {
                    state = MCCStates::FORWARD;
                } else {
                    state = MCCStates::REVERSE;
                }
            } 
            break;

        case MCCStates::REVERSE:
            if (rpm < MIN_MOVING_SPEED) {
                state = MCCStates::IDLE;
            }
            // OUTPUT: set acc_out pin based on acc_in from the pedal
            setAccOut(acceleratorPedal);
            break;

        case MCCStates::FORWARD:
            if (rpm < MIN_MOVING_SPEED) {
                state = MCCStates::IDLE;

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
            setAccOut(acceleratorPedal);
            break;

        case MCCStates::CRUISE_POWER:
            if (cruzMode != CRUZ_MODE::POWER) {
                state = MCCStates::FORWARD;
                break;
            } 
            // TODO: not enough stuff for power right now (10/22)
            // get current motor power
            // set setPoint to target power
            // setAccOut(compute());
            break;

        case MCCStates::CRUISE_SPEED:
            if (cruzMode != CRUZ_MODE::SPEED) {
                state = MCCStates::FORWARD;
                break;
            } 
            // set current rpm for speed_PID
            speed_PID.setProcessValue(rpm);
            // set target for speed_PID
            speed_PID.setSetPoint(motorSpeedSetpoint);
            speed_pid_compute = speed_PID.compute();
            // set accelerator 
            setAccOut(speed_pid_compute);
            break;

        // OFF state as our default
        default:
            if (digital_data.motorPower) {
                state = MCCStates::PARK;
                break;
            }
            // OUTPUT: make sure the motor isn't spinning when it's OFF
            setAccOut(0.0);
            // Set to known state since it is our default
            state = MCCStates::OFF;
            break;
    }

    // STATE MACHINE OVERRIDES AND SAFETY CHECKS

    // use if, else if because we should only switch to 1 state.
    // put higher priority checks up top.
    if (!digital_data.motorPower) {
        // the motor is off, so go into OFF state
        state = MCCStates::OFF;
        setAccOut(0.0);
    } else if (digital_data.parkBrake) {
        state = MCCStates::PARK;
        setAccOut(0.0);
    }
    
    // set motor output to 0 if foot brake is being pressed
    if (brakeSensor > BRAKE_SENSOR_THRESHOLD) {
        setAccOut(0.0);
    }
    // set brakeLED based on analog brake sensor 
    setBrakeLEDOutput(brakeSensor > BRAKE_SENSOR_THRESHOLD);

    mccState = this->state; // used to send state over CAN
}

// returns the current state of the state machine
MCCStates get_state() {
    return state;
}
