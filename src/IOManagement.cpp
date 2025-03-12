#include "IOManagement.h"

volatile Digital_Data digital_data;

volatile float acc_in = 0;
volatile float acc_out = 0;
volatile float regen_brake = 0;
volatile float lv_12V_telem = 0;
volatile float lv_5V_telem = 0;
volatile float lv_5V_current = 0;
volatile float current_in_telem = 0;
volatile float brake_pressure_telem = 0;

// Ticker to poll input readings at fixed rate
STM32TimerInterrupt IOTimer(TIM2);

void initIO() {
    pinMode(MCU_DIR, OUTPUT);
    pinMode(MCU_ECO, OUTPUT);
    pinMode(MC_ON, INPUT);
    pinMode(PRK_BRK_TELEM, INPUT);

    initADC(ADC1);

    if(IOTimer.attachInterruptInterval(IO_UPDATE_PERIOD, readIO))
    {
        printf("starting IO timer\n");
    }
    else 
    {
        printf("problem starting IO timer\n");
    }
}

void readIO() {
    digital_data.mc_on = digitalRead(MC_ON);
    digital_data.park_brake = digitalRead(PRK_BRK_TELEM);

    acc_in = readADC(ADC_CHANNEL_11); // PA_6
    lv_12V_telem = readADC(ADC_CHANNEL_6) * 3.3 *  35.1/5.1; // PA_1
    lv_5V_telem = readADC(ADC_CHANNEL_12) * 3.3 * 15.1/5.1; // PA_7
    lv_5V_current = readADC(ADC_CHANNEL_15) * INA180_CURRENT_MULTIPLIER; // PB_0
    current_in_telem = readADC(ADC_CHANNEL_8) * INA180_CURRENT_MULTIPLIER; // PA_3
    brake_pressure_telem = readADC(ADC_CHANNEL_5); // PA_0
}

void set_direction(bool dir){
    digitalWrite(MCU_DIR, dir);
    digital_data.direction = dir;
}

void set_eco_mode(bool eco){
    digitalWrite(MCU_ECO, eco);
    digital_data.eco_mode = eco;
}

void writeAccOut(float newAccOut) {
    acc_out = newAccOut;
    analogWrite(PA5, acc_out);
}

void writeRegenBrake(float newRegenBrake) {
    regen_brake = newRegenBrake;
    analogWrite(PA4, regen_brake);
}
