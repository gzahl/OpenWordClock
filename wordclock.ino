#include <PinChangeInterrupt.h>
#include <avr/sleep.h>
#include <DS3231.h>
#include <Wire.h>

// Button and clock pins
#define CLINT 2
#define BUP 10
#define BSET 9
#define BDOWN 8

// Setup Clock
DS3231 Clock;
bool h12 = false;
bool pm = false;

// counter to set cpu to sleep
int sleepcounter = 0;

// isr variable for button interrupts
volatile int button = 0;

// state definitions
typedef enum  {
    ST_CLOCK,
    ST_MENU
} STATES;

// event definitions
typedef enum  {
    EVT_NONE,
    EVT_CLOCK,
    EVT_BTN,
    EVT_UP,
    EVT_UP_HOLD,
    EVT_SET,
    EVT_SET_HOLD,
    EVT_DOWN,
    EVT_DOWN_HOLD
} EVENTS;

// current event
EVENTS event = EVT_NONE;

void setup() {
    // setup cpu status led
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH);

    // Begin I2C communication
    Wire.begin();

    // setup clock to interrupt every minute
    Clock.setA2Time(0,0,0,0b01111110, false, false, false);
    Clock.turnOnAlarm(2);
    Clock.checkIfAlarm(2);

    // attach clock interrupt
    pinMode(CLINT, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(CLINT), isr_alarm, FALLING);

    // set up button pins
    pinMode(BUP, INPUT);
    pinMode(BSET, INPUT);
    pinMode(BDOWN, INPUT);

    // attach button interrupts
    attachPCINT(digitalPinToPCINT(BUP), isr_bup, FALLING);
    attachPCINT(digitalPinToPCINT(BSET), isr_bset, FALLING);
    attachPCINT(digitalPinToPCINT(BDOWN), isr_bdown, FALLING);

    // Begin Serial communication
    Serial.begin(9600);
    Serial.println("Starting");
}

void loop() {
    // if button event happened, resolve button event, before hopping into
    // state machine. This translates EVT_BTN to a more specific button event.
    if (event == EVT_BTN) {
        Serial.print("Btn event: ");
        sleepcounter = 0;
        event = resolve_btn();
        button = 0;
    }

    // Not a state machine yet.
    switch (event) {
        case EVT_CLOCK:
            Clock.checkIfAlarm(2);
            Serial.println( "Clock event.");
            sleepcounter = 0;
            event = EVT_NONE;
            break;
        case EVT_UP:
            Serial.println("Up.");
            event = EVT_NONE;
            break;
        case EVT_UP_HOLD:
            Serial.println("Hold up.");
            event = EVT_NONE;
            break;
        case EVT_SET:
            Serial.println("Set.");
            event = EVT_NONE;
            break;
        case EVT_SET_HOLD:
            Serial.println("Hold set.");
            event = EVT_NONE;
            break;
        case EVT_DOWN:
            Serial.println("Down.");
            event = EVT_NONE;
            break;
        case EVT_DOWN_HOLD:
            Serial.println("Hold down.");
            event = EVT_NONE;
        default:
            sleepcounter++;
    }

    // if nothing happened for the last 200 cycles, fall asleep.
    if (sleepcounter > 00) {
        GoToSleep();
    }
    delay(10);
}

void isr_alarm() {
    // clock interrupt service routine
    event = EVT_CLOCK;
    return;
}

void GoToSleep() {
    // Set cpu to sleep routine
    sleep_enable();
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);
    digitalWrite(LED_BUILTIN, LOW);
    delay(10);
    sleep_cpu();
    digitalWrite(LED_BUILTIN, HIGH);
    sleep_disable();
    delay(10);
    return;
}

void isr_btn(const int& btn) {
    // General button interrupt service routine.
    // This will be called with the pin of the button, which caused the interrupt.
    if (button) {
        return;
    } else {
        event = EVT_BTN;
        button = btn;
        return;
    }
}

// interrupt service routines for all buttons
void isr_bup() {
    isr_btn(BUP);
}

void isr_bset() {
    isr_btn(BSET);
}

void isr_bdown() {
    isr_btn(BDOWN);
}


EVENTS resolve_btn() {
    // button resolver.
    // translates button event into specific button click or hold action.
    int count = 0;
    EVENTS evtout = EVT_NONE;
    while (count < 4 or evtout == EVT_NONE) {
        if (count > 100) {
            switch (button) {
                case BUP:
                    evtout = EVT_UP_HOLD;
                    break;
                case BSET:
                    evtout = EVT_SET_HOLD;
                    break;
                case BDOWN:
                    evtout = EVT_DOWN_HOLD;
                    break;
            }
            break;
        }

        if (digitalRead(button)) {
            switch (button) {
                case BUP:
                    evtout = EVT_UP;
                    break;
                case BSET:
                    evtout = EVT_SET;
                    break;
                case BDOWN:
                    evtout = EVT_DOWN;
                    break;
            }
            break;
        }
        count++;
        delay(10);
    }
    return evtout;
}
