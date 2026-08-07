// IOManagement: GPIO, ADC telem, and DAC commands to the motor controller
// Shared floats (rpm, mph, acc_in, ...) are the telem bag for CAN and control
// speed_calc writes rpm and mph, canPDC writes acc_in and regen_in
#include "IOManagement.h"

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

// Polls inputs at IO_UPDATE_PERIOD
STM32TimerInterrupt IOTimer(TIM7);

// Sample-and-delay debounce state for brake
static bool brake_last_raw = false;
static uint8_t brake_stable_count = 0;

DAC_HandleTypeDef hdac;

void initDAC() {
    // Two DAC channels: PA4 = regen command, PA5 = accel command (0 to 1)
    __HAL_RCC_DAC1_CLK_ENABLE();
    
    hdac.Instance = DAC1;
    HAL_DAC_Init(&hdac);

    DAC_ChannelConfTypeDef config = {0};
    config.DAC_Trigger = DAC_TRIGGER_NONE;
    config.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE;
    HAL_DAC_ConfigChannel(&hdac, &config, DAC_CHANNEL_1);  // PA4 - regen
    HAL_DAC_ConfigChannel(&hdac, &config, DAC_CHANNEL_2);  // PA5 - accel
    
    HAL_DAC_Start(&hdac, DAC_CHANNEL_1);
    HAL_DAC_Start(&hdac, DAC_CHANNEL_2);

    // PA5 is the default SPI SCK
    // Set PA5 to analog mode so the SPI default does not override the DAC
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    __HAL_RCC_GPIOA_CLK_ENABLE();
    GPIO_InitStruct.Pin  = GPIO_PIN_5;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

void initIO() {
  pinMode(MCU_DIR, OUTPUT);
  set_direction(FORWARD_VALUE);
  pinMode(MCU_ECO, OUTPUT);
  pinMode(MCU_MC_ON, INPUT);
  pinMode(MCU_SPEED_SIG, INPUT);
  pinMode(PRK_BRK_TELEM, INPUT);
  // The PCB has no external pulldown,use the MCU internal pulldown on PA0
  pinMode(BRAKE_TELEM, INPUT_PULLDOWN);

  initDAC();
  initADC(ADC1);

  if (IOTimer.attachInterruptInterval(IO_UPDATE_PERIOD, readIO)) {
#ifdef DEBUG_PRINTS
    printf("starting IO timer\n");
#endif
  } else {
#ifdef DEBUG_PRINTS
    printf("problem starting IO timer\n");
#endif
  }
}

void readIO() {
  digital_data.mc_speed_sig = digitalRead(MCU_SPEED_SIG);

#ifndef TEST_MODE
  // The park-brake sensor is not installed so keep park false
  digital_data.park_brake = false;
#endif

  // acc_in comes from CAN in readHandler (0x302 in production, 0x209 in test)
  lv_12V_telem = readADC(ADC_CHANNEL_6) * 3.3 * 35.1 / 5.1;              // PA_1
  lv_5V_telem = readADC(ADC_CHANNEL_12) * 3.3 * 15.1 / 5.1;              // PA_7
  lv_5V_current = readADC(ADC_CHANNEL_15) * INA180_CURRENT_MULTIPLIER;   // PB_0
  current_in_telem = readADC(ADC_CHANNEL_8) * INA180_CURRENT_MULTIPLIER; // PA_3
#ifndef TEST_MODE
  // Digital brake switch: HIGH = pressed, internal pulldown holds idle at 0
  // Sample-and-delay: require N stable samples before updating brake_pressed
  {
    bool raw = digitalRead(BRAKE_TELEM) == HIGH;
    if (raw != brake_last_raw) {
      brake_stable_count = 0;
      brake_last_raw = raw;
    } else if (raw != brake_pressed) {
      uint8_t need = raw ? BRAKE_DEBOUNCE_PRESS_SAMPLES
                         : BRAKE_DEBOUNCE_RELEASE_SAMPLES;
      if (++brake_stable_count >= need) {
        brake_pressed = raw;
        brake_stable_count = 0;
      }
    }
  }
  brake_pressure_telem = brake_pressed ? 3.3f : 0.0f;
  digital_data.brake_led =
      brake_pressed || (regen_in >= REGEN_BRAKE_LIGHT_THRESHOLD);
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
  // Clamp 0 to 1, then write 12-bit counts to DAC channel 2 (accel)
  if (newAccOut < 0.0f) newAccOut = 0.0f;
  else if (newAccOut > 1.0f) newAccOut = 1.0f;

  acc_out = newAccOut;
  HAL_DAC_SetValue(&hdac, DAC_CHANNEL_2, DAC_ALIGN_12B_R, (uint32_t)(acc_out * 4095.0f));
}

void writeRegenBrake(float newRegenBrake) {
  // Clamp 0 to 1, then write 12-bit counts to DAC channel 1 (regen)
  if (newRegenBrake < 0.0f) newRegenBrake = 0.0f;
  else if (newRegenBrake > 1.0f) newRegenBrake = 1.0f;

  regen_brake = newRegenBrake;
  HAL_DAC_SetValue(&hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_R, (uint32_t)(regen_brake * 4095.0f));
}
