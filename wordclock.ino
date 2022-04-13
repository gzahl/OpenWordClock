#include <PinChangeInterrupt.h>
#include <avr/sleep.h>
#include <DS3231.h>
#include <Wire.h>
#include "src/Wordclock.h"

// Version Number
#define VERSION "v0.1.0"

// Button and clock pins
#define CLINT 2
#define LEDPIN 4
#define BUP 10
#define BSET 9
#define BDOWN 8

// Timeout setting
#define TIMEOUT 60

// Setup Clock
DS3231 Clock;
bool h12 = false;
bool pm = false;

// color index
byte cindex = 51;
byte lastcindex = 0;
bool rainbow = false;

// brightness
byte brightness = 128;

// nightmode
struct ntime_t {
    byte hour;
    byte min5;
};
ntime_t ton = {0, 0};
ntime_t toff = {0, 0};
bool nightmode = false;

// isr variable for button interrupts
volatile byte button = 0;

// Setup the LEDs
Wordclock wordclock(LEDPIN);
// WordClockState clockstate;

// state definitions
enum state_t {
    ST_CLOCK,           // basic clock state
    ST_MENU,            // Menu display state
    ST_SET,             // Time setting mode
    ST_NIGHTMODE,       // Nightmode settings
    ST_BRIGHTNESS,      // Brightness settings
    ST_TEMPERATURE      // Temperature display
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
    Serial.println(F("OpenWordClock"));
    Serial.print(F("Firmware Version "));
    Serial.println(VERSION);

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
        case ST_NIGHTMODE:
            prog_nightmode(state, event);
            break;
        case ST_TEMPERATURE:
            prog_temperature(state, event);
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

            if (rainbow) {
                cindex++;
                cindex %= 102;
                ccode(cindex, wordclock.color);
            }

            wordclock.hour = Clock.getHour(h12, pm);
            wordclock.minute = Clock.getMinute();
            wordclock.second = Clock.getSecond();

            wordclock.clear();

            if (nightmode &&
                wordclock.hour >= toff.hour &&
                wordclock.minute >= toff.min5 &&
                wordclock.hour <= ton.hour &&
                wordclock.minute < ton.min5) {
                wordclock.update(WordClockState({0,0,0,0,0,0,0,0}));
            } else {
                wordclock.update();
            }
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
                if (rainbow) {
                    lastcindex = cindex;
                    cindex = 255;
                    rainbow = false;
                } else {
                    rainbow = true;
                    for (size_t rep = 0; rep < 2; rep++) {
                        wordclock.clear();
                        wordclock.show();
                        sendClockState(cindex, WordClockState());
                        delay(100);
                        wordclock.update();
                        wordclock.show();
                        sendClockState(cindex, wordclock.getState());
                        delay(100);
                    }
                }
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
    enum menuitem_t {set, nightmode, brightness, temperature, count};
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
                case nightmode:
                    state = ST_NIGHTMODE;
                    break;
                case brightness:
                    state = ST_BRIGHTNESS;
                    break;
                case temperature:
                    state = ST_TEMPERATURE;
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
            case nightmode:
                wordclock.update(Wordclock::NIGHTMODE);
                sendClockState(cindex, Wordclock::NIGHTMODE);
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

    enum setting_t {sinit, shour, smin5, smin1, ssec, sampm};
    static setting_t setting = sinit;

    static byte hour;
    static byte minute;
    static byte second;
    static bool pm = false;

    static int lastevent;
    static int lastupdate;
    static int elapsed;

    static WordClockState WCstate;

    if (setting == sinit) {
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
                case sampm:
                    pm = !pm;
                    break;
            }
            break;
        case EVT_SET:
            switch (setting) {
                case shour:
                    setting = smin1;
                    break;
                case smin5:
                    setting = shour;
                    break;
                case smin1:
                    setting = sampm;
                    Clock.setMinute(minute);
                    break;
                case sampm:
                    setting = ssec;
                    if (pm) {
                        hour += 12;
                        hour %= 24;
                    }
                    Clock.setHour(hour);
                    break;
                case ssec:
                    setting = sinit;
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
                case sampm:
                    pm = !pm;
                    break;
            }
            break;
        case EVT_TIMEOUT:
            setting = sinit;
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
                case sampm:
                    if (pm) {
                        WCstate = Wordclock::PM;
                    } else {
                        WCstate = Wordclock::AM;
                    }
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
                case sampm:
                    WCstate = WordClockState({0, 0, 0, 0, 0, 0, 0, 0});
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

void prog_nightmode(state_t& state, event_t& event) {

    enum setting_t {sactive, soffmin5, soffhour, sonmin5, sonhour};
    static setting_t setting = sactive;

    static ntime_t buffer_ton = {ton.hour, ton.min5};
    static ntime_t buffer_toff = {toff.hour, toff.min5};
    static bool buffer_nightmode = nightmode;

    static byte lastevent;
    static byte lastupdate;
    static byte elapsed;

    static WordClockState WCstate;

    switch (event) {
        case EVT_UP:
            switch (setting) {
                case sactive:
                    buffer_nightmode = !buffer_nightmode;
                    break;
                case sonmin5:
                    buffer_ton.min5 += 5;
                    buffer_ton.min5 %= 60;
                    break;
                case sonhour:
                    buffer_ton.hour += 1;
                    buffer_ton.hour %= 12;
                    break;
                case soffmin5:
                    buffer_toff.min5 += 5;
                    buffer_toff.min5 %= 60;
                    break;
                case soffhour:
                    buffer_ton.hour += 1;
                    buffer_ton.hour %= 12;
                    break;
            }
            break;
        case EVT_SET:
            switch (setting) {
                case sactive:
                    if (buffer_nightmode) {
                        wordclock.clear();
                        wordclock.update(Wordclock::TOFF);
                        wordclock.show();
                        sendClockState(cindex, Wordclock::TOFF);
                        setting = soffmin5;
                        delay(1000);
                    } else {
                        nightmode = false;
                        state = ST_CLOCK;
                        event_buffer = EVT_CLOCK;
                    }
                    break;
                case soffmin5:
                    setting = soffhour;
                    break;
                case soffhour:
                    wordclock.clear();
                    wordclock.update(Wordclock::TON);
                    wordclock.show();
                    sendClockState(cindex, Wordclock::TON);
                    setting = sonmin5;
                    delay(1000);
                    break;
                case sonmin5:
                    setting = sonhour;
                    break;
                case sonhour:
                    setting = sactive;
                    ton.hour = buffer_ton.hour;
                    ton.min5 = buffer_ton.min5;
                    toff.hour = buffer_toff.hour;
                    toff.min5 = buffer_toff.min5;
                    nightmode = true;
                    state = ST_CLOCK;
                    event_buffer = EVT_CLOCK;
                    break;
            }
        case EVT_DOWN:
            switch (setting) {
                case sactive:
                    buffer_nightmode = !buffer_nightmode;
                    break;
                case sonmin5:
                    buffer_ton.min5 += 55;
                    buffer_ton.min5 %= 60;
                    break;
                case sonhour:
                    buffer_ton.hour += 11;
                    buffer_ton.hour %= 12;
                    break;
                case soffmin5:
                    buffer_toff.min5 += 55;
                    buffer_toff.min5 %= 60;
                    break;
                case soffhour:
                    buffer_ton.hour += 11;
                    buffer_ton.hour %= 12;
                    break;
            }
            break;
        case EVT_TIMEOUT:
            setting = sactive;
            state = ST_CLOCK;
            event_buffer = EVT_CLOCK;
            break;
    }

    if (elapsed != lastupdate) {
        if (elapsed % 2) {
            switch (setting) {
                case sonmin5:
                case sonhour:
                    Wordclock::getState(WCstate, buffer_ton.hour, buffer_ton.min5, 0);
                    break;
                case soffmin5:
                case soffhour:
                    Wordclock::getState(WCstate, buffer_toff.hour, buffer_toff.min5, 0);
                    break;
                case sactive:
                    if (buffer_nightmode) {
                        WCstate = Wordclock::ON;
                    } else {
                        WCstate = Wordclock::OFF;
                    }
                    break;
            }
        } else {
            switch (setting) {
                case sonhour:
                case soffhour:
                    WCstate &= (~Wordclock::MSKHOUR);
                    break;
                case sonmin5:
                case soffmin5:
                    WCstate &= (~Wordclock::MSKMIN5);
                    break;
                case sactive:
                    WCstate = WordClockState({0, 0, 0, 0, 0, 0, 0, 0});
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
                Serial.print(F("Brightness: "));
                Serial.println(brightness);
                delay(10);
            }
            break;
        case EVT_DOWN:
            brightness--;
            break;
        case EVT_DOWN_HOLD:
            while (!digitalRead(BDOWN)) {
                wordclock.clear();
                wordclock.setBrightness(--brightness);
                wordclock.update();
                wordclock.show();
                Serial.print(F("Brightness: "));
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
        Serial.print(F("Brightness: "));
        Serial.println(brightness);
    }
    return;
}

void prog_temperature(state_t& state, event_t& event) {
    state = ST_CLOCK;
    event = EVT_CLOCK;
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
        Serial.print(F(" "));
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
