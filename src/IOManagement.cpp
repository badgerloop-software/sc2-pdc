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
volatile float mph = 0;
volatile float rpm = 0;

// Ticker to poll input readings at fixed rate
STM32TimerInterrupt IOTimer(TIM7);

// RPM pulse counting
// MC_SPEED_SIG fires 48 pulses per revolution.
// readIO() runs every IO_UPDATE_PERIOD µs and computes RPM from the count.
static volatile uint32_t pulseCount = 0;
#define PULSES_PER_REV 48

static void speedPulseISR() { pulseCount++; }

void initSpeedCounter() {
  attachInterrupt(digitalPinToInterrupt(MCU_SPEED_SIG), speedPulseISR, RISING);
}

void initIO() {
  pinMode(MCU_DIR, OUTPUT);
  pinMode(MCU_ECO, OUTPUT);
  pinMode(MCU_MC_ON, INPUT);
  pinMode(MCU_SPEED_SIG, INPUT);
  pinMode(PRK_BRK_TELEM, INPUT);

  initADC(ADC1);
  initSpeedCounter();

  if (IOTimer.attachInterruptInterval(IO_UPDATE_PERIOD, readIO)) {
    printf("starting IO timer\n");
  } else {
    printf("problem starting IO timer\n");
  }
}

void readIO() {
  // Compute RPM from pulse count since last call.
  // IO_UPDATE_PERIOD is in µs, convert to minutes for RPM.
  uint32_t count = pulseCount;
  pulseCount = 0;
  float interval_min = IO_UPDATE_PERIOD / 60000000.0f; // µs to minutes
  // rpm = (float)count / PULSES_PER_REV / interval_min;

  digital_data.mc_speed_sig = digitalRead(MCU_SPEED_SIG);

#ifndef TEST_MODE
  // In production, read mc_on and park_brake from physical GPIO.
  // In TEST_MODE these are sourced from CAN (0x300 byte 1) via readHandler.
  digital_data.mcu_mc_on = digitalRead(MCU_MC_ON);
  digital_data.park_brake = digitalRead(PRK_BRK_TELEM);
#endif

#ifndef TEST_MODE
  // In production, read acc_in from the physical pedal on PA_6.
  // In TEST_MODE, acc_in is sourced from CAN (0x209) via readHandler.
  acc_in = readADC(ADC_CHANNEL_11); // PA_6
#endif
  lv_12V_telem = readADC(ADC_CHANNEL_6) * 3.3 * 35.1 / 5.1;              // PA_1
  lv_5V_telem = readADC(ADC_CHANNEL_12) * 3.3 * 15.1 / 5.1;              // PA_7
  lv_5V_current = readADC(ADC_CHANNEL_15) * INA180_CURRENT_MULTIPLIER;   // PB_0
  current_in_telem = readADC(ADC_CHANNEL_8) * INA180_CURRENT_MULTIPLIER; // PA_3
#ifndef TEST_MODE
  brake_pressure_telem = readADC(ADC_CHANNEL_5); // PA_0
#endif
  // In TEST_MODE, brake_pressure_telem stays at 0 (its init value) so floating
  // ADC noise on the disconnected sensor doesn't trigger the safety override.
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
  acc_out = newAccOut;
  writeDAC(PA_5, acc_out);
}

void writeRegenBrake(float newRegenBrake) {
  regen_brake = newRegenBrake;
  writeDAC(PA_4, regen_brake);
}
