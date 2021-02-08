/*
 *    Copyright (C) 2018
 *    Matthias P. Braendli (matthias.braendli@mpb.li)
 *
 *    Copyright (C) 2017
 *    Albrecht Lohofener (albrechtloh@gmx.de)
 *
 *    This file is based on SDR-J
 *    Copyright (C) 2010, 2011, 2012
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *
 *
 */

#include <string>
#include <iostream>
#include <memory>
#include "radio-receiver.h"

using namespace std;

const char* fftPlacementMethodToString(FFTPlacementMethod fft_placement)
{
    switch (fft_placement) {
        case FFTPlacementMethod::EarliestPeakWithBinning:
            return "EarliestPeakWithBinning";
        case FFTPlacementMethod::StrongestPeak:
            return "StrongestPeak";
        case FFTPlacementMethod::ThresholdBeforePeak:
            return "ThresholdBeforePeak";
    }
    throw std::logic_error("Unhandled fft placement");
}

const char* freqSyncMethodToString(FreqsyncMethod method)
{
    switch (method) {
        case FreqsyncMethod::CorrelatePRS:
            return "CorrelatePRS";
        case FreqsyncMethod::GetMiddle:
            return "GetMiddle";
        case FreqsyncMethod::PatternOfZeros:
            return "PatternOfZeros";
    }
    throw std::logic_error("Unhandled freqsyncMethod placement");
}

RadioReceiver::RadioReceiver(
                RadioControllerInterface& rci,
                InputInterface& input,
                RadioReceiverOptions rro,
                int transmission_mode) :
    params(transmission_mode),
    mscHandler(params, false),
    ficHandler(rci),
    ofdmProcessor(input,
        params,
        rci,
        mscHandler,
        ficHandler,
        rro)
{ }

void RadioReceiver::restart(bool doScan)
{
    ofdmProcessor.set_scanMode(doScan);
    mscHandler.stopProcessing();
    ficHandler.clearEnsemble();
    ofdmProcessor.restart();
}

void RadioReceiver::restart_decoder()
{
    mscHandler.stopProcessing();
    ficHandler.clearEnsemble();
}

void RadioReceiver::stop()
{
    ofdmProcessor.stop();
    mscHandler.stopProcessing();
    ficHandler.clearEnsemble();
}

void RadioReceiver::setReceiverOptions(const RadioReceiverOptions rro)
{
    string fsm;
    switch (rro.freqsyncMethod) {
        case FreqsyncMethod::GetMiddle: fsm = "GetMiddle"; break;
        case FreqsyncMethod::CorrelatePRS: fsm = "CorrelatePRS"; break;
        case FreqsyncMethod::PatternOfZeros: fsm = "PatternOfZeros"; break;
    }

    clog << "New Receiver Options: " <<
        "TII: " << rro.decodeTII <<
        " disable coarse corr: " << rro.disableCoarseCorrector <<
        " freqsync: " << fsm <<
        " fft placement: " << fftPlacementMethodToString(rro.fftPlacementMethod) << endl;
    ofdmProcessor.setReceiverOptions(rro);
}

bool RadioReceiver::playSingleProgramme(ProgrammeHandlerInterface& handler,
        const std::string& dumpFileName, const Service& s)
{
    return playProgramme(handler, s, dumpFileName, true);
}

bool RadioReceiver::addServiceToDecode(ProgrammeHandlerInterface& handler,
        const std::string& dumpFileName, const Service& s)
{
    return playProgramme(handler, s, dumpFileName, false);
}

bool RadioReceiver::removeServiceToDecode(const Service& s)
{
    const auto comps = ficHandler.fibProcessor.getComponents(s);
    for (const auto& sc : comps) {
        if (sc.transportMode() == TransportMode::Audio) {
            const auto& subch = ficHandler.fibProcessor.getSubchannel(sc);
            if (subch.valid()) {
                return mscHandler.removeSubchannel(subch);
            }
        }
    }
    return false;
}

bool RadioReceiver::playProgramme(ProgrammeHandlerInterface& handler,
        const Service& s, const std::string& dumpFileName, bool unique)
{
    const auto comps = ficHandler.fibProcessor.getComponents(s);
    for (const auto& sc : comps) {
        if (sc.transportMode() == TransportMode::Audio) {
            const auto& subch = ficHandler.fibProcessor.getSubchannel(sc);

            if (subch.valid()) {
                if (unique) {
                    mscHandler.stopProcessing();
                }

                if (sc.audioType() == AudioServiceComponentType::DAB ||
                    sc.audioType() == AudioServiceComponentType::DABPlus) {
                    mscHandler.addSubchannel(
                            handler, sc.audioType(), dumpFileName, subch);
                    return true;
                }
            }
        }
    }

    return false;
}

uint16_t RadioReceiver::getEnsembleId(void) const
{
    return ficHandler.fibProcessor.getEnsembleId();
}

uint8_t RadioReceiver::getEnsembleEcc(void) const
{
    return ficHandler.fibProcessor.getEnsembleEcc();
}

DabLabel RadioReceiver::getEnsembleLabel(void) const
{
    return ficHandler.fibProcessor.getEnsembleLabel();
}

std::vector<Service> RadioReceiver::getServiceList(void) const
{
    return ficHandler.fibProcessor.getServiceList();
}

Service RadioReceiver::getService(uint32_t sId) const
{
    return ficHandler.fibProcessor.getService(sId);
}

std::list<ServiceComponent> RadioReceiver::getComponents(const Service& s) const
{
    return ficHandler.fibProcessor.getComponents(s);
}

bool RadioReceiver::serviceHasAudioComponent(const Service& s) const
{
    for (const auto& sc : getComponents(s)) {
        if (sc.transportMode() == TransportMode::Audio and
                (sc.audioType() == AudioServiceComponentType::DAB or
                 sc.audioType() == AudioServiceComponentType::DABPlus)) {
            return true;
        }
    }

    return false;
}

Subchannel RadioReceiver::getSubchannel(const ServiceComponent& sc) const
{
    return ficHandler.fibProcessor.getSubchannel(sc);
}

DABParams& RadioReceiver::getParams()
{
    return params;
}
