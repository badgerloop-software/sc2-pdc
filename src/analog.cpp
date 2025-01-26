#include "analog.h"

// Ticker to poll input readings at a fixed rate
STM32TimerInterrupt dataUpdater(TIM2);

void initAnalog() {
    initADC(ADC1);

    // start the Ticker to periodically read IO
    if(dataUpdater.attachInterruptInterval(IO_UPDATE_PERIOD, readAnalog)) {
        printf("starting readAnalog timer\n");
    } else {
        printf("problem starting readAnalog timer\n");
    }
}

void readAnalog() {
    lv_12V_telem = readADC(ADC_CHANNEL_6) * 3.3 *  35.1/5.1; // PA_1
    lv_5V_telem = readADC(ADC_CHANNEL_12) * 3.3 * 15.1/5.1; // PA_7
    lv_5V_current = readADC(ADC_CHANNEL_15); // PB_0
    current_in_telem = readADC(ADC_CHANNEL_8); // PA_3
    brake_pressure_telem = readADC(ADC_CHANNEL_5); // PA_0
}

void writeAccOut() {
    analogWrite(PA_5, acc_out);
}

void writeRegenBrake() {
    analogWrite(PA_4, regen_brake);
}