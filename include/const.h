#ifndef __CONST_H__
#define __CONST_H__

/**
 * Debugging Techniques:
 * 1. Send random data over CAN and verify if you received the same random data
 */

// Uncomment when building for the can-bounce test board
// #define TEST_MODE

// Uncomment to enable Serial debug prints/comment for production
// to save CPU cycles and reduce latency
#define DEBUG_PRINTS

// Timing constants
#define COUNTER_EXP 2
#define IO_UPDATE_PERIOD 50000 // us
#define DATA_SEND_PERIOD 50     // CAN TX period (ms); also debug print period

// Analog scaling
#define INA180_CURRENT_MULTIPLIER 10 // multiplies analog current telem to convert it into a readable value

// State machine
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

#define MIN_MOVING_SPEED 3.0 // rpm threshold to leave IDLE

// Brake
#define REGEN_BRAKE_LIGHT_THRESHOLD 0.05f // 5% regen turns brake lights on

// Stable IO samples -> (IO_UPDATE_PERIOD = 50 ms) before brake state changes
#define BRAKE_DEBOUNCE_PRESS_SAMPLES 3   // ~150 ms to assert
#define BRAKE_DEBOUNCE_RELEASE_SAMPLES 2 // ~100 ms to release

#endif
