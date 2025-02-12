#ifndef __CONST_H__
#define __CONST_H__

#define IO_UPDATE_PERIOD 100000 // us

#define INA180_CURRENT_MULTIPLIER 10 // multiplies analog current telem to convert it into a readable value

// How fast to transmit data over CAN in ms (and debug prints if on) 
#define DATA_SEND_PERIOD 50

#endif