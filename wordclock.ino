#include <DS3231.h>
#include <Wire.h>

#define CLINT 2

// Setup Clock
DS3231 Clock;
bool h12 = false;
bool pm = false;

typedef enum  {
    ST_CLOCK,
    ST_MENU
} STATES;

typedef enum  {
    EVT_CLOCK,
    EVT_BTN_SET
} EVENTS;

void setup() {
    Clock.setA2Time(0,0,0,0b01111110, false, false, false);
    Clock.turnOnAlarm(2);
    Clock.checkIfAlarm(2);

    pinMode(CLINT, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(CLINT), isr_alarm, FALLING);

    Serial.begin(9600);
    Serial.println("Starting");
}

void loop() {

}

void isr_alarm() {
    Serial.println("Interrupted.");
}
