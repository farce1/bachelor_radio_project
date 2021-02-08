/*
 *    Copyright (C) 2017
 *    Albrecht Lohofener (albrechtloh@gmx.de)
 *
 *    This file is based on SDR-J
 *    Copyright (C) 2010, 2011, 2012, 2013
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *
 */

#include <unistd.h>

#include <QApplication>
#include <QQuickStyle>
#include <QCoreApplication>
#include <QCommandLineParser>
#include <QDebug>
#include <QDir>
#include <QSettings>
#include <QIcon>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "version.h"
#include "dab-constants.h"
#include "radio_controller.h"
#include "gui_helper.h"
#include "debug_output.h"

int main(int argc, char** argv)
{
    QCoreApplication::setOrganizationName("Receiver_Project");
    QCoreApplication::setOrganizationDomain("Receiver_Project");
    QCoreApplication::setApplicationName("Receiver_Project");
    QCoreApplication::setApplicationVersion("1.0.0");

    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    // Handle debug output
    CDebugOutput::init();

    // Before printing anything, we set
    setlocale(LC_ALL, "");

    // Create new QT application
    QApplication app(argc, argv);

    //Initialise translation
    QTranslator *translator = new QTranslator;
    QString lang = CGUIHelper::mapToLanguage("pl_PL");
    CGUIHelper::loadTranslationFile(translator, lang);
    QCoreApplication::installTranslator(translator);

    // Register custiom types
    qmlRegisterType<WaterfallItem>("gui", 1, 0, "Waterfall");
    qRegisterMetaType<mot_file_t>("mot_file_t");

    // Set icon path
    QStringList themePaths;
    themePaths << ":/icons";
    QIcon::setThemeSearchPaths(themePaths);
    QIcon::setThemeName("custom_icons");

    // Handle the command line
    QCommandLineParser optionParser;
    optionParser.addHelpOption();
    optionParser.addVersionOption();

    //	Process the actual command line arguments given by the user
    optionParser.process(app);

    QVariantMap commandLineOptions;

    CRadioController radioController(commandLineOptions);

    QSettings settings;

    // Should we play the last station we have listened to previously?
    bool isAutoPlay = settings.value("enableLastPlayedStationState", false).toBool();
    QStringList lastStation = settings.value("lastchannel").toStringList();
    if( lastStation.count() == 2 )
        radioController.setAutoPlay(isAutoPlay, lastStation[1], lastStation[0]);

    // Load mandatory driver arguments to init input device
    radioController.setDeviceParam("SoapySDRDriverArgs", settings.value("soapyDriverArgs","").toString());
    radioController.setDeviceParam("SoapySDRAntenna", settings.value("soapyDriverAntenna","").toString());
    radioController.setDeviceParam("SoapySDRClockSource", settings.value("soapyDriverClockSource","").toString());

    CGUIHelper guiHelper(&radioController);
    guiHelper.setTranslator(translator);

    // Create new QML application, set some requried options and load the QML file
    QQmlApplicationEngine engine;
    QQmlContext* rootContext = engine.rootContext();

    // Connect C++ code to QML GUI
    rootContext->setContextProperty("guiHelper", &guiHelper);
    rootContext->setContextProperty("radioController", &radioController);

    // Load main page
    engine.load(QUrl("qrc:/QML/MainView.qml"));

    // Add MOT slideshow provider
    engine.addImageProvider(QLatin1String("SLS"), guiHelper.motImageProvider);

    // Run application
    app.exec();

    // Disconnect all signals and slots
    guiHelper.disconnect();
    radioController.disconnect();

    qDebug() << "main:" <<  "Application closed";

    return 0;
}
