/*
 *    Copyright (C) 2018
 *    Albrecht Lohofener (albrechtloh@gmx.de)
 *
 */

#ifndef CDEBUGOUTPUT_H
#define CDEBUGOUTPUT_H

#include <QString>
#include "gui_helper.h"

class CLogStringStream;

class CDebugOutput
{
public:
    static void init(void);
    static void customMessageHandler(QtMsgType type, const QMessageLogContext &, const QString & str);
    static void clogMessageHandler(std::string & str);
    static void handleMessage(QString str);
    static void setFileName(QString fileName);
    static void setCGUI(CGUIHelper *cGuiObject);

private:
    static QString fileName;
    static CGUIHelper *cGuiObject;
};

class CLogStringStream : public std::basic_streambuf<char, std::char_traits<char> > {
protected:
    int sync();
    int overflow(int c);

private:
    std::string buffer_;
};

#endif // CDEBUGOUTPUT_H
