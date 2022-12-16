#include <Arduino.h>

const int led = 13;
static boolean led_state = false;
static uint32_t led_timestamp = 0;
static uint32_t log_timestamp = 0;

void setup() 
{
  Serial.begin(9600);
  pinMode( led, OUTPUT);
  /* see if the card is present and can be initialized*/
  // if (!SD.begin(chipSelect)) 
  // {
  //   Serial.println("Card failed, or not present");
  //   /*Exit Program*/
  //   return;
  // }
  // Serial.println("Card Initialized.");
  // /*Check for RTC*/
  // if ( !rtc.begin() )
  // {
  //   Serial.println("Couldn't find the RTC");
  //   return;
  // }
  Serial.println("RTC DS1307 Initialized");
  led_timestamp = millis();
  log_timestamp = millis();
}

void loop()
{
  // LED Task
  if ( millis()-led_timestamp >= 1000u )
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