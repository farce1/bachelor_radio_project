/*
 *    Copyright (C) 2017
 *    Albrecht Lohofener (albrechtloh@gmx.de)
 *
 */

#ifndef CNULLDEVICE_H
#define CNULLDEVICE_H

#include "virtual_input.h"

class CNullDevice : public CVirtualInput
{
public:
    CNullDevice();

    void setFrequency(int Frequency);
    int getFrequency(void) const;
    bool restart(void);
    bool is_ok(void);
    void stop(void);
    void reset(void);
    int32_t getSamples(DSPCOMPLEX* Buffer, int32_t Size);
    std::vector<DSPCOMPLEX> getSpectrumSamples(int size);
    int32_t getSamplesToRead(void);
    float getGain(void) const;
    float setGain(int Gain);
    int getGainCount(void);
    void setAgc(bool AGC);
    std::string getDescription(void);
    CDeviceID getID(void);
};

#endif // CNULLDEVICE_H
