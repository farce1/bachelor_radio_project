/*
 *    Copyright (C) 2018
 *    Albrecht Lohofener (albrechtloh@gmx.de)
 *
 *
 */

#include "debug_output.h"

#include <iostream>
#include <QFile>
#include <QTextStream>
#include <QDateTime>

#ifdef __ANDROID__
    #include <android/log.h>
#endif

QString CDebugOutput::fileName = "";
CGUIHelper *CDebugOutput::cGuiObject = nullptr;

void CDebugOutput::init(void)
{
    // Redirect clog()
    std::clog.rdbuf(new CLogStringStream);

    // Redirect qDebug()
    qInstallMessageHandler(CDebugOutput::customMessageHandler);
}

void CDebugOutput::customMessageHandler(QtMsgType type, const QMessageLogContext &, const QString & str)
{
    QString txt;

    switch (type)
    {
    case QtInfoMsg: txt = QString("Info: %1").arg(str); break;
    case QtDebugMsg: txt = QString("Debug: %1").arg(str); break;
    case QtWarningMsg: txt = QString("Warning: %1").arg(str); break;
    case QtCriticalMsg: txt = QString("Critical: %1").arg(str); break;
    case QtFatalMsg: txt = QString("Fatal: %1").arg(str); break;
    }

    handleMessage(txt + "\n");
}

void CDebugOutput::clogMessageHandler(std::string &str)
{
    handleMessage("Info: " + QString::fromStdString(str));
}

void CDebugOutput::handleMessage(QString str)
{
    QString DateTime = QDateTime::currentDateTime().toString(Qt::ISODateWithMs);
    QString message = DateTime + " " + str;

    // Write only of file name is set
    if(fileName != "")
    {
        QFile outFile(fileName);
        outFile.open(QIODevice::WriteOnly | QIODevice::Append);
        QTextStream ts(&outFile);
        ts << message;
    }

    if(cGuiObject != nullptr)
    {
        cGuiObject->setNewDebugOutput(message);
    }

    // Console message output
#ifdef __ANDROID__
    std::cerr << message.toStdString();
#endif
}

void CDebugOutput::setFileName(QString FileName)
{
    CDebugOutput::fileName = FileName;
}

void CDebugOutput::setCGUI(CGUIHelper *CGuiObject)
{
    CDebugOutput::cGuiObject = CGuiObject;
}

int CLogStringStream::sync() {
    if (buffer_.length()) {
        CDebugOutput::clogMessageHandler(buffer_);
        buffer_.erase();
    }
    return 0;
}

int CLogStringStream::overflow(int c) {
    if (c != EOF) {
        buffer_ += static_cast<char>(c);
    } else {
        sync();
    }
    return c;
}
