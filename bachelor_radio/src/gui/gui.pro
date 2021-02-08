include(../backend.pri)

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

TEMPLATE = app
QT += qml quick charts multimedia core gui quickcontrols2

RESOURCES +=    resources.qrc
DISTFILES +=    \
    QML/MainView.qml \
    QML/RadioView.qml \
    QML/ExpertView.qml \
    QML/texts/TextStyle.qml \
    QML/texts/TextTitle.qml \
    QML/texts/TextStandart.qml \
    QML/texts/TextStation.qml \
    QML/texts/TextRadioInfo.qml \
    QML/texts/TextRadioStation.qml \
    QML/texts/TextExpert.qml \
    QML/InfoPage.qml \
    QML/settingpages/ExpertSettings.qml \
    QML/settingpages/GlobalSettings.qml \
    QML/settingpages/ChannelSettings.qml \
    QML/settingpages/RTLSDRSettings.qml \
    QML/settingpages/RTLTCPSettings.qml \
    QML/settingpages/SoapySDRSettings.qml \
    QML/settingpages/ExpertSettings.qml \
    QML/settingpages/NullSettings.qml \
    QML/components/MessagePopup.qml \
    QML/components/SettingSection.qml \
    QML/components/StationDelegate.qml \
    QML/components/Units.qml \
    QML/expertviews/ImpulseResponseGraph.qml \
    QML/expertviews/ConstellationGraph.qml \
    QML/expertviews/NullSymbolGraph.qml \
    QML/expertviews/SpectrumGraph.qml \
    QML/expertviews/TextOutputView.qml \
    QML/expertviews/RawRecorder.qml \
    QML/components/StationListModel.qml \
    QML/MotView.qml \
    QML/components/ViewBaseFrame.qml \
    QML/GeneralView.qml \
    QML/settingpages/RawFileSettings.qml \
    QML/components/WComboBox.qml \
    QML/components/WComboBoxList.qml \
    QML/components/WButton.qml \
    QML/components/WSwitch.qml \
    QML/components/WTumbler.qml \
    QML/components/WSpectrum.qml \
    QML/components/WMenu.qml \
    QML/expertviews/ServiceDetails.qml \
    QML/components/WDialog.qml

TRANSLATIONS = i18n/pl_PL.ts \

lupdate_only{ # Workaround for lupdate to scan QML files
SOURCES += QML/*.qml \
    QML/texts/*.qml \
    QML/settingpages/*.qml \
    QML/components/*.qml \
    QML/expertviews/*.qml \
}

HEADERS += \
    audio_output.h \
    debug_output.h \
    gui_helper.h \
    mot_image_provider.h \
    radio_controller.h \
    waterfallitem.h \
    version.h

SOURCES += \
    main.cpp \
    audio_output.cpp \
    debug_output.cpp \
    gui_helper.cpp \
    mot_image_provider.cpp \
    radio_controller.cpp \
    waterfallitem.cpp

