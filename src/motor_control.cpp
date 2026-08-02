#include "motor_control.h"

#include "can_pdc.h"
#include "io_management.h"
#include "speed_calc.h"

// ------------- GLOBALS -------------

volatile PDCStates pdcState = PDCStates::OFF;

// ------------- LOCAL -------------

static STM32TimerInterrupt state_updater(TIM2);

// ------------- PUBLIC FUNCTIONS -------------

void initPDCState() {
    pdcState = PDCStates::PARK;
    state_updater.attachInterruptInterval(IO_UPDATE_PERIOD, transition);
    set_eco_mode(true);
    set_direction(FORWARD_VALUE);
}

PDCStates get_state() {
    return pdcState;
}

void transition() {
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
