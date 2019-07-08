// Based on : https://github.com/adafruit/Light-and-Temp-logger/blob/master/lighttemplogger.ino

#include <SPI.h>
#include <SD.h>
#include <Wire.h>
#include "RTClib.h"

// A simple data logger for the Arduino analog pins

#define LOG_INTERVAL  1000 // mills between entries (reduce to take more/faster data)
#define SYNC_INTERVAL 1000 // mills between calls to flush() - to write data to the card
#define ECHO_TO_SERIAL   1 // echo data to serial port


// The analog pins that connect to the sensors
#define wsense1 A0           // analog 0
#define wsense2 A1           // analog 1
#define wsense3 A2           // analog 2

//CO2 PIN
#define co2_pin A3

#include "DHT.h"

#define DHTPIN 3     // Digital pin connected to the DHT sensor
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
DHT dht(DHTPIN, DHTTYPE);


RTC_DS1307 RTC; // define the Real Time Clock object

// for the data logging shield, we use digital pin 10 for the SD cs line
const int chipSelect = 10;
//Constant
uint32_t syncTime = 0; // time of last sync()

// the logging file
File logfile;

void setup(void)
{
  Serial.begin(9600);
  Serial.println();  

  // initialize the SD card
  Serial.print("Initializing SD card...");
  // make sure that the default chip select pin is set to
  // output, even if you don't use it:
  pinMode(10, OUTPUT);
  
  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.print("error: ");
    Serial.println("Card failed, or not present");
  }
  Serial.println("card initialized.");
  
  // create a new file
  char filename[] = "LOGGER00.CSV";
  for (uint8_t i = 0; i < 100; i++) {
    filename[6] = i/10 + '0';
    filename[7] = i%10 + '0';
    if (! SD.exists(filename)) {
      // only open a new file if it doesn't exist
      logfile = SD.open(filename, FILE_WRITE); 
      break;  // leave the loop!
    }
  }
  
  if (! logfile) {
    Serial.print("error: ");
    Serial.println("couldnt create file");
  }
  
  Serial.print("Logging to: ");
  Serial.println(filename);

  // connect to RTC
  Wire.begin();  
  if (!RTC.begin()) {
    Serial.println("RTC failed");
  }

  //Begin DHT22
  dht.begin();

  logfile.println("millis,stamp,datetime,humidity,dht22_temperature,wind_sensor_1_raw,wind_sensor_2_raw,wind_sensor_3_raw,co2_raw,co2");    
  Serial.println("millis,stamp,datetime,humidity,dht22_temperature,wind_sensor_1_raw,wind_sensor_2_raw,wind_sensor_3_raw,co2_raw,co2");
}

void loop(void)
{
  DateTime now;

  // delay for the amount of time we want between readings
  delay((LOG_INTERVAL -1) - (millis() % LOG_INTERVAL));
  
  // log milliseconds since starting
  uint32_t m = millis();
  logfile.print(m);           // milliseconds since start
  logfile.print(", ");    
#if ECHO_TO_SERIAL
  Serial.print(m);         // milliseconds since start
  Serial.print(", ");  
#endif

  // fetch the time
  now = RTC.now();
  // log time
  logfile.print(now.unixtime()); // seconds since 1/1/1970
  logfile.print(", ");
  logfile.print('"');
  logfile.print(now.year(), DEC);
  logfile.print("/");
  logfile.print(now.month(), DEC);
  logfile.print("/");
  logfile.print(now.day(), DEC);
  logfile.print(" ");
  logfile.print(now.hour(), DEC);
  logfile.print(":");
  logfile.print(now.minute(), DEC);
  logfile.print(":");
  logfile.print(now.second(), DEC);
  logfile.print('"');
#if ECHO_TO_SERIAL
  Serial.print(now.unixtime()); // seconds since 1/1/1970
  Serial.print(", ");
  Serial.print('"');
  Serial.print(now.year(), DEC);
  Serial.print("/");
  Serial.print(now.month(), DEC);
  Serial.print("/");
  Serial.print(now.day(), DEC);
  Serial.print(" ");
  Serial.print(now.hour(), DEC);
  Serial.print(":");
  Serial.print(now.minute(), DEC);
  Serial.print(":");
  Serial.print(now.second(), DEC);
  Serial.print('"');
#endif //ECHO_TO_SERIAL

  //Read Humidity
  float humidity = dht.readHumidity();
  float temp = dht.readTemperature();

  //Wind Sensor Read
  analogRead(wsense1); 
  delay(10);
  int wsense1_read = analogRead(wsense1);  
  
  analogRead(wsense2); 
  delay(10);
  int wsense2_read = analogRead(wsense2);  
  
  analogRead(wsense3); 
  delay(10);
  int wsense3_read = analogRead(wsense3);  
  
  analogRead(co2_pin); 
  delay(10);
  int co2_read = analogRead(co2_pin);  

  //Converted based on https://www.edaphic.com.au/wp-content/uploads/2015/07/Edaphic_B530_Ver2.2.pdf
  //102 - 0ppm; 921 -10,000ppm;  Linear; ax+b; a = 12.21 b= -1245.4212
  float co2_conv = 12.21*co2_read-1245.4212;
  
//Humidity Write
  logfile.print(", ");    
  logfile.print(humidity);
#if ECHO_TO_SERIAL
  Serial.print(", ");   
  Serial.print(humidity);
#endif //ECHO_TO_SERIAL

//Temp DHT22 Write
  logfile.print(", ");    
  logfile.print(temp);
#if ECHO_TO_SERIAL
  Serial.print(", ");   
  Serial.print(temp);
#endif //ECHO_TO_SERIAL

//Wind Sensor Write
  logfile.print(", ");    
  logfile.print(wsense1_read);
#if ECHO_TO_SERIAL
  Serial.print(", ");   
  Serial.print(wsense1_read);
#endif //ECHO_TO_SERIAL
  logfile.print(", ");    
  logfile.print(wsense2_read);
#if ECHO_TO_SERIAL
  Serial.print(", ");   
  Serial.print(wsense2_read);
#endif //ECHO_TO_SERIAL
  logfile.print(", ");    
  logfile.print(wsense3_read);
#if ECHO_TO_SERIAL
  Serial.print(", ");   
  Serial.print(wsense3_read);
#endif //ECHO_TO_SERIAL


//Write CO2
  logfile.print(", ");    
  logfile.print(co2_read);
#if ECHO_TO_SERIAL
  Serial.print(", ");   
  Serial.print(co2_read);
#endif //ECHO_TO_SERIAL

//Write CO2 Converted
  logfile.print(", ");    
  logfile.print(co2_conv);
#if ECHO_TO_SERIAL
  Serial.print(", ");   
  Serial.print(co2_conv);
#endif //ECHO_TO_SERIAL

  logfile.println();
#if ECHO_TO_SERIAL
  Serial.println();
#endif // ECHO_TO_SERIAL

  // Now we write data to disk! Don't sync too often - requires 2048 bytes of I/O to SD card
  // which uses a bunch of power and takes time
  if (((millis()- syncTime) < SYNC_INTERVAL) || isnan(humidity)) return;
  syncTime = millis();
  
  logfile.flush();
  
}
