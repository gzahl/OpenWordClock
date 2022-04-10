#include <Arduino.h>
#include "Wordclock.h"
// #include <iostream>

// ===== Static Constants =====

const WordClockState Wordclock::ESIST({0x0010, 0x0080, 0x1800, 0x8000, 0x0000, 0x0000, 0x0000, 0x0000});

const WordClockState Wordclock::MIN5[12] = {
    {0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000}, /* __:00 */
    {0x0000, 0x0800, 0x8100, 0x1000, 0x0000, 0x0018, 0x0180, 0x0000}, /* __:05 */
    {0x0020, 0x0a40, 0x8500, 0x1000, 0x0000, 0x0000, 0x0000, 0x0000}, /* __:10 */
    {0x0000, 0x0800, 0xc100, 0x3000, 0x0004, 0x0042, 0x0420, 0x0000}, /* __:15 */
    {0x0000, 0x0800, 0xa100, 0x5000, 0x0002, 0x0024, 0x0240, 0x0000}, /* __:20 */
    {0x0100, 0x1008, 0x0080, 0x0000, 0x0008, 0x0099, 0x0180, 0x0000}, /* __:25 */
    {0x0100, 0x1008, 0x0080, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000}, /* __:30 */
    {0x0100, 0x1808, 0x8180, 0x1000, 0x0000, 0x0018, 0x0180, 0x0000}, /* __:35 */
    {0x0000, 0x0000, 0x2000, 0x4000, 0x000a, 0x00a5, 0x0240, 0x0000}, /* __:40 */
    {0x0000, 0x0000, 0x4000, 0x2000, 0x000c, 0x00c3, 0x0420, 0x0000}, /* __:45 */
    {0x0020, 0x0240, 0x0400, 0x0000, 0x0008, 0x0081, 0x0000, 0x0000}, /* __:50 */
    {0x0000, 0x0000, 0x0000, 0x0000, 0x0008, 0x0099, 0x0180, 0x0000} /* __:55 */
};

const WordClockState Wordclock::HOURS[12] = {
    {0x0000, 0x0000, 0x0000, 0x0800, 0x8010, 0x0100, 0x0008, 0x0000}, /* 00:__ or 12:00*/
    {0x0000, 0x2000, 0x0040, 0x0402, 0x0000, 0x0000, 0x0000, 0x0000}, /* 01:__ */
    {0x0200, 0x2004, 0x0040, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000}, /* 02:__ */
    {0x0000, 0x4002, 0x0020, 0x0004, 0x0000, 0x0000, 0x0000, 0x0000}, /* 03:__ */
    {0x0000, 0x0000, 0x0000, 0x0000, 0x1000, 0x0800, 0x8001, 0x0000}, /* 04:__ */
    {0x0000, 0x0000, 0x0000, 0x0000, 0x2000, 0x0400, 0x4002, 0x0000}, /* 05:__ */
    {0x4000, 0x0000, 0x0006, 0x0060, 0x0000, 0x0000, 0x0000, 0x0000}, /* 06:__ */
    {0x0000, 0x0000, 0x0000, 0x0400, 0x4020, 0x0200, 0x2004, 0x0000}, /* 07:__ */
    {0x8000, 0x0000, 0x0009, 0x0010, 0x0000, 0x0000, 0x0000, 0x0000}, /* 08:__ */
    {0x0000, 0x0000, 0x0010, 0x0108, 0x0080, 0x0000, 0x0000, 0x0000}, /* 09:__ */
    {0x0000, 0x0000, 0x0000, 0x0080, 0x0900, 0x1000, 0x0000, 0x0000}, /* 10:__ */
    {0x0800, 0x8001, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000} /* 11:__ */
};

const WordClockState Wordclock::SEC[20] = {
    {0x0000, 0x0000, 0x0000, 0x0000, 0x28fc, 0x9324, 0xfc50, 0x0000}, /* _0 */
    {0x0000, 0x0000, 0x0000, 0x0000, 0x0908, 0x9fe2, 0x0000, 0x0001}, /* _1 */
    {0x0000, 0x0000, 0x0000, 0x0000, 0x1904, 0x9424, 0x1c44, 0x0001}, /* _2 */
    {0x0000, 0x0000, 0x0000, 0x0000, 0x0882, 0x90a4, 0xe268, 0x0000}, /* _3 */
    {0x0000, 0x0000, 0x0000, 0x0000, 0x2070, 0x8441, 0x404f, 0x0000}, /* _4 */
    {0x0000, 0x0000, 0x0000, 0x0000, 0x889e, 0x9124, 0xe248, 0x0000}, /* _5 */
    {0x0000, 0x0000, 0x0000, 0x0000, 0x88fc, 0x9124, 0xe448, 0x0000}, /* _6 */
    {0x0000, 0x0000, 0x0000, 0x0000, 0x0802, 0x0c24, 0x0e4c, 0x0000}, /* _7 */
    {0x0000, 0x0000, 0x0000, 0x0000, 0x88ec, 0x9124, 0xec48, 0x0000}, /* _8 */
    {0x0000, 0x0000, 0x0000, 0x0000, 0x489c, 0x9224, 0xfc44, 0x0000}, /* _9 */
    {0x8fc0, 0x3242, 0xc509, 0x000f, 0x0000, 0x0000, 0x0000, 0x0000}, /* 0_ */
    {0x9080, 0xfe20, 0x0009, 0x0010, 0x0000, 0x0000, 0x0000, 0x0000}, /* 1_ */
    {0x98c0, 0x2242, 0xc489, 0x0010, 0x0000, 0x0000, 0x0000, 0x0000}, /* 2_ */
    {0x8820, 0x0a40, 0x2689, 0x000e, 0x0000, 0x0000, 0x0000, 0x0000}, /* 3_ */
    {0x0700, 0x4412, 0x04f8, 0x0004, 0x0000, 0x0000, 0x0000, 0x0000}, /* 4_ */
    {0x89e0, 0x1248, 0x2489, 0x000e, 0x0000, 0x0000, 0x0000, 0x0000}, /* 5_ */
    {0x8fc0, 0x1248, 0x4489, 0x000e, 0x0000, 0x0000, 0x0000, 0x0000}, /* 6_ */
    {0x8020, 0xc240, 0xe4c0, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000}, /* 7_ */
    {0x8ec0, 0x1248, 0xc489, 0x000e, 0x0000, 0x0000, 0x0000, 0x0000}, /* 8_ */
    {0x89c0, 0x2244, 0xc449, 0x000f, 0x0000, 0x0000, 0x0000, 0x0000} /* 9_ */
};

const WordClockState Wordclock::UHR({0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x6000, 0x0000, 0x0002});

const WordClockState Wordclock::AM({0x1f00, 0x4412, 0x0120, 0x001f, 0x1fc, 0x0301, 0xfc10, 0x0001});
const WordClockState Wordclock::PM({0x1fc0, 0x4422, 0x8220, 0x0003, 0x1fc, 0x0301, 0xfc10, 0x0001});

const WordClockState Wordclock::ON({0x0000, 0x041f, 0x8209, 0x000f, 0x41e0, 0x8200, 0x0003, 0x0000});
const WordClockState Wordclock::OFF({0x8f80, 0x0420, 0x01f1, 0x1f80, 0x0024, 0x1f82, 0x0424, 0x0000});

const WordClockState Wordclock::TON({0x0010, 0x3f80, 0x1800, 0x0178, 0x0380, 0x3800, 0x0001, 0x0003});
const WordClockState Wordclock::TOFF({0xc010, 0xbf81, 0x181e, 0x07c0, 0x8090, 0x3e00, 0x1009, 0x0000});

const WordClockState Wordclock::MSKMIN5({0x0160, 0x1e68, 0xe780, 0x7000, 0x000e, 0x00ff, 0x07e0, 0x0000});
const WordClockState Wordclock::MSKHOUR({0xca00, 0xe007, 0x007f, 0x0dfe, 0xf9b0, 0x1f00, 0xe00f, 0x0000});

const WordClockState Wordclock::INITPIC({0x078f, 0x0221, 0x1805, 0xfc60, 0x0611, 0x1029, 0x7821, 0x0000});
const WordClockState Wordclock::BRIGHTNESS({0x4200, 0x0444, 0x80e1, 0xdf4f, 0xe088, 0x5040, 0x2044, 0x0000});
const WordClockState Wordclock::NIGHTMODE({0x0780, 0xfe3f, 0x361d, 0x8070, 0x7241, 0x0440, 0x0470, 0x0000});
const WordClockState Wordclock::SET({0x0880, 0x6429, 0xc000, 0x290f, 0x0084, 0x0040, 0x043f, 0x0000});

// ===== Constructors ======
Wordclock::Wordclock(uint16_t pin) :
    Adafruit_NeoPixel(LED_COUNT, pin, NEO_GRBW + NEO_KHZ800),
    hour(0),
    minute(0),
    second(0),
    color{0, 255, 0, 0},
    state{0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000} {
        update(INITPIC);
    };

// ===== Member Functions ======

void Wordclock::update(const WordClockState& thestate) {
    state = thestate;
    uint32_t cvalue;
    if (color[3]) {
        cvalue = Color(0, 0, 0, color[3]);
    } else {
        cvalue = Color(color[0], color[1], color[2]);
    }
    for (size_t index = 0; index < 114; index++) {
        if ( (thestate>>index) & 0x01) {
            setPixelColor(index, cvalue);
        }
    }
    return;
};

void Wordclock::update() {
    unsigned short hourind;
    if (minute < 25) {
        hourind = hour%12;
    } else {
        hourind = (hour+1)%12;
    }

    unsigned short min1ind = minute%5;
    unsigned short min5ind = (minute-min1ind)/5;

    state = ESIST | MIN5[min5ind] | HOURS[hourind];
    if (min5ind == 0) {
        state |= UHR;
    }
    // unsigned short minstate;
    // minstate = pow(2, min1ind)- 1;
    state |= round(pow(2, min1ind))-1;
    update(state);
    return;
}

WordClockState Wordclock::getState() {
  return state;
}

static void Wordclock::getState(
    WordClockState& clockstate,
    const byte& hour,
    const byte& minute,
    const byte& second
) {
    unsigned short hourind;
    if (minute < 25) {
        hourind = hour%12;
    } else {
        hourind = (hour+1)%12;
    }

    unsigned short min1ind = minute%5;
    unsigned short min5ind = (minute-min1ind)/5;

    clockstate = Wordclock::ESIST | Wordclock::MIN5[min5ind] | Wordclock::HOURS[hourind];
    if (min5ind == 0) {
        clockstate |= Wordclock::UHR;
    }
    // unsigned short minstate;
    // minstate = pow(2, min1ind)- 1;
    clockstate |= round(pow(2, min1ind))-1;
    return clockstate;
}
