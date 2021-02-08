/*
 *    Copyright (C) 2018
 *    Albrecht Lohofener (albrechtloh@gmx.de)
 *
 */

#ifndef DECODER_ADAPTER_H
#define DECODER_ADAPTER_H

#include <memory>
#include <vector>
#include <cstdint>
#include <cmath>
#include <cstdio>
#include "dab-processor.h"
#include "pad_decoder.h"
#include "radio-controller.h"
#include "subchannel_sink.h"
#include "dab_decoder.h"
#include "dabplus_decoder.h"

class DecoderAdapter: public DabProcessor, public SubchannelSinkObserver, public PADDecoderObserver
{
    public:
        DecoderAdapter(ProgrammeHandlerInterface& mr,
                     int16_t bitRate,
                     AudioServiceComponentType &dabModus,
                     const std::string& dumpFileName);

        virtual void addtoFrame(uint8_t *v);

        // SubchannelSinkObserver impl
        virtual void FormatChange(const AUDIO_SERVICE_FORMAT& /*format*/);
        virtual void StartAudio(int /*samplerate*/, int /*channels*/, bool /*float32*/);
        virtual void PutAudio(const uint8_t* /*data*/, size_t /*len*/);
        virtual void ProcessPAD(const uint8_t* /*xpad_data*/, size_t /*xpad_len*/, bool /*exact_xpad_len*/, const uint8_t* /*fpad_data*/);
        virtual void AudioError(const std::string& /*hint*/);
        virtual void AudioWarning(const std::string& /*hint*/);
        virtual void FECInfo(int /*total_corr_count*/, bool /*uncorr_errors*/);

        // PADDecoderObserver impl
        virtual void PADChangeDynamicLabel(const DL_STATE& dl);
        virtual void PADChangeSlide(const MOT_FILE& slide);
        virtual void PADLengthError(size_t announced_xpad_len, size_t xpad_len);

    private:
        int16_t bitRate;
        int frameErrorCounter = 0;
        ProgrammeHandlerInterface& myInterface;
        std::unique_ptr<SubchannelSink> decoder;
        PADDecoder padDecoder;

        struct FILEDeleter{ void operator()(FILE* fd){ if (fd) fclose(fd); }};
        std::unique_ptr<FILE, FILEDeleter> dumpFile;

        int audioSamplerate = 0;
        int audioChannels = 0;
        std::string audioFormat;
};
#endif // DECODER_ADAPTER_H

