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
