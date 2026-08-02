#include "io_management.h"

#include "debug.h"

// ------------- GLOBALS -------------

volatile Digital_Data digital_data;

volatile uint16_t acc_in_raw = 0;
volatile float acc_in = 0;
volatile float acc_out = 0;
volatile float regen_in = 0;
volatile float regen_brake = 0;
volatile float lv_12V_telem = 0;
volatile float lv_5V_telem = 0;
volatile float lv_5V_current = 0;
volatile float current_in_telem = 0;
volatile bool brake_pressed = false;
volatile float brake_pressure_telem = 0.0f;
volatile float mph = 0;
volatile float rpm = 0;

// ------------- LOCAL -------------

static STM32TimerInterrupt IOTimer(TIM7);

static volatile uint32_t pulseCount = 0;

static bool brake_last_raw = false;
static uint8_t brake_stable_count = 0;

static DAC_HandleTypeDef hdac;

// ------------- LOCAL FUNCTIONS -------------

static void speedPulseISR() {
    pulseCount++;
}

static void initDAC() {
    __HAL_RCC_DAC1_CLK_ENABLE();

    hdac.Instance = DAC1;
    HAL_DAC_Init(&hdac);

    DAC_ChannelConfTypeDef config = {0};
    config.DAC_Trigger = DAC_TRIGGER_NONE;
    config.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE;
    HAL_DAC_ConfigChannel(&hdac, &config, DAC_CHANNEL_1);
    HAL_DAC_ConfigChannel(&hdac, &config, DAC_CHANNEL_2);

    HAL_DAC_Start(&hdac, DAC_CHANNEL_1);
    HAL_DAC_Start(&hdac, DAC_CHANNEL_2);

    // PA5 default SPI SCK; force analog so DAC output is not overridden.
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    __HAL_RCC_GPIOA_CLK_ENABLE();
    GPIO_InitStruct.Pin = GPIO_PIN_5;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

// ------------- PUBLIC FUNCTIONS -------------

void initSpeedCounter() {
    attachInterrupt(digitalPinToInterrupt(MCU_SPEED_SIG), speedPulseISR, RISING);
}

void initIO() {
    pinMode(MCU_DIR, OUTPUT);
    set_direction(FORWARD_VALUE);
    pinMode(MCU_ECO, OUTPUT);
    pinMode(MCU_MC_ON, INPUT);
    pinMode(MCU_SPEED_SIG, INPUT);
    pinMode(PRK_BRK_TELEM, INPUT);
    pinMode(BRAKE_TELEM, INPUT_PULLDOWN);

    initDAC();
    initADC(ADC1);
    initSpeedCounter();

    if (!IOTimer.attachInterruptInterval(IO_UPDATE_PERIOD, readIO)) {
        debugError("ERROR: IO timer");
    }
}

void readIO() {
    uint32_t count = pulseCount;
    pulseCount = 0;
    (void)count;

    digital_data.mc_speed_sig = digitalRead(MCU_SPEED_SIG);

#ifndef TEST_MODE
    digital_data.park_brake = false;
#endif

    lv_12V_telem = readADC(ADC_CHANNEL_6) * 3.3 * 35.1 / 5.1;
    lv_5V_telem = readADC(ADC_CHANNEL_12) * 3.3 * 15.1 / 5.1;
    lv_5V_current = readADC(ADC_CHANNEL_15) * INA180_CURRENT_MULTIPLIER;
    current_in_telem = readADC(ADC_CHANNEL_8) * INA180_CURRENT_MULTIPLIER;

#ifndef TEST_MODE
    {
        bool raw = digitalRead(BRAKE_TELEM) == HIGH;
        if (raw != brake_last_raw) {
            brake_stable_count = 0;
            brake_last_raw = raw;
        } else if (raw != brake_pressed) {
            uint8_t need =
                raw ? BRAKE_DEBOUNCE_PRESS_SAMPLES : BRAKE_DEBOUNCE_RELEASE_SAMPLES;
            if (++brake_stable_count >= need) {
                brake_pressed = raw;
                brake_stable_count = 0;
            }
        }
    }
    brake_pressure_telem = brake_pressed ? 3.3f : 0.0f;
    digital_data.brake_led = brake_pressed || (regen_in >= REGEN_BRAKE_LIGHT_THRESHOLD);
#endif
}

void set_direction(bool dir) {
    digitalWrite(MCU_DIR, dir);
    digital_data.direction = dir;
}

void set_eco_mode(bool eco) {
    digitalWrite(MCU_ECO, eco);
    digital_data.eco_mode = eco;
}

void writeAccOut(float newAccOut) {
    if (newAccOut < 0.0f) {
        newAccOut = 0.0f;
    } else if (newAccOut > 1.0f) {
        newAccOut = 1.0f;
    }

    acc_out = newAccOut;
    HAL_DAC_SetValue(&hdac, DAC_CHANNEL_2, DAC_ALIGN_12B_R, (uint32_t)(acc_out * 4095.0f));
}

void writeRegenBrake(float newRegenBrake) {
    if (newRegenBrake < 0.0f) {
        newRegenBrake = 0.0f;
    } else if (newRegenBrake > 1.0f) {
        newRegenBrake = 1.0f;
    }

    regen_brake = newRegenBrake;
    HAL_DAC_SetValue(&hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_R, (uint32_t)(regen_brake * 4095.0f));
}
