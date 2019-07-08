// Based on : https://github.com/adafruit/Light-and-Temp-logger/blob/master/lighttemplogger.ino

#include <SPI.h>
#include <SD.h>
#include <Wire.h>
#include "RTClib.h"

// A simple data logger for the Arduino analog pins

#define LOG_INTERVAL  1000 // mills between entries (reduce to take more/faster data)
#define SYNC_INTERVAL 1000 // mills between calls to flush() - to write data to the card
#define ECHO_TO_SERIAL   1 // echo data to serial port

// the digital pins that connect to the LEDs
#define redLEDpin 2
#define greenLEDpin 3

// The analog pins that connect to the sensors
#define analogPin A0           // analog 0


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

  logfile.println("millis,stamp,datetime,humidity,A0");    
  Serial.println("millis,stamp,datetime,humidity,A0");
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
  
  analogRead(analogPin); 
  delay(10);
  int analogReading = analogRead(analogPin);  


  logfile.print(", ");    
  logfile.print(humidity);
#if ECHO_TO_SERIAL
  Serial.print(", ");   
  Serial.print(humidity);
#endif //ECHO_TO_SERIAL


  logfile.print(", ");    
  logfile.print(analogReading);
#if ECHO_TO_SERIAL
  Serial.print(", ");   
  Serial.print(analogReading);
#endif //ECHO_TO_SERIAL

  logfile.println();
#if ECHO_TO_SERIAL
  Serial.println();
#endif // ECHO_TO_SERIAL

  digitalWrite(greenLEDpin, LOW);

  // Now we write data to disk! Don't sync too often - requires 2048 bytes of I/O to SD card
  // which uses a bunch of power and takes time
  if (((millis()- syncTime) < SYNC_INTERVAL) || isnan(humidity)) return;
  syncTime = millis();
  
  logfile.flush();
  
}
