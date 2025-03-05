#include <Arduino.h>
#include "IOManagement.h"
#include "const.h"
#include "canPDC.h"

CANPDC canBus(CAN1, DEF);

void setup() {
  Serial.begin(115200);
  initIO();
}

void loop() {
  canBus.sendPDCData();
  canBus.runQueue(DATA_SEND_PERIOD);
}
 
void testDigitalInputs() {
  printf("TESTING DIGITAL INPUTS\n");
  
  // PA8 -> D9
  printf("MCU_SPEED_SIG (PA8): %d\n", digitalRead(MCU_SPEED_SIG));
  // PB4 -> D12
  printf("PRK_BRK_TELEM (PB4): %d\n", digitalRead(PRK_BRK_TELEM));
}

void testDigitalOutputs() {
  printf("TESTING DIGITAL OUTPUTS\n");
  
  // PA6 -> A5
  set_direction(HIGH);
  printf("MCU_DIR (PA6) HIGH\n");
  delay(5000);

  set_direction(LOW);
  printf("MCU_DIR (PA6) LOW\n");
  delay(5000);

  // PB1 -> D6
  set_eco_mode(HIGH);
  printf("MCU_ECO (PB1) HIGH\n");
  delay(5000);

  set_eco_mode(LOW);
  printf("MCU_ECO (PB1) LOW\n");
  delay(5000);
}

void testAnalogInputs() {
  printf("TESTING ANALOG INPUTS\n");
  
  float lv12V = readADC(ADC_CHANNEL_6) * 3.3;
  float lv5V = readADC(ADC_CHANNEL_12) * 3.3;
  float lv5V_current = readADC(ADC_CHANNEL_15) * 3.3;
  float current_in = readADC(ADC_CHANNEL_8) * 3.3;
  float brake_pressure = readADC(ADC_CHANNEL_5) * 3.3;

  // ADC6 -> A1
  printf("LV_12V (ADC6): %.2fV\n", lv12V);
  // ADC12 -> A6
  printf("LV_5V (ADC12): %.2fV\n", lv5V);
  // ADC15 -> D3
  printf("LV_5V Current (ADC15): %.2fV\n", lv5V_current);
  // ADC8 -> A2
  printf("Current In (ADC8): %.2fV\n", current_in);
  // ADC5 -> A0
  printf("Brake Pressure (ADC5): %.2fV\n", brake_pressure);
}

void testAnalogOutputs() {
  printf("TESTING ANALOG OUTPUTS\n");

  // DAC2 -> A4
  printf("Acc_Out (DAC2) 0V\n");
  writeAccOut(0.0);
  delay(5000);
  
  printf("Acc_Out (DAC2) 1.65V\n");
  writeAccOut(0.5);
  delay(5000);
  
  printf("Acc_Out (DAC2) 3.3V\n");
  writeAccOut(1.0);
  delay(5000);

  // DAC1 -> A3
  printf("Regen_Brake (DAC1) 0.0V\n");
  writeRegenBrake(0.0);
  delay(5000);
  
  printf("Regen_Brake (DAC1) 1.65V\n");
  writeRegenBrake(0.5);
  delay(5000);
  
  printf("Regen_Brake (DAC1) 3.3V\n");
  writeRegenBrake(1.0);
  delay(5000);
}

  enum TestMode {
  TEST_DIGITAL_INPUTS = 1,
  TEST_DIGITAL_OUTPUTS = 2,
  TEST_ANALOG_INPUTS = 3,
  TEST_ANALOG_OUTPUTS = 4
};

TestMode currentTest = TEST_DIGITAL_INPUTS;  // Default test mode

void testSetup() {
  initIO();
  printf("1 - Digital Inputs\n");
  printf("2 - Digital Outputs\n");
  printf("3 - Analog Inputs\n");
  printf("4 - Analog Outputs\n");
}

void testLoop() {
  if (Serial.available() > 0) {
      char input = Serial.read();  
      if (input == '1') {
          testDigitalInputs();
      } 
      else if (input == '2') {
          testDigitalOutputs();
      } 
      else if (input == '3') {
          testAnalogInputs();
      } 
      else if (input == '4') {
          testAnalogOutputs();
      } 
  }
}




