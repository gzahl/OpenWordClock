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

#ifndef __WORDCLOCK_H_INCLUDED__
#define __WORDCLOCK_H_INCLUDED__

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include "WCState.h"

#define LED_COUNT 114

class Wordclock : public Adafruit_NeoPixel
{
private:

public:


    static const WordClockState ESIST;
    static const WordClockState MIN5[12];
    static const WordClockState HOURS[12];
    static const WordClockState UHR;

    static const WordClockState MSKMIN5;
    static const WordClockState MSKHOUR;

    WordClockState state;
    byte hour;
    byte minute;
    byte second;
    byte color[4];

    // Constructor
    Wordclock(uint16_t pin);

    // update and state functions
    void update();
    void update(const WordClockState& thestate);
    WordClockState getState();

    // static class functions
    static void getState(
        WordClockState& clockstate,
        const byte& hour,
        const byte& minute,
        const byte& second
    );
};

#endif
