import QtQuick 2.2
import QtQuick.Layouts 1.1
import QtQuick.Controls 2.3
import QtGraphicalEffects 1.0

// Import custom styles
import "texts"
import "components"

ViewBaseFrame {
    id: frame
    labelText: "Informacje o stacji"
    Layout.maximumHeight: 230

    TextRadioInfo {
        anchors.top: parent.top
        anchors.topMargin: 5
        anchors.horizontalCenter: parent.horizontalCenter
        text: radioController.ensemble.trim()
    }

    // Use 2 Images to switch between speaker & speaker_mute icon (instead of toggle button). 
    // Permits use of color with org.kde.desktop style
    Image {
        id: speakerIcon
        anchors.verticalCenter: signalStrength.verticalCenter
        anchors.right: parent.right
        width: 30
        height: 30
        visible: true
        source: "qrc:/icons/speaker.png"

        MouseArea {
            anchors.fill: parent
            onClicked: {radioController.setVolume(0); speakerIconMutedRed.visible = true; speakerIcon.visible = false}
        }
    }

    Image {
        id: speakerIconMuted
        anchors.verticalCenter: signalStrength.verticalCenter
        anchors.right: parent.right
        width: 30
        height: 30
        visible: false

        source: "qrc:/icons/speaker_muted.png"
    }

    ColorOverlay {
        id: speakerIconMutedRed
        visible: false
        anchors.fill: speakerIconMuted
        source: speakerIconMuted
        color: "red"
        MouseArea {
            anchors.fill: parent
            onClicked: {radioController.setVolume(100); speakerIconMutedRed.visible = false; speakerIcon.visible = true}
        }
    }

    RowLayout{
        id: signalStrength
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.leftMargin: 5 * 2
        spacing: 2 * 2

        Accessible.name: qsTr("Signal noise ratio: " + radioController.snr )

        Rectangle{
            height: 4 * 2
            width: 4 * 2
            color: (radioController.snr > 2) ? "green" : "dimgrey"
            Accessible.ignored: true
        }
        Rectangle{
            height: 8 * 2
            width: 4 * 2
            color: (radioController.snr > 5) ? "green" : "dimgrey"
            Accessible.ignored: true
        }
        Rectangle{
            height: 12 * 2
            width: 4 * 2
            color: (radioController.snr > 8) ? "green" : "dimgrey"
            Accessible.ignored: true
        }
        Rectangle{
            height: 16 * 2
            width: 4 * 2
            color: (radioController.snr > 11) ? "green" : "dimgrey"
            Accessible.ignored: true
        }

        Rectangle{
            height: 20 * 2
            width: 4 * 2
            color: (radioController.snr > 15) ? "green" : "dimgrey"
            Accessible.ignored: true
        }
    }

    ColumnLayout {
        anchors.centerIn: parent

        /* Station Name */
        RowLayout {
            Layout.alignment: Qt.AlignHCenter

            TextRadioStation {
                id:textRadioStation
                Layout.margins: 10
                Layout.maximumWidth: parent.parent.parent.width
                wrapMode: Text.Wrap
                horizontalAlignment: Text.AlignHCenter
                text: radioController.title.trim()

                // Use an Item to display icons as Image
                Item {
                    property bool isSignal: false
                    id: antennaSymbol
                    
                    anchors.leftMargin: 10
                    implicitWidth: antennaIcon.width
                    implicitHeight: implicitWidth
                    
                    property bool isTextTooLongForAntenna: {
                        var maxWidth = parent.parent.parent.parent.width;
                        var textAndAntennaWidth = parent.width + implicitWidth + 30 + anchors.leftMargin;
                        return ( textAndAntennaWidth > maxWidth )
                    }
                    
                    visible: opacity == 0 ? false : true
                    opacity: 100
                    
                    Connections {
                        target: frame
                        onWidthChanged: { reanchorAntenna() }
                    }
                    
                    Connections {
                        target: textRadioStation
                        onTextChanged: { reanchorAntenna() }
                    }
                    
                    states: [
                    State {
                        name: "alignRight"
                        AnchorChanges {
                            target: antennaSymbol
                            anchors.left: textRadioStation.right
                            anchors.verticalCenter: textRadioStation.verticalCenter
                            anchors.top: undefined
                            anchors.horizontalCenter: undefined
                        }
                    },
                    State {
                        name: "alignBottom"
                        AnchorChanges {
                            target: antennaSymbol
                            anchors.left: undefined
                            anchors.verticalCenter: undefined
                            anchors.top: textRadioStation.bottom
                            anchors.horizontalCenter: textRadioStation.horizontalCenter
                        }
                    }
                    ]
                    
                    Image {
                        id: antennaIcon
                        width: 30
                        height: 30
                        visible: false
                        source: "qrc:/icons/antenna.png"
                    }
                    
                    Image {
                        id: antennaIconNoSignal
                        width: antennaIcon.width
                        height: antennaIcon.height
                        visible: false
                        source: "qrc:/icons/antenna_nosignal.png"
                    }
                    
                    Connections {
                        target: antennaSymbol
                        onIsSignalChanged: { 
                            if (antennaSymbol.isSignal) {
                                antennaIconNoSignal.visible = false;
                                antennaIcon.visible = true;
                            } else {
                                antennaIconNoSignal.visible = true;
                                antennaIcon.visible = false;
                            }
                        }
                    }
                    
                    NumberAnimation on opacity {
                        id: effect
                        to: 0;
                        duration: 6000;
                        running: false
                    }

                    Connections {
                        target: radioController
                        onIsFICCRCChanged: {
                            if(radioController.isFICCRC)
                                __setIsSignal(true)
                            else
                                __setIsSignal(false)
                        }

                        onIsSyncChanged: {
                            if(radioController.isSync)
                                __setIsSignal(true)
                            else
                                __setIsSignal(false)
                        }
                    }
                }
            }
        }

        /* Station Text */
        TextRadioInfo {
            Layout.alignment: Qt.AlignHCenter
            Layout.margins: 10
            Layout.maximumWidth: parent.parent.width
            width: frame.width
            wrapMode: Text.WordWrap
            horizontalAlignment: Text.AlignHCenter
            text: radioController.text.trim()
        }
    }

    RowLayout{
        id: stationInfo
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 5
        width: parent.width

        TextRadioInfo {
            id: stationType
            visible: stationInfo.visible
            Layout.alignment: Qt.AlignLeft
            Layout.leftMargin: 5
            verticalAlignment: Text.AlignBottom
            text: qsTranslate("DABConstants", radioController.stationType)
        }

        TextRadioInfo {
            visible: stationInfo.visible
            Layout.alignment: Qt.AlignRight
            Layout.rightMargin: 5
            verticalAlignment: Text.AlignBottom
            Layout.maximumWidth: parent.parent.width - stationType.width
            fontSizeMode: Text.Fit
            minimumPixelSize: 8;
            text: (radioController.isDAB ? "Transmisja DAB" : "Transmisja DAB+")
                + " " + radioController.audioMode
        }
    }

    Component.onCompleted: {
        if(radioController.isFICCRC &&
                radioController.isSync)
            __setIsSignal(true)
    }

    function __setIsSignal(value) {
        if(value) {
            antennaSymbol.isSignal = true
            effect.restart()
        }
        else {
            antennaSymbol.isSignal = false
            antennaSymbol.opacity = 100
            effect.stop()
        }
    }

    function reanchorAntenna() {
        if (!antennaSymbol.isTextTooLongForAntenna)
            antennaSymbol.state = "alignRight"
        else
            antennaSymbol.state = "alignBottom"
    }
}
