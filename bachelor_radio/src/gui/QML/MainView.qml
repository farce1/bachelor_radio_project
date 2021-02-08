

/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/
import QtQml 2.10
import QtQuick 2.9
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.3
import QtQuick.Controls.Material 2.1
import QtQuick.Controls.Universal 2.1
import QtQuick.Window 2.2
import Qt.labs.settings 1.0

import "texts"
import "settingpages"
import "components"

ApplicationWindow {
    id: mainWindow

    property bool isExpertView: false
    property bool isFullScreen: false
    property bool isLoaded: false
    property bool isStationNameInWindowTitle: false

    StationListModel {
        id: stationList
    }
    StationListModel {
        id: favoritsList
    }

    readonly property bool inPortrait: mainWindow.width < mainWindow.height

    function getWidth() {
            return 700
    }

    function getHeight() {
            return 500
    }

    width: getWidth()
    height: getHeight()

    title: "Projekt dekodera DAB+"
    visible: true

    visibility: isFullScreen ? Window.FullScreen : Window.Windowed

    Component.onCompleted: {
        isLoaded = true
    }

    Settings {
        property alias width: mainWindow.width
        property alias height: mainWindow.height
        property alias stationListSerialize: stationList.serialized
    }
    menuBar: MenuBar {
        id: stationMenu

        Menu {
            title: "Wybór Stacji"

            Action {
                id: startStationScanItem
                text: qsTr("Skanuj stacje")
                onTriggered: {
                    startStationScanItem.enabled = false
                    stopStationScanItem.enabled = true
                    radioController.startScan()
                }
            }

            Action {
                id: stopStationScanItem
                enabled: false
                text: qsTr("Przerwij skanowanie")
                onTriggered: {
                    startStationScanItem.enabled = true
                    stopStationScanItem.enabled = false
                    radioController.stopScan()
                }
            }

            Action {
                text: qsTr("Wyczyść listę")
                onTriggered: stationList.clearStations()
            }

            Action {
                id: stationSettingsItem
                text: qsTr("Ustawienia stacji")
                onTriggered: {
                    stationSettingsDialog.title = "Ustawienia stacji"
                    stationSettingsDialog.open()
                }
            }
        }
        Menu {
            title: "Opcje"
            id: optionsMenu

            Action {
                text: qsTr("Ustawienia")
                onTriggered: {
                    globalSettingsDialog.title = "Settings"
                    globalSettingsDialog.open()
                }
            }
            Action {
                text: qsTr("Zaaw. Ustawienia")
                onTriggered: {
                    expertSettingsDialog.title = "Expert Settings"
                    expertSettingsDialog.open()
                }
            }
            Action {
                text: qsTr("O aplikacji")
                onTriggered: {
                    aboutDialog.title = "About"
                    aboutDialog.open()
                }
            }
            Action {
                text: qsTr("Zakończ")
                onTriggered: guiHelper.close()
            }
        }
    }


    GridLayout {
        id: grid
        anchors.fill: parent
        rows: 12
        columns: 12
        property double colMulti: grid.width / grid.columns
        property double rowMulti: grid.height / grid.rows
        function prefWidth(item) {
            return colMulti * item.Layout.columnSpan
        }
        function prefHeight(item) {
            return rowMulti * item.Layout.rowSpan
        }

        Rectangle {             Layout.rowSpan: 10
            Layout.columnSpan: 2
            Layout.preferredWidth: grid.prefWidth(this)
            Layout.preferredHeight: grid.prefHeight(this)

    ColumnLayout {
        anchors.fill: parent
        anchors.topMargin: 5

        TextStandart {
            text: "Brak stacji na liście"
            visible: stationChannelView.count ? false : true
            Layout.margins: 10
        }

        ListView {
            id: stationChannelView
            model: stationList
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            delegate: StationDelegate {
                stationNameText: stationName
                stationSIdValue: stationSId
                channelNameText: channelName == "File" ? qsTr("File") : channelName
                isFavorit: favorit
                isExpert: isExpertView
                onClicked: radioController.play(channelName, stationName,
                                                stationSId)
                onFavoritClicked: {
                    var favoritInvert = !favorit
                    stationList.setFavorit(stationSId, channelName,
                                           favoritInvert) // Invert favorit

                    if (favoritInvert)
                        favoritsList.addStation(stationName, stationSId,
                                                channelName, true)
                    else
                        favoritsList.removeStation(stationSId, channelName)
                }
            }

            ScrollIndicator.vertical: ScrollIndicator {}
        }

        RowLayout {
            Layout.margins: 10
            visible: isExpertView ? true : false

            Accessible.role: Accessible.ComboBox
            Accessible.name: manualChannelText.text + " " + manualChannelBox.currentText

            TextStandart {
                id: manualChannelText
                text: qsTr("Manual channel")
                Layout.fillWidth: true

                Accessible.ignored: true
            }

            WComboBox {
                id: manualChannelBox
                sizeToContents: true
                model: ["5A", "5B", "5C", "5D", "6A", "6B", "6C", "6D", "7A", "7B", "7C", "7D", "8A", "8B", "8C", "8D", "9A", "9B", "9C", "9D", "10A", "10B", "10C", "10D", "11A", "11B", "11C", "11D", "12A", "12B", "12C", "12D", "13A", "13B", "13C", "13D", "13E", "13F", "LA", "LB", "LC", "LD", "LE", "LF", "LG", "LH", "LI", "LJ", "LK", "LL", "LM", "LN", "LO", "LP"]

                onActivated: {
                    radioController.setManualChannel(model[index])
                }
            }
        }
    }
}
    Rectangle {
        color: '#153243'
        Layout.rowSpan: 10
        Layout.columnSpan: 10
        Layout.preferredWidth: grid.prefWidth(this)
        Layout.preferredHeight: grid.prefHeight(this)
    GeneralView {
        id: generalView
        isExpert: isExpertView
        isPortrait: inPortrait
    }

    }
    Rectangle {
        id: greenRect
        color: '#B4B8AB'
        Layout.rowSpan: 2
        Layout.columnSpan: 12
        Layout.preferredWidth: grid.prefWidth(this)
        Layout.preferredHeight: grid.prefHeight(this)
        TextStandart {
            text: "GUI Projekt dekodera DAB+. Znalezione stacje: " + stationChannelView.count
            anchors.centerIn: parent
        }
    }
    }

    WDialog {
        id: aboutDialog

        contentItem: InfoPage {
            id: infoPage
        }
    }

    WDialog {
        id: stationSettingsDialog
        content: Loader {
            id: stationSettingsLoader
            anchors.right: parent.right
            anchors.left: parent.left
            height: item.implicitHeight
            source: "qrc:/QML/settingpages/ChannelSettings.qml"
            onLoaded: isStationNameInWindowTitle
                      = stationSettingsLoader.item.addStationNameToWindowTitleState
        }
        Connections {
            target: stationSettingsLoader.item
            onAddStationNameToWindowTitleStateChanged: isStationNameInWindowTitle = stationSettingsLoader.item.addStationNameToWindowTitleState
        }
    }

    WDialog {
        id: globalSettingsDialog

        content: Loader {
            id: globalSettingsLoader
            anchors.right: parent.right
            anchors.left: parent.left
            height: item.implicitHeight
            source: "qrc:/QML/settingpages/GlobalSettings.qml"
            onLoaded: isFullScreen = false
        }
    }

    WDialog {
        id: expertSettingsDialog

        content: Loader {
            id: expertSettingsLoader
            anchors.right: parent.right
            anchors.left: parent.left
            height: item.implicitHeight
            source: "qrc:/QML/settingpages/ExpertSettings.qml"
            onLoaded: isExpertView = expertSettingsLoader.item.enableExpertModeState
        }

        Connections {
            target: expertSettingsLoader.item
            onEnableExpertModeStateChanged: isExpertView
                                            = expertSettingsLoader.item.enableExpertModeState
        }
    }

    MessagePopup {
        id: errorMessagePopup
        x: mainWindow.width / 2 - width / 2
        y: mainWindow.height - stationMenu.height - height
        revealedY: mainWindow.height - stationMenu.height - height
        hiddenY: mainWindow.height
        color: "#8b0000"
    }

    MessagePopup {
        id: infoMessagePopup
        x: mainWindow.width / 2 - width / 2
        y: mainWindow.height - stationMenu.height - height
        revealedY: mainWindow.height - stationMenu.height - height
        hiddenY: mainWindow.height
        color: "#468bb7"
        onOpened: closeTimer.running = true
        Timer {
            id: closeTimer
            interval: 1 * 5000 // 5 s
            repeat: false
            onTriggered: {
                infoMessagePopup.close()
            }
        }
    }

    Connections {
        target: radioController

        onShowErrorMessage: {
            errorMessagePopup.text = Text

            if (mainWindow.isLoaded)
                errorMessagePopup.open()
        }

        onShowInfoMessage: {
            infoMessagePopup.text = Text
            infoMessagePopup.open()
        }

        onScanStopped: {
            startStationScanItem.enabled = true
            stopStationScanItem.enabled = false
        }

        onScanProgress: {
            startStationScanItem.enabled = false
            stopStationScanItem.enabled = true
        }

        onNewStationNameReceived: stationList.addStation(station, sId,
                                                         channel, false)
    }

    Connections {
        target: guiHelper

        onMinimizeWindow: hide()
        onMaximizeWindow: showMaximized()
        onRestoreWindow: {
            // On Linux (KDE?): Hide before we restore
            // otherwise the window will occasionaly not be brought to the front
            if (Qt.platform.os === "linux" && !active)
                // Linux Workaround to display the window
                hide()
            showNormal()
            raise() // Stay in foreground
            if (Qt.platform.os === "linux" && !active)
                // Linux Workaround to display the window
                requestActivate()
        }
    }

    onVisibilityChanged: {
        if (visibility === Window.Minimized)
            guiHelper.tryHideWindow()
    }
}
