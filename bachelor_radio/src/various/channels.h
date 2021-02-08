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
 */

#ifndef CHANNELS_H
#define CHANNELS_H

#include <map>
#include <string>

#define NUMBEROFCHANNELS 54

class Channels
{
public:
    Channels();
    int getFrequency(const std::string& channelName);
    std::string getNextChannel(void);
    std::string getCurrentChannel(void);
    int getCurrentFrequency(void);
    int getCurrentIndex(void);
    std::string getChannelForFrequency(int frequency);

    static std::string firstChannel;

private:
    std::string getChannelNameAtIndex(int index);

    std::map<std::string, int> frequencyMap;
    int currentFrequencyIndex;
    std::string currentChannel;
    int currentFrequency;
};

#endif // CCHANNELS_H
