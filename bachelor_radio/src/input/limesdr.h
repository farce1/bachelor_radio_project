/*
 *    Copyright (C) 2020
 *    Albrecht Lohofener (albrechtloh@gmx.de)
 *
 *    This file is based on Qt-DAB
 *    Copyright (C) 2014 .. 2017
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *
 *
*/
#ifndef __LIMESDR__
#define __LIMESDR__

#include <vector>
#include <thread>
#include <lime/LimeSuite.h>

#include "virtual_input.h"
#include "dab-constants.h"
#include "MathHelper.h"
#include "ringbuffer.h"


class CLimeSDR : public CVirtualInput {
public:
    CLimeSDR(RadioControllerInterface& radioController);
    ~CLimeSDR(void);
    CLimeSDR(const CLimeSDR&) = delete;
    CLimeSDR& operator=(const CLimeSDR&) = delete;

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
    void setVFOFrequency(int32_t);
    int32_t	getVFOFrequency();

private:
    void limesdr_thread_run(void);

    RadioControllerInterface& radioController;
    lms_device_t *theDevice;
    lms_name_t	antennas[10];
    lms_stream_meta_t meta;
    lms_stream_t stream;
    std::thread limesdr_thread;

    bool running = false;
    int freq = 0;
    int currentLinearityGain = 10;

    bool sw_agc = false;
    RingBuffer<DSPCOMPLEX> SampleBuffer;
    RingBuffer<DSPCOMPLEX> SpectrumSampleBuffer;
};

#endif // __LIMESDR__
