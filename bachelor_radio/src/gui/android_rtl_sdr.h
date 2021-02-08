/*
 *    Copyright (C) 2019
 *    Albrecht Lohofener (albrechtloh@gmx.de)
 *    Driver for https://github.com/martinmarinov/rtl_tcp_andro-
 */

#ifndef CANDROID_RTL_SDR_H
#define CANDROID_RTL_SDR_H

#include <QtAndroidExtras/QAndroidJniObject>
#include <QtAndroidExtras/QtAndroid>
#include <QtAndroidExtras/QAndroidActivityResultReceiver>

#include "virtual_input.h"
#include "rtl_tcp.h"

class ActivityResultReceiver;

class CAndroid_RTL_SDR : public CRTL_TCP_Client
{
public:
    CAndroid_RTL_SDR(RadioControllerInterface &RadioController);
    ~CAndroid_RTL_SDR();

    // Override
    std::string getDescription(void);
    CDeviceID getID(void);
    bool restart(void);
    bool is_ok(void);

    void setErrorMessage(QString message);
    void setLoaded(bool isLoaded);
    void setOpenInstallDialog(void);

private:
    std::unique_ptr<ActivityResultReceiver> resultReceiver;
    QString message;
    bool isLoaded = false;
    bool isPending = false;

signals:
    void showAndroidInstallDialog(QString Title, QString Text);
};

class ActivityResultReceiver : public QAndroidActivityResultReceiver
{
public:
    ActivityResultReceiver(CAndroid_RTL_SDR *Client): Android_RTL_SDR(Client){}

    virtual void handleActivityResult(int receiverRequestCode, int resultCode, const QAndroidJniObject &data);

private:
    CAndroid_RTL_SDR *Android_RTL_SDR;
};

#endif // CANDROID_RTL_SDR_H
