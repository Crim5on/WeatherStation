#pragma once


/** @returns the time waited in microseconds until pin has value val */
static inline unsigned long busyWaitUntilPinIsVal(const uint8_t pin, const bool val)
{
    unsigned long timeStampStart = micros();
    while(digitalRead(pin) != val){
        ; // wait
    }
    unsigned long timeStampStop = micros();
    unsigned long timePassed = timeStampStop - timeStampStart;
    return timePassed;
}