#include <OneWire.h>

/* Example Input
 ******************************************
  ROM = 3B 19 C5 58 9 FC 6C C7
  Chip = MAX31850
  Data = 0 E0 1 B0 1E F0 FF FF FF 80  CRC=80
  Address = 0x0
  Temperature = 30.00 Celsius, 86.00 Fahrenheit
******************************************
 * 
 */

byte addr_temp_0[8] = {0x3B,0x19,0xC5,0x58,0x09,0xFC,0x6C,0xC7};
byte addr_temp_1[8] = {0x3B,0xF8,0x0B,0x59,0x09,0xFC,0x2C,0x90};

OneWire  ds(10); 
 
void setup() {
  Serial.begin(9600);
}

float readTemp(byte addr){
  
}

void loop() {
  byte present = 0;
  byte data[12];
  byte i;
  float celsius;
  // put your main code here, to run repeatedly:
  ds.select(addr_temp_0);
  ds.write(0x44, 1);
  //ds.depower();
  delay(1000);     // maybe 750ms is enough, maybe not
  // we might do a ds.depower() here, but the reset will take care of it.
  present = ds.reset();
  ds.select(addr_temp_0);    
  ds.write(0xBE);  

  for ( i = 0; i < 9; i++) {           // we need 9 bytes
    data[i] = ds.read();
    Serial.print(data[i], HEX);
    Serial.print(" ");
  }

  // Convert the data to actual temperature
  // because the result is a 16 bit signed integer, it should
  // be stored to an "int16_t" type, which is always 16 bits
  // even when compiled on a 32 bit processor.
  int16_t raw = (data[1] << 8) | data[0];
  celsius = (float)raw / 16.0;
  Serial.println("Celsius Temperature: ");
  Serial.println(celsius);
  delay(1000);
}
