import QtQuick 2.6
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.1
import Qt.labs.settings 1.0

// Import custom styles
import "../texts"
import "../components"

Item {
    id: settingsPage

    property bool isLoaded: false
    property var deviceMap: {
        "0": "None",
        "1": "Airspy",
        "2": "rtl-sdr",
        "3": "SoapySDR",
        "4": "rtl-tcp",
        "5": "RAW file"
    }

    anchors.fill: parent
    implicitHeight: layout.implicitHeight

    Settings {
        property alias device: deviceBox.currentIndex
        property alias enableAGCState: enableAGC.checked
        property alias manualGainState: manualGain.value
        property alias manualGainValue: valueSliderView.text
        property alias enableAutoSdr: enableAutoSdr.checked
    }

    Component.onCompleted: {
        console.debug("Inicjalizacja opcji")
        console.debug("Ustawienie wzmocnienia: " + manualGain.value)
        console.debug("Autoregulacja wzmocnienia: " + enableAGC.checked)
        radioController.setGain(manualGain.value)
        radioController.setAGC(enableAGC.checked)
    }

    Connections {
        target: radioController

        onGainCountChanged: manualGain.to = radioController.gainCount
    }

    Connections {
        target: guiHelper

        onNewDeviceId: {
            switch (deviceId) {
            case 0:
                deviceBox.currentIndex = 0
                break
            case 1:
                deviceBox.currentIndex = 0
                break
            case 2:
                deviceBox.currentIndex = 1
                break
            case 3:
                deviceBox.currentIndex = 5
                break
            case 4:
                deviceBox.currentIndex = 2
                break
            case 5:
                deviceBox.currentIndex = 4
                break
            case 6:
                deviceBox.currentIndex = 3
                break
            default:
                deviceBox.currentIndex = 0
            }
        }
    }

    ColumnLayout {
        id: layout
        anchors.fill: parent
        spacing: 20

        SettingSection {
            text: qsTr("Global receiver settings")

            WSwitch {
                id: enableAGC
                text: qsTr("Automatic RF gain")
                Layout.fillWidth: true
                checked: true
                onClicked: {
                    radioController.setAGC(checked)

                    if (checked == false)
                        radioController.setGain(manualGain.value)
                }
            }

            ColumnLayout {
                spacing: 10
                opacity: enabled ? 1 : 0.5
                enabled: !enableAGC.checked

                RowLayout {
                    TextStandart {
                        id: nameSliderView
                        Layout.fillWidth: true
                        text: qsTr("Manual gain")
                    }

                    TextStandart {
                        id: labelSliderView
                        Layout.alignment: Qt.AlignRight
                        text: qsTr("Value: ")
                    }

                    TextStandart {
                        id: valueSliderView
                        text: radioController.gainValue.toFixed(2)
                    }
                }

                Slider {
                    id: manualGain
                    from: 0
                    to: 100
                    stepSize: 1
                    Layout.fillWidth: true

                    onValueChanged: {
                        if (enableAGC.checked == false)
                            radioController.setGain(value)
                    }
                }
            }

            WSwitch {
                id: enableAutoSdr
                text: qsTr("Auto detect")
                Layout.fillWidth: true
                checked: true
                onCheckedChanged: {
                    if (enableAutoSdr.checked)
                        guiHelper.openAutoDevice()
                }
            }

            WComboBoxList {
                id: deviceBox
                enabled: !enableAutoSdr.checked
                Layout.fillWidth: true

                textRole: 'trLabel'
                model: ListModel {
                    id: deviceBoxModel
                    ListElement {
                        label: "None"
                        trLabel: qsTr("None")
                        trContext: "GlobalSettings"
                    }
                    ListElement {
                        label: "Airspy"
                        trLabel: qsTr("Airspy")
                        trContext: "GlobalSettings"
                    }
                    ListElement {
                        label: "rtl-sdr"
                        trLabel: qsTr("rtl-sdr")
                        trContext: "GlobalSettings"
                    }
                    ListElement {
                        label: "SoapySDR"
                        trLabel: qsTr("SoapySDR")
                        trContext: "GlobalSettings"
                    }
                    ListElement {
                        label: "rtl-tcp"
                        trLabel: qsTr("rtl-tcp")
                        trContext: "GlobalSettings"
                    }
                    ListElement {
                        label: "RAW file"
                        trLabel: qsTr("RAW file")
                        trContext: "GlobalSettings"
                    }
                }

                onCurrentIndexChanged: {
                    // Load appropriate settings
                    switch (currentIndex) {
                    case 1:
                        sdrSpecificSettings.source = "qrc:/QML/settingpages/AirspySettings.qml"
                        break
                    case 2:
                        sdrSpecificSettings.source = "qrc:/QML/settingpages/RTLSDRSettings.qml"
                        break
                    case 3:
                        sdrSpecificSettings.source = "qrc:/QML/settingpages/SoapySDRSettings.qml"
                        break
                    case 4:
                        sdrSpecificSettings.source = "qrc:/QML/settingpages/RTLTCPSettings.qml"
                        break
                    case 5:
                        sdrSpecificSettings.source = "qrc:/QML/settingpages/RawFileSettings.qml"
                        break
                    default:
                        sdrSpecificSettings.source = "qrc:/QML/settingpages/NullSettings.qml"
                        break
                    }

                    console.debug(
                                "Użyto urządzenia: " + deviceMap[currentIndex])
                }
            }

            Component.onCompleted: {
                if (enableAutoSdr.checked)
                    guiHelper.openAutoDevice()

                sdrSpecificSettings.item.initDevice(enableAutoSdr.checked)
                isLoaded = true
            }
        }

        Loader {
            id: sdrSpecificSettings
            Layout.fillWidth: true

            onLoaded: {
                if (isLoaded)
                    item.initDevice(enableAutoSdr.checked)
            }
        }
    }
}
