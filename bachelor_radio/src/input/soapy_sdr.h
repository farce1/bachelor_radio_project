/*
 *    Copyright (C) 2018
 *    Matthias P. Braendli (matthias.braendli@mpb.li)
 *    Albrecht Lohofener (albrechtloh@gmx.de)
 *
 */

#pragma once

#include <atomic>
#include <thread>
#include "virtual_input.h"
#include "ringbuffer.h"
#include <SoapySDR/Version.hpp>
#include <SoapySDR/Modules.hpp>
#include <SoapySDR/Registry.hpp>
#include <SoapySDR/Device.hpp>

class CSoapySdr_Thread;

class CSoapySdr : public CVirtualInput
{
public:
    CSoapySdr(RadioControllerInterface& radioController);
    ~CSoapySdr();
    CSoapySdr(const CSoapySdr&) = delete;
    CSoapySdr operator=(const CSoapySdr&) = delete;

    virtual void setFrequency(int Frequency);
    virtual int getFrequency(void) const;
    virtual bool restart(void);
    virtual bool is_ok(void);
    virtual void stop(void);
    virtual void reset(void);
    virtual int32_t getSamples(DSPCOMPLEX* Buffer, int32_t Size);
    virtual std::vector<DSPCOMPLEX> getSpectrumSamples(int size);
    virtual int32_t getSamplesToRead(void);
    virtual float setGain(int gainIndex);
    virtual float getGain(void) const;
    virtual int getGainCount(void);
    virtual void setAgc(bool AGC);
    virtual std::string getDescription(void);
    virtual CDeviceID getID(void);
    virtual bool setDeviceParam(DeviceParam param, const std::string& value);

private:
    void setDriverArgs(const std::string& args);
    void setAntenna(const std::string& antenna);
    void setClockSource(const std::string& clock_source);
    void decreaseGain();
    void increaseGain();

    RadioControllerInterface& radioController;
    int m_freq = 0;
    std::string m_driver_args;
    std::string m_antenna;
    std::string m_clock_source;
    SoapySDR::Device *m_device = nullptr;
    std::atomic<bool> m_running = ATOMIC_VAR_INIT(false);
    bool m_sw_agc = false;

    RingBuffer<DSPCOMPLEX> m_sampleBuffer;
    RingBuffer<DSPCOMPLEX> m_spectrumSampleBuffer;

    std::vector<double> m_gains;

    std::thread m_thread;
    void workerthread(void);
    void process(SoapySDR::Stream *stream);
};

