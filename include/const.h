#ifndef __CONST_H__
#define __CONST_H__

/**
 * Debugging Techniques:
 * 1. Send random data over CAN and verify if you received the same random data
*/
#define DEBUG_TECHNIQUE 1

#define IO_UPDATE_PERIOD 100000 // us
#define PID_UPDATE_INTERVAL IO_UPDATE_PERIOD/1000000 // sec

#define INA180_CURRENT_MULTIPLIER 10 // multiplies analog current telem to convert it into a readable value

// How fast to transmit data over CAN in ms (and debug prints if on) 
#define DATA_SEND_PERIOD 50

// PID macros
#define POWER_P_PARAM 0.0 // TODO: tune these
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
// is 0.0 to 1.0 due to how analog output pins work.
#define MIN_OUT 0.0
#define MAX_OUT 1.0

// State machine constants
#define FORWARD_VALUE 0
#define REVERSE_VALUE 1
#define BRAKE_SENSOR_THRESHOLD 0.14 // 0.7/5

// Speed constants
#define MIN_MOVING_SPEED 3.0        // speed threshold for idle state

#endif