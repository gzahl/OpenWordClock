#include "WCState.h"
#include <Arduino.h>
// #include <iostream>              // Uncomment when testing the class standalone

#define WCSTATESIZE 8
#define WCWORDSIZE 16

WordClockState::WordClockState() : value{0, 0, 0, 0, 0, 0, 0, 0} {
};

WordClockState::WordClockState (unsigned short val[]) : value{*val} {
};

WordClockState::WordClockState (
        unsigned short a,
        unsigned short b,
        unsigned short c,
        unsigned short d,
        unsigned short e,
        unsigned short f,
        unsigned short g,
        unsigned short h
) : value{a, b, c, d, e, f, g, h} {
};

unsigned short WordClockState::getWord (const short index) {
    return value[index];
}

bool WordClockState::getBit (const short index) {
    return ((value[index/WCWORDSIZE] >> (index%WCWORDSIZE)) & 0x1);
}

WordClockState WordClockState::operator~ (){
    WordClockState inverted;
    for (size_t index = 0; index < WCSTATESIZE; index++) {
        inverted.value[index] = ~(this->value[index]);
    }
    return inverted;
}

WordClockState& WordClockState::operator&= (const WordClockState& rhs) {
    for (size_t index = 0; index < WCSTATESIZE; index++) {
        this->value[index] &= rhs.value[index];
    }
    return *this;
}

WordClockState WordClockState::operator& (const WordClockState& rhs){
    WordClockState lhs(*this);
    lhs &= rhs;
    return lhs;
}

WordClockState& WordClockState::operator|= (const WordClockState& rhs) {
    for (size_t index = 0; index < WCSTATESIZE; index++) {
        this->value[index] |= rhs.value[index];
    }
    return *this;
}

WordClockState WordClockState::operator| (const WordClockState& rhs){
    WordClockState lhs(*this);
    lhs |= rhs;
    return lhs;
}

WordClockState& WordClockState::operator^= (const WordClockState& rhs) {
    for (size_t index = 0; index < WCSTATESIZE; index++) {
        this->value[index] ^= rhs.value[index];
    }
    return *this;
}

WordClockState WordClockState::operator^ (const WordClockState& rhs){
    WordClockState lhs(*this);
    lhs ^= rhs;
    return lhs;
}

WordClockState& WordClockState::operator>>= (const int rhs) {
    int swap = rhs / WCWORDSIZE;
    int shift = rhs % WCWORDSIZE;
    size_t index = 0;
    while (index < WCSTATESIZE - swap - 1) {
        value[index] = (value[index + swap] >> shift) | (value[index + swap + 1] << (WCWORDSIZE - shift));
        index++;
    }
    value[index] = value[index + swap] >> shift;
    index++;
    while (index < WCSTATESIZE) {
        value[index] = 0;
        index++;
    }
    return *this;
}

WordClockState WordClockState::operator>> (const int rhs){
    WordClockState lhs(*this);
    lhs >>= rhs;
    return lhs;
}

WordClockState& WordClockState::operator<<= (const int rhs) {
    int swap = rhs / WCWORDSIZE;
    int shift = rhs % WCWORDSIZE;
    size_t index = WCSTATESIZE;
    while (index > swap + 1) {
        index--;
        value[index] = (value[index - swap] << shift) | (value[index - swap - 1] >> (WCWORDSIZE - shift));
    }
    index--;
    value[index] = value[0] << shift;
    while (index > 0) {
        index--;
        value[index] = 0;
    }
    return *this;
}

WordClockState WordClockState::operator<< (const int rhs){
    WordClockState lhs(*this);
    lhs <<= rhs;
    return lhs;
}

WordClockState& WordClockState::operator&= (const unsigned short rhs) {
    value[0] &= rhs;
    return *this;
}

WordClockState& WordClockState::operator|= (const unsigned short rhs) {
    value[0] |= rhs;
    return *this;
}

short WordClockState::operator& (const short& rhs){
    return value[0] & rhs;
}

short WordClockState::operator| (const short& rhs){
    return value[0] | rhs;
}


// ===================================================================================
// ===== Code below is for testing only. Comment out when compiling for arduino ======
// ===================================================================================


// void Stateprinter(WordClockState& state, int startbit, int passtheendbit) {
//     for (size_t index = passtheendbit; index > startbit; index--) {
//         std::cout << state.getBit(index-1);
//         // state.getBit(index-1);
//         if (index%8 == 1) {
//             std::cout << " ";
//         }
//     }
// }
//
// int main() {
//
//     unsigned short initvals[] {0x8010, 0x0080, 0x1800, 0x8000, 0x0000, 0x0000, 0x0000, 0x0000};
//
//     WordClockState alpha(0x0010, 0x0080, 0x1800, 0x8000, 0x0000, 0x0000, 0x0000, 0x0000);
//     WordClockState beta(initvals);
//     std::cout << std::endl;
//
//     WordClockState delta[3] = {
//         {12, 15, 15, 0, 0, 0, 0, 15+64},
//         alpha,
//         initvals
//     };
//
//     std::cout << "alpha: ";
//     Stateprinter(alpha, 0, 32);
//     std::cout << std::endl;
//     std::cout << "shift: ";
//     beta = (alpha << 4);
//     // beta >>=4;
//     Stateprinter(beta, 0, 32);
//     std::cout << std::endl;
//
//     WordClockState gamma;
//     gamma = alpha << 1;
//
//     std::cout << "shift: ";
//     Stateprinter(gamma, 0, 32);
//     std::cout << std::endl;
//     std::cout << std::endl;
//
//     for (size_t index = 0; index < 3; index++) {
//         std::cout << "delta[" << index << "]: ";
//         Stateprinter(delta[index], 0, 32);
//         std::cout << std::endl;
//     }
//     std::cout << std::endl << std::dec;
//
//     unsigned int index = 0;
//     for (size_t row = 0; row < 10; row++) {
//         for (size_t column = 0; column < 11; column++) {
//             index = 10 * column + (row * ((column + 1) % 2))  + (9 - row) * (column % 2) + 4;
//             if ((alpha >> index) & 0x1) {
//                 std::cout << index <<  ": X, ";
//             } else {
//                 std::cout << index <<  ": -, ";
//             }
//         }
//         std::cout << std::endl;
//     }
//     return 0;
// }
