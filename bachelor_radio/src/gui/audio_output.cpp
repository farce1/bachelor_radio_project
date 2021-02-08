/*
 *    Copyright (C) 2018
 *    Matthias P. Braendli (matthias.braendli@mpb.li)
 *
 *    Copyright (C) 2017
 *    Albrecht Lohofener (albrechtloh@gmx.de)
 *
 */

#include <QDebug>
#include <stdio.h>

#include "audio_output.h"

CAudioThread::CAudioThread(RingBuffer<int16_t>& buffer, QObject *parent) :
    QThread(parent),
    buffer(buffer),
    audioIODevice(buffer, this),
    cardRate(48000)
{
    connect(&checkAudioBufferTimer, &QTimer::timeout,
            this, &CAudioThread::checkAudioBufferTimeout);

    // Check audio state every 1 s, start audio if bytes are available
    checkAudioBufferTimer.start(1000);

    // Move event processing of CAudioThread to this thread
    QObject::moveToThread(this);
}

CAudioThread::~CAudioThread(void)
{
    if (audioOutput != nullptr) {
        delete audioOutput;
        audioOutput = nullptr;
    }

    if (info != nullptr) {
        delete info;
        info = nullptr;
    }
}

void CAudioThread::setRate(int sampleRate)
{
    if (cardRate != sampleRate) {
        qDebug() << "Odtwarzacz audio:"
                 << "Częstotliwość próbkowania" << sampleRate << "Hz";
        cardRate = sampleRate;
        // restart audio within thread with new sample rate
        init(cardRate);
    }
}

void CAudioThread::setVolume(qreal volume)
{
    if (audioOutput != nullptr) {
        qDebug() << "Odtwarzacz audio:"
                 << "Głośność" << volume;
        audioOutput->setVolume(volume);
    }
}

void CAudioThread::init(int sampleRate)
{
    if (audioOutput != nullptr) {
        delete audioOutput;
        audioOutput = nullptr;
    }

    audioFormat.setSampleRate(sampleRate);
    audioFormat.setChannelCount(2);
    audioFormat.setSampleSize(16);
    audioFormat.setCodec("audio/pcm");
    audioFormat.setByteOrder(QAudioFormat::LittleEndian);
    audioFormat.setSampleType(QAudioFormat::SignedInt);

    info = new QAudioDeviceInfo(QAudioDeviceInfo::defaultOutputDevice());
    if (!info->isFormatSupported(audioFormat)) {
        qDebug() << "Odtwarzacz audio:"
                 << "Format \"audio/pcm\" 16-bit stereo niewspierany. Mogą wystąpić problemy z audio";
    }   

    qDebug() << "Odtwarzacz audio: Aktualnie używane audio" << info->deviceName();

//    foreach (const QAudioDeviceInfo &deviceInfo, QAudioDeviceInfo::availableDevices(QAudio::AudioOutput))
//        qDebug() << "Audio:" << "Available sound output device: " << deviceInfo.deviceName();

    audioOutput = new QAudioOutput(*info, audioFormat, this);
    audioOutput->setBufferSize(audioOutput->bufferSize()*2);
    connect(audioOutput, &QAudioOutput::stateChanged, this, &CAudioThread::handleStateChanged);

    audioIODevice.start();

    // Disable audio at startup. It will be started through "checkAudioBufferTimeout" method when needed
    //audioOutput->start(&audioIODevice);
}

void CAudioThread::run()
{
    // QAudioOutput needs to create within run()
    init(cardRate);
    // start event loop of QThread
    exec();
}

void CAudioThread::stop(void)
{
    audioIODevice.stop();
    audioOutput->stop();
}

void CAudioThread::reset(void)
{
    audioIODevice.flush();

    // For some reason audio under Windows stops working if reset() is used
    //audioOutput->reset();
}

void CAudioThread::handleStateChanged(QAudio::State newState)
{
    currentState = newState;

    switch (newState) {
    case QAudio::ActiveState:
        qDebug() << "Odtwarzacz audio:"
                 << "W stanie aktywnym";
        break;
    case QAudio::SuspendedState:
        qDebug() << "Audio:"
                 << "W stanie zawieszenia";
        break;
    case QAudio::StoppedState:
        qDebug() << "Odtwarzacz audio:"
                 << "W stanie zatrzymania";
        break;
    case QAudio::IdleState:
        qDebug() << "Odtwarzacz audio:"
                 << "W stanie oczekiwania";
        audioOutput->stop();
        break;
    default:
        qDebug() << "Odtwarzacz audio:"
                 << "W nieznanym stanie:" << newState;
        break;
    }
}

void CAudioThread::checkAudioBufferTimeout()
{
    int32_t Bytes = buffer.GetRingBufferReadAvailable();

    // Start audio if bytes are available and audio is not active
    if (audioOutput && Bytes && currentState != QAudio::ActiveState) {
        audioIODevice.start();
        audioOutput->start(&audioIODevice);
    }
}

CAudioIODevice::CAudioIODevice(RingBuffer<int16_t>& buffer, QObject* parent) :
    QIODevice(parent),
    buffer(buffer)
{
}

void CAudioIODevice::start()
{
    open(QIODevice::ReadOnly);
}

void CAudioIODevice::stop()
{
    buffer.FlushRingBuffer();
    close();
}

void CAudioIODevice::flush()
{
    buffer.FlushRingBuffer();
}

qint64 CAudioIODevice::readData(char* data, qint64 len)
{
    qint64 total = 0;

    total = buffer.getDataFromBuffer(data, len / 2); // we have int16 samples

    // If the buffer is empty return zeros.
    if (total == 0) {
        memset(data, 0, len);
        total = len / 2;
    }

    return total * 2;
}

qint64 CAudioIODevice::writeData(const char* data, qint64 len)
{
    Q_UNUSED(data);
    Q_UNUSED(len);

    return 0;
}

qint64 CAudioIODevice::bytesAvailable() const
{
    return buffer.GetRingBufferReadAvailable();
}

CAudio::CAudio(RingBuffer<int16_t>& buffer, QObject *parent) :
    QObject(parent),
    audioThread(nullptr),
    buffer(buffer),
    audioIODevice(buffer, this)
{
    audioThread = std::make_unique<CAudioThread>(buffer);
    audioThread->start();
}

CAudio::~CAudio(void)
{
    if (audioThread != nullptr) {
        audioThread->quit();
        audioThread->wait();
    }
}

void CAudio::stop(void)
{
    // Call stopInternal of CAudioThread (and invoke it in the other thread)
    QMetaObject::invokeMethod(audioThread.get(), "stop", Qt::QueuedConnection);
}

void CAudio::reset(void)
{
    // Call resetInternal of CAudioThread (and invoke it in the other thread)
    QMetaObject::invokeMethod(audioThread.get(), "reset", Qt::QueuedConnection);
}

void CAudio::setRate(int sampleRate)
{
    // Call setRateInternal of CAudioThread (and invoke it in the other thread)
    QMetaObject::invokeMethod(audioThread.get(), "setRate", Qt::QueuedConnection, Q_ARG(int, sampleRate));
}

void CAudio::setVolume(qreal volume)
{
    // Call setVolumeInternal of CAudioThread (and invoke it in the other thread)
    QMetaObject::invokeMethod(audioThread.get(), "setVolume", Qt::QueuedConnection, Q_ARG(qreal, volume));
}


