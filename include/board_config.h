#ifndef __BOARD_CONFIG_H__
#define __BOARD_CONFIG_H__

#include "adc.h"

// ------------- TIMING -------------
#define IO_UPDATE_PERIOD 50000  // us
#define PID_UPDATE_INTERVAL ((float)IO_UPDATE_PERIOD / 1000000.0f)
#define DATA_SEND_PERIOD 50  // ms, CAN TX (+ debug if on)
#define COUNTER_EXP 2

// ------------- DEBUG -------------
// 0 off (not in flash) | 1 human
#define SC2_DEBUG 0

// ------------- TEST -------------
// Uncomment for can-bounce test board (park_brake from CAN).
// #define TEST_MODE

// ------------- IO PINS -------------
#define MCU_DIR PB7
#define MCU_ECO PB1
#define MCU_MC_ON PA10
#define MCU_SPEED_SIG PA8
#define PRK_BRK_TELEM PB4
#define BRAKE_TELEM PA0

#define INA180_CURRENT_MULTIPLIER 10

// ------------- PID (reserved) -------------
#define POWER_P_PARAM 0.0
#define POWER_I_PARAM 0.0
#define POWER_D_PARAM 0.0
#define SPEED_P_PARAM 2.5
#define SPEED_I_PARAM 0.0
#define SPEED_D_PARAM 0.0

#define MIN_POWER 0.0
#define MAX_POWER 1.0
#define MIN_RPM 0.0
#define MAX_RPM 1000.0
#define MIN_OUT 0.0
#define MAX_OUT 1.0

// ------------- STATE MACHINE -------------
enum class PDCStates : uint8_t {
    OFF,
    PARK,
    IDLE,
    FORWARD,
    REVERSE,
};

extern volatile PDCStates pdcState;

#define FORWARD_VALUE 0
#define REVERSE_VALUE 1

// ------------- SPEED / BRAKE -------------
#define MIN_MOVING_SPEED 3.0  // mph threshold for idle state
#define REGEN_BRAKE_LIGHT_THRESHOLD 0.05f
#define BRAKE_DEBOUNCE_PRESS_SAMPLES 3    // ~150 ms at 50 ms IO period
#define BRAKE_DEBOUNCE_RELEASE_SAMPLES 2  // ~100 ms
#define BRAKE_ADC_CHANNEL ADC_CHANNEL_5
#define BRAKE_PRESSURE_THRESHOLD_V 1.65f

#define SPEED_CALC_INTERVAL_US 50000  // us
#define ARRAY_SIZE 20
#define PULSES_PER_REV 48
#define WHEEL_CIRCUMFERENCE 69  // inches

#endif  // __BOARD_CONFIG_H__
