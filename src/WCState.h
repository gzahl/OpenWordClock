#ifndef __WCState_H_INCLUDED__
#define __WCState_H_INCLUDED__

// #include <Arduino.h>

#define WCSTATESIZE 8

class WordClockState {
private:
    unsigned short value[WCSTATESIZE];

public:
    WordClockState ();
    WordClockState (unsigned short val[]);
    WordClockState (
        unsigned short a,
        unsigned short b,
        unsigned short c,
        unsigned short d,
        unsigned short e,
        unsigned short f,
        unsigned short g,
        unsigned short h
    );

    unsigned short getWord(const short index);
    bool getBit(const short index);


    WordClockState operator~ ();
    WordClockState operator& (const WordClockState& rhs);
    WordClockState& operator&= (const WordClockState& rhs);
    WordClockState operator| (const WordClockState& rhs);
    WordClockState& operator|= (const WordClockState& rhs);
    WordClockState operator^ (const WordClockState& rhs);
    WordClockState& operator^= (const WordClockState& rhs);
    WordClockState operator>> (const int rhs);
    WordClockState& operator>>= (const int rhs);
    WordClockState operator<< (const int rhs);
    WordClockState& operator<<= (const int rhs);

    WordClockState& operator&= (const unsigned short rhs);
    WordClockState& operator|= (const unsigned short rhs);
    short operator& (const short& rhs);
    short operator| (const short& rhs);
};

#endif
