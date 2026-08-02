#include "debug.h"

#if SC2_DEBUG

#include <Arduino.h>

#include "board_config.h"
#include "can_pdc.h"
#include "io_management.h"
#include "motor_control.h"

// ------------- LOCAL -------------

static int debugCounter;

// ------------- LOCAL FUNCTIONS -------------

static const char* pdcStateToString(PDCStates state) {
    switch (state) {
        case PDCStates::OFF:
            return "OFF";
        case PDCStates::PARK:
            return "PARK";
        case PDCStates::IDLE:
            return "IDLE";
        case PDCStates::FORWARD:
            return "FORWARD";
        case PDCStates::REVERSE:
            return "REVERSE";
        default:
            return "UNKNOWN";
    }
}

static void debugPrintState() {
    Serial.printf(
        "state=%s can_dir=%s fr=%u acc_in_raw=%u "
        "acc_in=%.3f acc_out=%.3f regen=%.3f rpm=%.1f mph=%.1f "
        "brake=%u brake_v=%.3f park=%u mcu_dir=%u eco=%u brake_led=%u\n",
        pdcStateToString(get_state()), forwardAndReverse == FORWARD_VALUE ? "Fwd" : "Rev",
        forwardAndReverse, acc_in_raw, acc_in, acc_out, regen_brake, rpm, mph, brake_pressed,
        brake_pressure_telem, digital_data.park_brake, digital_data.direction, digital_data.eco_mode,
        digital_data.brake_led);
}

// ------------- PUBLIC FUNCTIONS -------------

void debugInit() {
    Serial.begin(115200);
    Serial.printf("Starting up...\n");
}

void debugUpdate() {
    if (debugCounter >= (1000 / DATA_SEND_PERIOD)) {
        debugPrintState();
        debugCounter = 0;
    }
    debugCounter++;
}

void debugError(const char* msg) {
    printf("%s\n", msg);
}

#endif  // SC2_DEBUG
