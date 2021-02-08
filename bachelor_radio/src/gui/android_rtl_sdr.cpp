/*
 *    Copyright (C) 2019
 *    Albrecht Lohofener (albrechtloh@gmx.de)
 *
 *    Driver for https://github.com/martinmarinov/rtl_tcp_andro-
 */

#include <QAndroidJniEnvironment>
#include <QDesktopServices>

#include "android_rtl_sdr.h"

#define RESULT_OK -1

CAndroid_RTL_SDR::CAndroid_RTL_SDR(RadioControllerInterface &RadioController) : CRTL_TCP_Client(RadioController)
{
}

CAndroid_RTL_SDR::~CAndroid_RTL_SDR()
{
}

std::string CAndroid_RTL_SDR::getDescription()
{
    return "Android rtl-sdr " + message.toStdString();
}

CDeviceID CAndroid_RTL_SDR::getID()
{
    return CDeviceID::ANDROID_RTL_SDR;
}

bool CAndroid_RTL_SDR::restart()
{
    if(isLoaded)
        return CRTL_TCP_Client::restart();

    if(isPending)
        return false;

    std::clog << "CAndroid_RTL_SDR: Start Android rtl-sdr driver " << std::endl;
    isPending = true;

    // Start Android rtl_tcp
    QAndroidJniObject path = QAndroidJniObject::fromString("iqsrc://-a 127.0.0.1 -p 1234 -s 2048000");

    QAndroidJniObject uri = QAndroidJniObject::callStaticObjectMethod(
                "android/net/Uri",
                "parse",
                "(Ljava/lang/String;)Landroid/net/Uri;", path.object<jstring>());

    QAndroidJniObject ACTION_VIEW = QAndroidJniObject::getStaticObjectField<jstring>(
                "android/content/Intent",
                "ACTION_VIEW");

    QAndroidJniObject intent(
                "android/content/Intent",
                "(Ljava/lang/String;)V",
                ACTION_VIEW.object<jstring>());

    QAndroidJniObject result = intent.callObjectMethod(
                "setData",
                "(Landroid/net/Uri;)Landroid/content/Intent;",
                uri.object<jobject>());

    resultReceiver = std::make_unique<ActivityResultReceiver>(this);
    QtAndroid::startActivity(intent, 1, resultReceiver.get());

    // Catch exception
    QAndroidJniEnvironment env;
    if (env->ExceptionCheck()) {
        env->ExceptionClear();
    }

    // Configure rtl_tcp_client
    setServerAddress("127.0.0.1");
    setPort(1234);

    return true;
}

bool CAndroid_RTL_SDR::is_ok()
{
    if(isPending)
        return true;
    else
        return CRTL_TCP_Client::is_ok();
}


void CAndroid_RTL_SDR::setErrorMessage(QString message)
{
    this->message = message;
    radioController.onMessage(message_level_t::Error, message.toStdString());
}

void CAndroid_RTL_SDR::setLoaded(bool isLoaded)
{
    this->isLoaded = isLoaded;

    // Try to connect to rtl-tcp
    if(isLoaded)
    {
        isPending = false;
        CRTL_TCP_Client::restart();
    }
}

void CAndroid_RTL_SDR::setOpenInstallDialog()
{
    // Create pop up
    QAndroidJniObject dialog("InstallRtlTcpAndro");
    QAndroidJniObject activity = QtAndroid::androidActivity();
    QAndroidJniObject fm = activity.callObjectMethod("getFragmentManager",
                                                     "()Landroid/app/FragmentManager;");

    // Init pop up
    QAndroidJniObject okButtonText = QAndroidJniObject::fromString(QT_TR_NOOP("OK"));
    QAndroidJniObject cancelButtonText = QAndroidJniObject::fromString(QT_TR_NOOP("Cancel"));
    QAndroidJniObject message = QAndroidJniObject::fromString(QT_TR_NOOP("Android RTL-SDR driver is not installed. Do you would like to install it?"));
    dialog.callMethod<void>("setMessageText",
                            "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V",
                            okButtonText.object<jstring>(), cancelButtonText.object<jstring>(), message.object<jstring>());

    // Show pop up
    QAndroidJniObject string = QAndroidJniObject::fromString("Message");
    dialog.callMethod<void>("show",
                            "(Landroid/app/FragmentManager;Ljava/lang/String;)V",
                            fm.object(), string.object<jstring>());
}


void ActivityResultReceiver::handleActivityResult(int receiverRequestCode, int resultCode, const QAndroidJniObject &data)
{
    if(receiverRequestCode == 1)
    {
        if(resultCode == RESULT_OK)
        {
            std::clog << "CAndroid RTL_SDR: Successfully opened" << std::endl;
            Android_RTL_SDR->setLoaded(true);
        }
        else
        {
            QAndroidJniObject MessageType = QAndroidJniObject::fromString("detailed_exception_message");
            QString Message;

            if(data.isValid())
            {
                QAndroidJniObject result = data.callObjectMethod(
                            "getStringExtra",
                            "(Ljava/lang/String;)Ljava/lang/String;",
                            MessageType.object<jstring>());

                Message = result.toString();
            }
            else
            {
                // We assume here that the Android RTL-SDR driver is not installed
                Message = QT_TR_NOOP("Android RTL-SDR driver is not installed");

                Android_RTL_SDR->setOpenInstallDialog();
            }

            std::clog << "Android RTL_SDR: " << Message.toStdString() << std::endl;
            Android_RTL_SDR->setErrorMessage(Message);

            Android_RTL_SDR->setLoaded(false);
        }
    }
}
