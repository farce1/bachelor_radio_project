/*
 *    Copyright (C) 2017
 *    Albrecht Lohofener (albrechtloh@gmx.de)
 *
 *
 */

#include "null_device.h"

CNullDevice::CNullDevice()
{

}

void CNullDevice::setFrequency(int Frequency)
{
    (void) Frequency;
}

int CNullDevice::getFrequency(void) const
{
    return 0;
}

bool CNullDevice::restart()
{
    return false;
}

bool CNullDevice::is_ok()
{
    return true;
}

void CNullDevice::stop()
{

}

void CNullDevice::reset()
{

}

int32_t CNullDevice::getSamples(DSPCOMPLEX *Buffer, int32_t Size)
{
    memset((void*)Buffer, 0, Size * sizeof(DSPCOMPLEX));

    return Size;
}

std::vector<DSPCOMPLEX> CNullDevice::getSpectrumSamples(int size)
{
    std::vector<DSPCOMPLEX> sampleBuffer(size);
    std::fill(sampleBuffer.begin(), sampleBuffer.end(), 0);
    return sampleBuffer;
}

int32_t CNullDevice::getSamplesToRead()
{
    return 0;
}

float CNullDevice::getGain() const
{
    return 0;
}

float CNullDevice::setGain(int Gain)
{
    (void) Gain;

    return 0;
}

int CNullDevice::getGainCount()
{
    return 0;
}

void CNullDevice::setAgc(bool AGC)
{
    (void) AGC;
}

std::string CNullDevice::getDescription()
{
    return "Null device";
}

CDeviceID CNullDevice::getID()
{
    return CDeviceID::NULLDEVICE;
}
