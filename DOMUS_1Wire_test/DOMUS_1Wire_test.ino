#include <OneWire.h>
#include <DallasTemperature.h>

// Data wire is plugged into port 2 on the Arduino
#define ONE_WIRE_BUS 2
#define TEMPERATURE_PRECISION 9

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);

// arrays to hold device addresses
DeviceAddress thermometer_body1 = {0x3B,0xF8,0x0B,0x59,0x09,0xFC,0x2C,0x90};
DeviceAddress thermometer_body2 = {0x3B,0x19,0xC5,0x58,0x09,0xFC,0x6C,0xC7};
DeviceAddress thermometer_body3 = {0x3B,0xC7,0xDF,0x58,0x09,0xFC,0x0C,0xB4};
DeviceAddress thermometer_ball1 = {0x3B,0xA2,0xF0,0x58,0x09,0xFC,0x2C,0x28};
DeviceAddress thermometer_ball2 = {0x3B,0xC1,0xD7,0x58,0x09,0xFC,0x2C,0x1B};
DeviceAddress thermometer_ball3 = {0x3B,0xAE,0xE0,0x58,0x09,0xFC,0x0C,0x0A};

void setup(void)
{
  // start serial port
  Serial.begin(9600);
  Serial.println("Starting Temperatuer Read");

  // Start up the library
  sensors.begin();

  
  // set the resolution to 9 bit
  sensors.setResolution(thermometer_body1, TEMPERATURE_PRECISION);
  sensors.setResolution(thermometer_body2, TEMPERATURE_PRECISION);
  sensors.setResolution(thermometer_body3, TEMPERATURE_PRECISION);
  sensors.setResolution(thermometer_ball1, TEMPERATURE_PRECISION);
  sensors.setResolution(thermometer_ball2, TEMPERATURE_PRECISION);
  sensors.setResolution(thermometer_ball3, TEMPERATURE_PRECISION);
}

// function to print the temperature for a device
void printTemperature(DeviceAddress deviceAddress)
{
  float tempC = sensors.getTempC(deviceAddress);
  Serial.print("Temp C: ");
  Serial.print(tempC);
}

// function to print a device address
void printAddress(DeviceAddress deviceAddress)
{
  for (uint8_t i = 0; i < 8; i++)
  {
    // zero pad the address if necessary
    if (deviceAddress[i] < 16) Serial.print("0");
    Serial.print(deviceAddress[i], HEX);
  }
}


// main function to print information about a device
void printData(DeviceAddress deviceAddress)
{
  Serial.print("Device Address: ");
  printAddress(deviceAddress);
  Serial.print(" ");
  printTemperature(deviceAddress);
  Serial.println();
}

void loop(void)
{ 
  // call sensors.requestTemperatures() to issue a global temperature 
  // request to all devices on the bus
  Serial.print("Requesting temperatures...");
  sensors.requestTemperatures();
  Serial.println("DONE");

  // print the device information
  printData(thermometer_body1);
  printData(thermometer_body2);
  printData(thermometer_body3);
  printData(thermometer_ball1);
  printData(thermometer_ball2);
  printData(thermometer_ball3);
}
