/*
Copyright © 2022 Jacob Nuernberg

Permission is hereby granted, free of charge, to any person obtaining a copy 
of this software and associated documentation files (the “Software”), to deal 
in the Software without restriction, including without limitation the rights 
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell 
copies of the Software, and to permit persons to whom the Software is 
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all 
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE 
SOFTWARE.
*/


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

const WordClockState Wordclock::UHR({0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x6000, 0x0000, 0x0002});

const WordClockState Wordclock::MSKMIN5({0x0160, 0x1e68, 0xe780, 0x7000, 0x000e, 0x00ff, 0x07e0, 0x0000});
const WordClockState Wordclock::MSKHOUR({0xca00, 0xe007, 0x007f, 0x0dfe, 0xf9b0, 0x1f00, 0xe00f, 0x0000});

// ===== Constructors ======
Wordclock::Wordclock(uint16_t pin) :
    Adafruit_NeoPixel(LED_COUNT, pin, NEO_GRBW + NEO_KHZ800),
    hour(0),
    minute(0),
    second(0),
    color{0, 255, 0, 0},
    state{0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000} {
        update();
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
