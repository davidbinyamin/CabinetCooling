
// Include Libraries
#include "Arduino.h"
#include <LiquidCrystal.h>
#include <NTC_Thermistor.h>
#include "FanControl.h"

//---- Pin Definitions ----//
#define interrupt_pin 2

#define FAN_CONTROL_PIN 4

#define LCD_PIN_RS 7
#define LCD_PIN_E 8
#define LCD_PIN_DB4 9
#define LCD_PIN_DB5 10
#define LCD_PIN_DB6 11
#define LCD_PIN_DB7 12
#define LCD_PIN_A 13

// Temperature Sensor definitions
#define SENSOR_PIN             A5
#define REFERENCE_RESISTANCE   7500
#define NOMINAL_RESISTANCE     100000
#define NOMINAL_TEMPERATURE    24
#define B_VALUE                3950
#define READINGS_NUMBER  10
#define DELAY_TIME  20

//---- Global variables and defines ----//
NTC_Thermistor* thermistor = NULL;
FanControl fans(FAN_CONTROL_PIN, 5, 0, 29, 42);

LiquidCrystal lcd(LCD_PIN_RS,LCD_PIN_E,LCD_PIN_DB4,LCD_PIN_DB5,LCD_PIN_DB6,LCD_PIN_DB7);

byte degree[8] = 
{
  0b00011,
  0b00011,
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b00000,
};

String temperatureLine; 
String fanDutyLine;
volatile byte state = LOW;

void setup() 
{
  // Set pins mode
  pinMode(LCD_PIN_A,OUTPUT);
  
  // Start with display BackLight off
  digitalWrite(LCD_PIN_A,LOW);
  
  // Attach interrupt pin (pin 2)
  pinMode(interrupt_pin, INPUT_PULLUP);
  attachInterrupt(0, displayToggle, RISING);

  // Set up the LCD's number of columns and rows
  lcd.begin(16, 2);
  lcd.createChar(1, degree);
  lcd.noDisplay();
  
  // Set temperature sensor
  thermistor = new NTC_Thermistor(SENSOR_PIN,REFERENCE_RESISTANCE,NOMINAL_RESISTANCE,NOMINAL_TEMPERATURE,B_VALUE,READINGS_NUMBER,DELAY_TIME);
  
  fans.init();
}

void loop() 
{
    const double temperature = thermistor->readCelsius();
    
    fans.updateFanDuty(temperature);   
    fans.setFanSpeed();
    updateLCD(temperature, fans.getFanDuty());

    delay(2000);
}

void displayToggle() 
{
  static unsigned long last_interrupt_time = 0;
  unsigned long interrupt_time = millis();
  // If interrupts come faster than 200ms, assume it's a bounce and ignore
  if (interrupt_time - last_interrupt_time > 200) 
  {
    state = !state;
    digitalWrite(LCD_PIN_A,state);
    state ? lcd.display() : lcd.noDisplay();
  }
   
  last_interrupt_time = interrupt_time;
}

void updateLCD(double temperature, int fanDuty)
{
    temperatureLine = String("Temp: " + String(temperature));
    fanDutyLine = String("Fan speed:" + String(fanDuty) + "%");

  if(state)
  {
    lcd.display();
    lcd.setCursor(0, 0);
    lcd.clear();
    lcd.print(temperatureLine);
    lcd.write(byte(1));
    lcd.print("C");
    lcd.setCursor(0,1);
    lcd.print(fanDutyLine); 
  }
}
