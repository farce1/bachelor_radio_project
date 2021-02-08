/*
 *    Copyright (C) 2018
 *    Matthias P. Braendli (matthias.braendli@mpb.li)
 *
 *    Copyright (C) 2017
 *    Albrecht Lohofener (albrechtloh@gmx.de)
 *
 */

#ifndef CINPUTFACTORY_H
#define CINPUTFACTORY_H

#include <string>

#include "virtual_input.h"
#include "radio-controller.h"

class CInputFactory
{
public:
    static CVirtualInput* GetDevice(RadioControllerInterface& radioController, const std::string& Device);
    static CVirtualInput* GetDevice(RadioControllerInterface& radioController, const CDeviceID deviceId);

private:
    static CVirtualInput* GetAutoDevice(RadioControllerInterface& radioController);
    static CVirtualInput* GetManualDevice(RadioControllerInterface& radioController, const std::string& Device);
};

#endif // CINPUTFACTORY_H
