#include <PinChangeInterrupt.h>
#include <avr/sleep.h>
#include <DS3231.h>
#include <Wire.h>
#include "src/Wordclock.h"

// Button and clock pins
#define CLINT 2
#define LEDPIN 4
#define BUP 10
#define BSET 9
#define BDOWN 8
#define TIMEOUT 60

// Setup Clock
DS3231 Clock;
bool h12 = false;
bool pm = false;

// color index
byte cindex = 51;
byte lastcindex = 0;

// brightness
byte brightness = 128;

// isr variable for button interrupts
volatile int button = 0;

// Setup the LEDs
Wordclock wordclock(LEDPIN);
// WordClockState clockstate;

// state definitions
enum state_t {
    ST_CLOCK,
    ST_MENU,
    ST_SET,
    ST_TOFF,
    ST_BRIGHTNESS
};

// event definitions
enum event_t {
    EVT_NONE,
    EVT_CLOCK,
    EVT_BTN,
    EVT_UP,
    EVT_UP_HOLD,
    EVT_SET,
    EVT_SET_HOLD,
    EVT_DOWN,
    EVT_DOWN_HOLD,
    EVT_TIMEOUT
};

// current state
state_t state  = ST_CLOCK;

// current event
event_t event = EVT_NONE;
volatile event_t event_buffer = EVT_NONE;

// ============================================================================
// ===== SETUP FUNCTION =======================================================
// ============================================================================

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

    // Begin Wordclock LED control
    wordclock.begin();
    wordclock.setBrightness(brightness);
    delay(100);

    ccode(cindex, wordclock.color);

    wordclock.clear();
    wordclock.update(wordclock.INITPIC);
    wordclock.show();
    sendClockState(cindex, wordclock.INITPIC);

    event_buffer = EVT_CLOCK;
    delay(2000);
}

// ============================================================================
// ===== Main Loop ============================================================
// ============================================================================

void loop() {
    // counter to put cpu to sleep
    static int sleepcounter = 0;

    // timeout tracker
    static int lastevent = 0;

    event = event_buffer;
    event_buffer = EVT_NONE;

    // if button event happened, resolve button event, before hopping into
    // state machine. This translates EVT_BTN to a more specific button event.
    if (event == EVT_BTN) {
        sleepcounter = 0;
        resolve_btn(event);
    }

    switch (state) {
        case ST_CLOCK:
            prog_clock(state, event);
            break;
        case ST_MENU:
            prog_menu(state, event);
            break;
        case ST_SET:
            prog_set(state, event);
            break;
        case ST_BRIGHTNESS:
            prog_brightness(state, event);
            break;
        case ST_TOFF:
            prog_toff(state, event);
            break;
    }

    if (event != EVT_NONE) {
        // With each event, reset timeout and sleepcounter mechanism
        sleepcounter = 0;
        lastevent = millis()/1000;
    } else if (state == ST_CLOCK) {
        // if no event occurs for 5s in the clock state,
        // put the cpu to sleep
        sleepcounter++;
        if (sleepcounter > 500) {
            GoToSleep();
        }
    } else {
        // if no event happens in any other state for TIMEOUT seconds
        // trigger a timeout event.
        if (millis()/1000 - lastevent > TIMEOUT) {
            event_buffer = EVT_TIMEOUT;
        }
    }
    delay(10);
}

// ============================================================================
// ===== state machine programs ===============================================
// ============================================================================

void prog_clock(state_t& state, event_t& event) {
    switch (event) {
        case EVT_CLOCK:
            Clock.checkIfAlarm(2);
            delay(10);
            wordclock.hour = Clock.getHour(h12, pm);
            wordclock.minute = Clock.getMinute();
            wordclock.second = Clock.getSecond();

            wordclock.clear();
            wordclock.update();
            wordclock.show();

            sendClockState(cindex, wordclock.getState());
            break;

        case EVT_UP:
            cindex++;
            cindex %= 102;
            ccode(cindex, wordclock.color);
            event_buffer = EVT_CLOCK;
            break;

        case EVT_UP_HOLD:
            if (cindex > 101) {
                break;
            } else {
                while (!digitalRead(BUP)) {
                    cindex++;
                    cindex %= 102;
                    ccode(cindex, wordclock.color);
                    wordclock.clear();
                    wordclock.update();
                    wordclock.show();
                    sendClockState(cindex, wordclock.getState());
                    delay(50);
                }
                break;
            }
            break;

        case EVT_SET:
            if (cindex > 101) {
                cindex = lastcindex;
            } else {
                lastcindex = cindex;
                cindex = 255;
            }
            ccode(cindex, wordclock.color);
            event_buffer = EVT_CLOCK;
            break;

        case EVT_SET_HOLD:
            state = ST_MENU;
            event_buffer = EVT_SET_HOLD;
            break;

        case EVT_DOWN:
            cindex--;
            if (cindex == 255) {
                cindex = 101;
            }
            cindex %= 102;
            ccode(cindex, wordclock.color);
            event_buffer = EVT_CLOCK;
            break;

        case EVT_DOWN_HOLD:
            if (cindex > 101) {
                break;
            } else {
                while (!digitalRead(BDOWN)) {
                    cindex--;
                    if (cindex == 255) {
                        cindex = 101;
                    }
                    cindex %= 102;
                    ccode(cindex, wordclock.color);
                    wordclock.clear();
                    wordclock.update();
                    wordclock.show();
                    sendClockState(cindex, wordclock.getState());
                    delay(50);
                }
                break;
            }
    }
    return;
}

void prog_menu(state_t& state, event_t& event) {
    enum menuitem_t {set, toff, brightness, count};
    static int menuitem = set;
    switch (event) {
        case EVT_UP:
            menuitem++;
            menuitem %= count;
            break;
        case EVT_SET:
            switch (menuitem) {
                case set:
                    state = ST_SET;
                    break;
                case toff:
                    state = ST_TOFF;
                    break;
                case brightness:
                    state = ST_BRIGHTNESS;
                    break;
            }
            break;
        case EVT_DOWN:
            menuitem += (count - 1);
            menuitem %= count;
            break;
        case EVT_TIMEOUT:
            state = ST_CLOCK;
            break;
    }
    if (event != EVT_NONE) {
        wordclock.clear();
        switch (menuitem) {
            case set:
                wordclock.update(Wordclock::SET);
                sendClockState(cindex, Wordclock::SET);
                break;
            case toff:
                wordclock.update(Wordclock::TOFF);
                sendClockState(cindex, Wordclock::TOFF);
                break;
            case brightness:
                wordclock.update(Wordclock::BRIGHTNESS);
                sendClockState(cindex, Wordclock::BRIGHTNESS);
                break;
        }
        wordclock.show();
    }
    return;
}

void prog_set(state_t& state, event_t& event) {

    enum setting_t {init, shour, smin5, smin1, ssec};
    static setting_t setting = init;

    static byte hour;
    static byte minute;
    static byte second;

    static int lastevent;
    static int lastupdate;
    static int elapsed;

    static WordClockState WCstate;

    if (setting == init) {
        hour = wordclock.hour;
        minute = wordclock.minute;
        second = wordclock.second;
        elapsed = 0;
        lastupdate = 0;
        lastevent = millis()/1000;
        setting = smin5;
    }

    switch (event) {
        case EVT_UP:
            switch (setting) {
                case shour:
                    ++hour;
                    hour %= 12;
                    break;
                case smin5:
                    minute += 5;
                    minute %= 60;
                    break;
                case smin1:
                    ++minute;
                    minute %= 60;
                    break;
                case ssec:
                    second = 0;
                    Clock.setSecond(second);
                    break;
            }
            break;
        case EVT_SET:
            switch (setting) {
                case shour:
                    setting = smin1;
                    Clock.setHour(hour);
                    break;
                case smin5:
                    setting = shour;
                    break;
                case smin1:
                    setting = ssec;
                    Clock.setMinute(minute);
                    break;
                case ssec:
                    setting = init;
                    state = ST_CLOCK;
                    event_buffer = EVT_CLOCK;
                    break;
            }
            break;
        case EVT_DOWN:
            switch (setting) {
                case shour:
                    hour += 11;
                    hour %= 12;
                    break;
                case smin5:
                    minute += 55;
                    minute %= 60;
                    break;
                case smin1:
                    minute += 59;
                    minute %= 60;
                    break;
                case ssec:
                    second = 0;
                    Clock.setSecond(second);
                    break;
            }
            break;
        case EVT_TIMEOUT:
            setting = init;
            state = ST_CLOCK;
            break;
    }


    if (elapsed != lastupdate) {
        if (elapsed % 2) {
            switch (setting) {
                case smin5:
                case shour:
                case smin1:
                    Wordclock::getState(WCstate, hour, minute, second);
                    break;
                case ssec:
                    second = Clock.getSecond();
                    WCstate = Wordclock::SEC[second % 10] | Wordclock::SEC[second / 10 + 10];
                    break;
            }
        } else {
            Wordclock::getState(WCstate, hour, minute, second);
            switch (setting) {
                case shour:
                    WCstate &= (~Wordclock::MSKHOUR);
                    break;
                case smin5:
                    WCstate &= (~Wordclock::MSKMIN5);
                    break;
                case smin1:
                    WCstate &= ~0x000f;
                    break;
                case ssec:
                    second = Clock.getSecond();
                    WCstate = Wordclock::SEC[second % 10] | Wordclock::SEC[second / 10 + 10];
                    break;
            }

        }

        wordclock.clear();
        wordclock.update(WCstate);
        wordclock.show();
        sendClockState(cindex, WCstate);
        lastupdate = elapsed;
    }

    if (event != EVT_NONE) {
        lastevent = millis()/1000;
        elapsed = 0;
    }
    elapsed = millis()/1000 - lastevent;
    return;
}

void prog_toff(state_t& state, event_t& event) {
    Serial.println("Prog: toff");
    state = ST_CLOCK;
    event_buffer = EVT_CLOCK;
    return;
}

void prog_brightness(state_t& state, event_t& event) {
    switch (event) {
        case EVT_UP:
            brightness++;
            break;
        case EVT_UP_HOLD:
            while (!digitalRead(BUP)) {
                wordclock.clear();
                wordclock.setBrightness(++brightness);
                wordclock.update();
                wordclock.show();
                Serial.print("Brightness: ");
                Serial.println(brightness);
                delay(10);
            }
            break;
        case EVT_DOWN:
            brightness--;
            break;
        case EVT_DOWN_HOLD:
            while (!digitalRead(DOWN)) {
                wordclock.clear();
                wordclock.setBrightness(--brightness);
                wordclock.update();
                wordclock.show();
                Serial.print("Brightness: ");
                Serial.println(brightness);
                delay(10);
            }
            break;
        case EVT_SET:
        case EVT_TIMEOUT:
            state = ST_CLOCK;
            event_buffer = EVT_CLOCK;
            break;
    }
    if (event != EVT_NONE) {
        wordclock.clear();
        wordclock.setBrightness(brightness);
        wordclock.update();
        wordclock.show();
        Serial.print("Brightness: ");
        Serial.println(brightness);
    }
    return;
}

// ============================================================================
// ===== interrupt service routines ===========================================
// ============================================================================

void isr_alarm() {
    // clock interrupt service routine
    event_buffer = EVT_CLOCK;
    return;
}

void isr_btn(const int& btn) {
    // General button interrupt service routine.
    // This will be called with the pin of the button, which caused the interrupt.

    if (button) {
        return;
    } else {
        event_buffer = EVT_BTN;
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

void resolve_btn(event_t& evtout) {
    // button resolver.
    // translates button event into specific button click or hold action.
    int count = 0;
    while (!digitalRead(button)) {
        count++;
        if (count > 100) {
            break;
        }
        delay(10);
    }
    if (count < 4) {
        evtout = EVT_NONE;
    } else if (count < 100) {
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
    } else if (count > 100) {
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
    }
    button = 0;
    return;
}

// ============================================================================
// ===== helper functions =====================================================
// ============================================================================

void sendClockState(const int& cindex, const WordClockState& state) {
    Serial.print(cindex);
    Serial.print(" ");
    for (size_t index = 0; index < 8; index++) {
        Serial.print(state.getWord(index), HEX);
        Serial.print(" ");
    }
    Serial.println();
    delay(10);
    return;
}

byte cvalue(const byte& index) {
    if (index < 17) {
        return index;
    } else if (index < 51) {
        return 255;
    } else if (index < 68) {
        return 68 - index;
    } else {
        return 0;
    }
}

void ccode(const byte& index, byte* colors) {
    if (index > 102) {
        colors[0] = 0;
        colors[1] = 0;
        colors[2] = 0;
        colors[3] = 255;
    } else {
        colors[0] = cvalue(index)*15;
        colors[1] = cvalue((index+34)%102)*15;
        colors[2] = cvalue((index+68)%102)*15;
        colors[3] = 0;
    }
    return;
}

void GoToSleep() {
    // Set cpu to sleep routine
    sleep_enable();
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);
    digitalWrite(LED_BUILTIN, LOW);
    delay(100);
    sleep_cpu();
    digitalWrite(LED_BUILTIN, HIGH);
    sleep_disable();
    delay(100);
    return;
}
