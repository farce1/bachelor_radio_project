/*
 *    Copyright (C) 2018
 *    Matthias P. Braendli (matthias.braendli@mpb.li)
 *
 *    Copyright (C) 2017
 *    Albrecht Lohofener (albrechtloh@gmx.de)
 *
 *    This file is based on SDR-J
 *    Copyright 2015 by Andrea Montefusco IW0HDV
 *    Copyright (C) 2010, 2011, 2012, 2013
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *
 *
*/
#ifndef __AIRSPY_RADIO__
#define __AIRSPY_RADIO__

#include "virtual_input.h"
#include "dab-constants.h"
#include "MathHelper.h"
#include "ringbuffer.h"

#include <vector>

#ifndef __MINGW32__
#include "libairspy/airspy.h"
#else
#include "airspy.h"
#endif

enum class CAirspy_IOCTL {
    SET_BIAS_TEE
};

class CAirspy : public CVirtualInput {
public:
    CAirspy(RadioControllerInterface& radioController);
    ~CAirspy(void);
    CAirspy(const CAirspy&) = delete;
    CAirspy& operator=(const CAirspy&) = delete;

    void setFrequency(int nf);
    int getFrequency(void) const;
    bool restart(void);
    bool is_ok(void);
    void stop(void);
    void reset(void);
    int32_t getSamples(DSPCOMPLEX* Buffer, int32_t Size);
    std::vector<DSPCOMPLEX> getSpectrumSamples(int size);
    int32_t getSamplesToRead(void);
    float getGain(void) const;
    float setGain(int gain);
    int getGainCount(void);
    void setAgc(bool agc);
    std::string getDescription(void);
    bool setDeviceParam(DeviceParam param, int value);

    CDeviceID getID(void);

private:
    RadioControllerInterface& radioController;

    const int AIRSPY_SAMPLERATE = 4096000;
    const int AIRSPY_GAIN_MIN = 0;
    const int AIRSPY_GAIN_MAX = 21;

    bool running = false;
    int freq = 0;

    size_t num_frames = 0;

    bool sw_agc = false;
    int currentLinearityGain = 10;
    RingBuffer<DSPCOMPLEX> SampleBuffer;
    RingBuffer<DSPCOMPLEX> SpectrumSampleBuffer;
    struct airspy_device *device;

    static int callback(airspy_transfer_t*);
    int data_available(const DSPCOMPLEX* buf, size_t num_samples);
};

#endif
