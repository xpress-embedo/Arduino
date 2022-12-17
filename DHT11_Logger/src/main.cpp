#include <Arduino.h>
#include <RTClib.h>
#include <SD.h>
#include <DHT.h>
#include <DHT_U.h>

// Macros
#define FILE_NAME_SIZE          (13u)
#define SD_BUFFER_SIZE          (20u)
#define LED_TASK_TIME           (1000u)
#define DHT11_TASK_TIME         (2000u)
#define DHTPIN                  (2)
#define DHTTYPE                 (DHT11)

// Timestamp/Task Related Variables
static uint32_t led_timestamp = 0;
static uint32_t dht11_timestamp = 0;

// Led Related Variables
const int led = 13;
static boolean led_state = false;

// DHT11 Sensor Related Variables
static float temperature = 0.0;
static float humidity = 0.0;
DHT_Unified dht(DHTPIN, DHTTYPE);

// RTC Object and Related Variables
RTC_DS1307 rtc;
uint8_t day;
uint8_t month;
uint16_t year;
uint8_t hour;
uint8_t minute;
uint8_t second;

// SD Card Related Variables
const int chip_select = 4;
/* SD Card is attached to the SPI bus as follow
MOSI  -- 11
MISO  -- 12
CLK   -- 13
CS    -- 4
*/
char filename[FILE_NAME_SIZE] = { 0 };
char sd_buffer[SD_BUFFER_SIZE] = { 0 };
File my_file;

void setup() 
{
  Serial.begin(9600);
  pinMode( led, OUTPUT);

  // see if the card is present and can be initialized
  if (!SD.begin(chip_select)) 
  {
    Serial.println("Card failed, or not present");
    /*Exit Program*/
    return;
  }
  Serial.println("Card Initialized.");

  // Check for RTC
  if ( !rtc.begin() )
  {
    Serial.println("Couldn't find the RTC");
    return;
  }
  Serial.println("RTC DS1307 Initialized");

  // Initialize DHT Sensor
  dht.begin();
  delay( 1000 );

  led_timestamp = millis();
  dht11_timestamp = millis();
}

void loop()
{
  // DHT11 Measurement and Logging Task
  if( millis() - dht11_timestamp >= DHT11_TASK_TIME )
  {
    dht11_timestamp = millis();
    // get temperature event and print its value
    sensors_event_t event_temp;
    // get humidity event and print its value
    sensors_event_t event_humid;

    dht.temperature().getEvent( &event_temp );
    dht.humidity().getEvent( &event_humid );
    
    // Check if any reads failed and exit early (to try again).
    if (isnan(event_temp.temperature) || isnan(event_humid.relative_humidity) ) 
    {
      Serial.println(F("Failed to read from DHT sensor!"));
    }
    else
    {
      // Read temperature as Celsius (the default)
      temperature = event_temp.temperature;
      // Read the humidity value
      humidity = event_humid.relative_humidity;

      // get the data from RTC
      DateTime now = rtc.now();
      day = now.day();
      month = now.month();
      year = now.year();
      year = year-2000u;
      hour = now.hour();
      minute = now.minute();
      second = now.second();

      // Saving only the integer part
      uint8_t temp = (uint8_t)temperature;
      uint8_t humid = (uint8_t)humidity;

      // prepare filename
      snprintf( filename, FILE_NAME_SIZE, "%.2d-%.2d-%d.txt", month, day, year);
      // Save data in the memory card
      snprintf( sd_buffer, SD_BUFFER_SIZE, "%.2d:%.2d:%.2d,%d,%d", \
                hour, minute, second, temp, humid );
      // printing SD card buffer data, for debugging or for plotting also
      Serial.println( sd_buffer );
      my_file = SD.open( filename, FILE_WRITE );
      my_file.println( sd_buffer );
      my_file.close();
    }
  }


  // LED Task
  if ( millis()-led_timestamp >= LED_TASK_TIME )
  {
    led_timestamp = millis();
    if ( led_state )
    {
      led_state = false;
      digitalWrite( led, HIGH);
    }
    else
    {
      led_state = true;
      digitalWrite( led, LOW);
    }
  }
}