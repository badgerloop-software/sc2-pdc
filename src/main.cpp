#include <Arduino.h>
#include "IOManagement.h"
#include "const.h"
#include "canPDC.h"

void testLoop();
void testSetup();
void testDigitalInputs();
void testDigitalOutputs();
void testAnalogInputs();
void testAnalogOutputs();

CANPDC canBus(CAN1, DEF);

void setup() {
  Serial.begin(115200);
  while (!Serial);
  Serial.println("Serial initialized!");
  initIO();
  testSetup();
}

void loop() {
  canBus.sendPDCData();
  canBus.runQueue(DATA_SEND_PERIOD);
  testLoop();
}
 
void testDigitalInputs() {
  Serial.println("TESTING DIGITAL INPUTS");
  
  // PA8 -> D9
  Serial.print("MCU_SPEED_SIG (PA8): ");
  Serial.println(digitalRead(MCU_SPEED_SIG));
  
  // PB4 -> D12
  Serial.print("PRK_BRK_TELEM (PB4): ");
  Serial.println(digitalRead(PRK_BRK_TELEM));
}

void testDigitalOutputs() {
  Serial.println("TESTING DIGITAL OUTPUTS");
  
  // PA6 -> A5
  set_direction(HIGH);
  Serial.println("MCU_DIR (PA6) HIGH");
  delay(5000);

  set_direction(LOW);
  Serial.println("MCU_DIR (PA6) LOW");
  delay(5000);

  // PB1 -> D6
  set_eco_mode(HIGH);
  Serial.println("MCU_ECO (PB1) HIGH");
  delay(5000);

  set_eco_mode(LOW);
  Serial.println("MCU_ECO (PB1) LOW");
  delay(5000);
}

void testAnalogInputs() {
  Serial.println("TESTING ANALOG INPUTS");
  
  float lv12V = readADC(ADC_CHANNEL_6) * 3.3;
  float lv5V = readADC(ADC_CHANNEL_12) * 3.3;
  float lv5V_current = readADC(ADC_CHANNEL_15) * 3.3;
  float current_in = readADC(ADC_CHANNEL_8) * 3.3;
  float brake_pressure = readADC(ADC_CHANNEL_5) * 3.3;

  Serial.print("LV_12V (ADC6): ");
  Serial.print(lv12V);
  Serial.println("V");

  Serial.print("LV_5V (ADC12): ");
  Serial.print(lv5V);
  Serial.println("V");

  Serial.print("LV_5V Current (ADC15): ");
  Serial.print(lv5V_current);
  Serial.println("V");

  Serial.print("Current In (ADC8): ");
  Serial.print(current_in);
  Serial.println("V");

  Serial.print("Brake Pressure (ADC5): ");
  Serial.print(brake_pressure);
  Serial.println("V");
}

void testAnalogOutputs() {
  Serial.println("TESTING ANALOG OUTPUTS");

  // DAC2 -> A4
  Serial.println("Acc_Out (DAC2) 0V");
  writeAccOut(0.0);
  delay(5000);
  
  Serial.println("Acc_Out (DAC2) 1.65V");
  writeAccOut(0.5);
  delay(5000);
  
  Serial.println("Acc_Out (DAC2) 3.3V");
  writeAccOut(1.0);
  delay(5000);

  // DAC1 -> A3
  Serial.println("Regen_Brake (DAC1) 0.0V");
  writeRegenBrake(0.0);
  delay(5000);
  
  Serial.println("Regen_Brake (DAC1) 1.65V");
  writeRegenBrake(0.5);
  delay(5000);
  
  Serial.println("Regen_Brake (DAC1) 3.3V");
  writeRegenBrake(1.0);
  delay(5000);
}

enum TestMode {
  TEST_DIGITAL_INPUTS = 1,
  TEST_DIGITAL_OUTPUTS = 2,
  TEST_ANALOG_INPUTS = 3,
  TEST_ANALOG_OUTPUTS = 4
};

void testSetup() {
  initIO();
  Serial.println("1 - Digital Inputs");
  Serial.println("2 - Digital Outputs");
  Serial.println("3 - Analog Inputs");
  Serial.println("4 - Analog Outputs");
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





