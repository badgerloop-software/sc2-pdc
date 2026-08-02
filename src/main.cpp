#include <Arduino.h>

#include <stdlib.h>

#include "adc.h"
#include "board_config.h"
#include "can_pdc.h"
#include "debug.h"
#include "io_management.h"
#include "motor_control.h"
#include "speed_calc.h"

// DEBUG_TECHNIQUE: 0 production, 1 random CAN echo for bus testing
#define DEBUG_TECHNIQUE 0

// ------------- LOCAL -------------

static CanPdc canBus(CAN1, DEF);

#if DEBUG_TECHNIQUE == 1

static int counter = 0;
static void randomizeData();
static void debugPrintRandom();

static void randomizeData() {
    acc_out = ((float)rand() / RAND_MAX);
    regen_brake = ((float)rand() / RAND_MAX);
    lv_12V_telem = ((float)rand() / RAND_MAX) * 12.0;
    lv_5V_telem = ((float)rand() / RAND_MAX) * 5.0;
    lv_5V_current = ((float)rand() / RAND_MAX) * 3.3;
    current_in_telem = ((float)rand() / RAND_MAX) * 3.3;
    brake_pressed = rand() % 2;
    digital_data.brake_led = brake_pressed;
    digital_data.direction = rand() % 2;
    digital_data.mc_speed_sig = rand() % 2;
    digital_data.eco_mode = rand() % 2;
    digital_data.mcu_mc_on = rand() % 2;
    digital_data.park_brake = rand() % 2;
}

static void debugPrintRandom() {
#if SC2_DEBUG
    Serial.printf("acc_in: %f\n", acc_in);
    Serial.printf("acc_out: %f\n", acc_out);
    Serial.printf("regen_brake: %f\n", regen_brake);
    Serial.printf("lv_12V_telem: %f\n", lv_12V_telem);
    Serial.printf("lv_5V_telem: %f\n", lv_5V_telem);
    Serial.printf("lv_5V_current: %f\n", lv_5V_current);
    Serial.printf("current_in_telem: %f\n", current_in_telem);
    Serial.printf("brake_pressed: %i\n", brake_pressed);
    Serial.printf("brakeLED: %i\n", digital_data.brake_led);
    Serial.printf("digital_data.direction: %i\n", digital_data.direction);
    Serial.printf("digital_data.mc_speed_sig: %i\n", digital_data.mc_speed_sig);
    Serial.printf("digital_data.eco_mode: %i\n", digital_data.eco_mode);
    Serial.printf("digital_data.mcu_mc_on: %i\n", digital_data.mcu_mc_on);
    Serial.printf("digital_data.park_brake: %i\n", digital_data.park_brake);
#endif
}
#endif

// ------------- PUBLIC FUNCTIONS -------------

void setup() {
    debugInit();
    digitalWrite(PB6, HIGH);
    initIO();
    startSpeedCalculation();

#if DEBUG_TECHNIQUE == 0
    initPDCState();
#elif DEBUG_TECHNIQUE == 1
    srand(millis());
    randomizeData();
#endif
}

void loop() {
#if DEBUG_TECHNIQUE == 0
    debugUpdate();
#elif DEBUG_TECHNIQUE == 1
    if (counter >= COUNTER_EXP) {
        randomizeData();
        debugPrintRandom();
        counter = 0;
#if SC2_DEBUG
        Serial.printf("Received forwardAndReverse: %i\n", forwardAndReverse);
#endif
    }
    counter++;
#endif

    canBus.sendPDCData();
    canBus.runQueue(DATA_SEND_PERIOD);
}
