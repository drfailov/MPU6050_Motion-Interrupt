/* Arduino and MPU6050 MotionDetect interrupt test
 *  As I don't find any working sample code for MPU6050 to generate interrupts when moved,
 *  I dig into datasheets ind wrote my own code. I just want to share my project.
 *  
 *  Tested with: Arduino Nano, gy-521 module
 *  Wiring:
 *  MPU6050 VCC       -> Arduino VCC
 *  MPU6050 GND       -> Arduino GND
 *  MPU6050 SDA       -> Arduino A4
 *  MPU6050 SCL       -> Arduino A5
 *  MPU6050 INT       -> Arduino D3
 * 
 * 
 * Info is got from 
 * MPU-6000/MPU-6050 Register Map and Descriptions
 * Document Number: RM-MPU-6000A-00
 * Revision: 3.2
 * Release Date: 11/14/2011
 * 
 * And
 * 
 * MPU-6000/MPU-6050 Product Specification
 * Document Number: PS-MPU-6000A-00
 * Revision: 3.3
 * Release Date: 5/16/2012
 * 
 * Section 8.1 of Specification: Motion Interrupt
 * 
 * Newer releases of datasheet and register map got removed 
 * any motion interrupt-related sections for some reason,
 * so I used older versions of documents to write this code.
*/
#include "Wire.h"

#define pinBuzzer 7
//when nothing connected to AD0 than address is 0x68
#define MPU6050_ADDRESS 0x68 //can also be 0x69


void writeByte(uint8_t address, uint8_t subAddress, uint8_t data) {
  Wire.begin();
  Wire.beginTransmission(address); // Initialize the Tx buffer
  Wire.write(subAddress); // Put slave register address in Tx buffer
  Wire.write(data); // Put data in Tx buffer
  Wire.endTransmission(); // Send the Tx buffer
  //delay(1);
}

uint8_t readByte(uint8_t address, uint8_t subAddress) {
  uint8_t data; // `data` will store the register data
  Wire.beginTransmission(address); // Initialize the Tx buffer
  Wire.write(subAddress); // Put slave register address in Tx buffer
  Wire.endTransmission(false); // Send the Tx buffer, but send a restart to keep connection alive
  Wire.requestFrom(address, (uint8_t) 1); // Read one byte from slave register address
  data = Wire.read(); // Fill Rx buffer with result
  return data; // Return data read from slave register
}

void setup() {
  Serial.begin(115200);
  Serial.println("Setting-up MPU-6050...");
  
  Serial.println(F("Reset MPU [0x6B | PWR_MGMT_1] -> [00000000]"));
  Serial.println(F("DEVICE_RESET, SLEEP, CYCLE, -, TEMP_DIS, CLKSEL, CLKSEL, CLKSEL"));
  Serial.print(F("Old: ")); printBinary(readByte(MPU6050_ADDRESS, 0x6B));
  writeByte(MPU6050_ADDRESS, 0x6B, 0b00000000);
  Serial.print(F("New: ")); printBinary(readByte(MPU6050_ADDRESS, 0x6B));
  Serial.println(F("---"));

  Serial.println(F("Enable Accel and disable gyro [0x6C | PWR_MGMT_2] -> [11000111]"));
  Serial.println(F("LP_WAKE_CTRL, LP_WAKE_CTRL, STBY_XA, STBY_YA, STBY_ZA, STBY_XG, STBY_YG, STBY_ZG"));
  Serial.print(F("Old: ")); printBinary(readByte(MPU6050_ADDRESS, 0x6C));
  writeByte(MPU6050_ADDRESS, 0x6C, 0b11000111);
  Serial.print(F("New: ")); printBinary(readByte(MPU6050_ADDRESS, 0x6C));
  Serial.println(F("---"));

  Serial.println(F("Set Accel HPF to RESET settings [0x1C | ACCEL_CONFIG] -> [00000000]"));
  Serial.println(F("XA_ST, YA_ST, ZA_ST, AFS_SEL, AFS_SEL, ACCEL_HPF, ACCEL_HPF, ACCEL_HPF"));
  Serial.print(F("Old: ")); printBinary(readByte(MPU6050_ADDRESS, 0x1C));
  writeByte(MPU6050_ADDRESS, 0x1C, 0b00000000);
  Serial.print(F("New: ")); printBinary(readByte(MPU6050_ADDRESS, 0x1C));
  Serial.println(F("---"));

  Serial.println("Set Accel LPF Filter settings to 256Hz Bandwidth [0x1A | CONFIG] -> [00000000]");
  Serial.println("-, -, EXT_SYNC_SET, EXT_SYNC_SET, EXT_SYNC_SET, DLPF_CFG, DLPF_CFG, DLPF_CFG");
  Serial.print("Old: "); printBinary(readByte(MPU6050_ADDRESS, 0x1A));
  writeByte(MPU6050_ADDRESS, 0x1A, 0b00000000);
  Serial.print("New: "); printBinary(readByte(MPU6050_ADDRESS, 0x1A));
  Serial.println("---");
  
  Serial.println("Set Motion Interrupt Enable [0x38 | INT_ENABLE] -> [01000000]");
  Serial.println("FF_EN, MOT_EN, ZMOT_EN, FIFO_OFLOW_EN, I2C_MST_INT_EN, -, -, DATA_RDY_EN");
  Serial.print("Old: "); printBinary(readByte(MPU6050_ADDRESS, 0x38));
  writeByte(MPU6050_ADDRESS, 0x38, 0b01000000);
  Serial.print("New: "); printBinary(readByte(MPU6050_ADDRESS, 0x38));
  Serial.println("---");

  Serial.println("Set Motion Duration [0x20 | MOT_DUR] -> [00000001]");
  Serial.println("MOT_DUR, MOT_DUR, MOT_DUR, MOT_DUR, MOT_DUR, MOT_DUR, MOT_DUR, MOT_DUR");
  Serial.print("Old: "); printBinary(readByte(MPU6050_ADDRESS, 0x20));
  writeByte(MPU6050_ADDRESS, 0x20, 0b00000001);
  Serial.print("New: "); printBinary(readByte(MPU6050_ADDRESS, 0x20));
  Serial.println("---");

  //0b00010100 = 20(less sensitive), 0b00000100 = 4 (more sensitive), 0b00000010 = 2 (very sensitive)
  Serial.println("Set Motion Threshold [0x1F | MOT_THR] -> [00000100]");
  Serial.println("MOT_THR, MOT_THR, MOT_THR, MOT_THR, MOT_THR, MOT_THR, MOT_THR, MOT_THR");
  Serial.print("Old: "); printBinary(readByte(MPU6050_ADDRESS, 0x1F));  
  writeByte(MPU6050_ADDRESS, 0x1F, 0b00000100); 
  delay(1);
  Serial.print("New: "); printBinary(readByte(MPU6050_ADDRESS, 0x1F));
  Serial.println("---");
  
  //This item from datasheet seems wrong: Address notes 0x1A, but seems like 0x1C is correct, based on context
  Serial.println(F("Set Accel HPF to HOLD settings [0x1c | ACCEL_CONFIG] -> [00000111]"));
  Serial.println(F("XA_ST, YA_ST, ZA_ST, AFS_SEL, AFS_SEL, ACCEL_HPF, ACCEL_HPF, ACCEL_HPF"));
  Serial.print(F("Old: ")); printBinary(readByte(MPU6050_ADDRESS, 0x1C));
  writeByte(MPU6050_ADDRESS, 0x1C, 0b00000111);
  Serial.print(F("New: ")); printBinary(readByte(MPU6050_ADDRESS, 0x1C));
  Serial.println(F("---"));

  //00000111 = 1.25Hz, 10000111 = 2.5Hz, 01000111 = 5Hz, 11000111 = 10Hz
  Serial.println(F("Set the frequency of wakeup in cycle mode [0x6C | PWR_MGMT_2] -> [11000111]"));
  Serial.println(F("LP_WAKE_CTRL, LP_WAKE_CTRL, STBY_XA, STBY_YA, STBY_ZA, STBY_XG, STBY_YG, STBY_ZG"));
  Serial.print(F("Old: ")); printBinary(readByte(MPU6050_ADDRESS, 0x6C));
  writeByte(MPU6050_ADDRESS, 0x6C, 0b01000111);
  Serial.print(F("New: ")); printBinary(readByte(MPU6050_ADDRESS, 0x6C));
  Serial.println(F("---"));

  //Cycle mode significantly decreases sensivity, so if you want to use cycle mode, increase sensivity
  //In Cycle mode my MPU6050 consumed around 0.03mA
  Serial.println(F("Enable Cycle Mode (Low power mode) [0x6B | PWR_MGMT_1] -> [00100000]"));
  Serial.println(F("DEVICE_RESET, SLEEP, CYCLE, -, TEMP_DIS, CLKSEL, CLKSEL, CLKSEL"));
  Serial.print(F("Old: ")); printBinary(readByte(MPU6050_ADDRESS, 0x6B));
  writeByte(MPU6050_ADDRESS, 0x6B, 0b00100000);
  Serial.print(F("New: ")); printBinary(readByte(MPU6050_ADDRESS, 0x6B));
  Serial.println(F("---"));
  
  delay(2); //wait until first (false) interrupt occurs
  attachInterrupt(digitalPinToInterrupt(3), interrupt, RISING);
  Serial.println(F("IMU ready! Waiting for first interrupt..."));
}

void interrupt() {
  Serial.println(F("INTERRUPT!"));
  pinMode(pinBuzzer, OUTPUT);
  digitalWrite(pinBuzzer, HIGH);
  delay(30);
  digitalWrite(pinBuzzer, LOW);
  pinMode(pinBuzzer, INPUT);
}

void printBinary(byte b) {
  for (int i = 7; i >= 0; i-- ){
    Serial.print((b >> i) & 0X01);//shift and select first bit
  }
  Serial.println();
}

void loop() {
}
