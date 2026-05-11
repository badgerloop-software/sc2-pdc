#ifndef __CONST_H__
#define __CONST_H__

/**
 * Debugging Techniques:
 * 1. Send random data over CAN and verify if you received the same random data
 */

// Uncomment when building for the can-bounce test board.
// When defined, GPIO reads for park_brake and mcu_mc_on are skipped
// so CAN-sourced values from the test board are used instead.
// #define TEST_MODE

// Uncomment to enable Serial debug prints. Comment out for production
// to save CPU cycles and reduce latency.
// #define DEBUG_PRINTS

#define COUNTER_EXP 2

#define IO_UPDATE_PERIOD 100000 // us
#define PID_UPDATE_INTERVAL                                                    \
  (IO_UPDATE_PERIOD /                                                          \
   1000000.0f) // sec, float division to avoid truncation to 0

#define INA180_CURRENT_MULTIPLIER                                              \
  10 // multiplies analog current telem to convert it into a readable value

// How fast to transmit data over CAN in ms (and debug prints if on)
#define DATA_SEND_PERIOD 50

///////////////
// PID macros
///////////////
// Todo: tune these
#define POWER_P_PARAM 0.0
#define POWER_I_PARAM 0.0
#define POWER_D_PARAM 0.0
#define SPEED_P_PARAM 2.5
#define SPEED_I_PARAM 0.0
#define SPEED_D_PARAM 0.0

// limits for power and speed PIDs.
#define MIN_POWER 0.0
#define MAX_POWER 1.0
#define MIN_RPM 0.0
#define MAX_RPM 1000.0

// limits for outputs of PID
// is 0.0 to 1.0 due to how AnalogOut pins work.
#define MIN_OUT 0.0
#define MAX_OUT 1.0

//////////////////////
// State machine stuff
/////////////////////
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

extern volatile CRUZ_MODE cruzMode;
extern volatile PDCStates pdcState;

#define FORWARD_VALUE 1
#define REVERSE_VALUE 0
#define BRAKE_SENSOR_THRESHOLD 0.5 // 0.14 resting and 0.99 fully pressed

///////////////
// Speed stuff
///////////////

#define MIN_MOVING_SPEED 3.0 // speed threshold for idle state

#endif