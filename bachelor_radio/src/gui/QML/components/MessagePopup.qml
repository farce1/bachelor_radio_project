import QtQuick 2.5
import QtQuick.Controls 2.3
import QtGraphicalEffects 1.0

import "../texts"

Popup {
    property alias text: textView.text
    property alias color: backgroundRect.color
    property int revealedY: 0
    property int hiddenY: -150

    id: popup
    width: textView.width + 65
    height: (textView.paintedHeight + 10) < 50 ? 50 : (textView.paintedHeight + 10)
    closePolicy: Popup.NoAutoClose
    enter:  Transition {
        NumberAnimation { property: "y"; from: hiddenY; to: revealedY;}
    }
    exit: Transition {
        NumberAnimation { property: "y"; from: revealedY; to: hiddenY;}
    }

    background: Rectangle {
        id:backgroundRect
        color: "#8b0000"
    }
    contentItem: Item {
        TextStandart {
            id: textView
            text: ""
            anchors.verticalCenter: parent.verticalCenter
            anchors.right: parent.right
            wrapMode: Text.WordWrap
            color: "white"
            onTextChanged: {
                if(paintedWidth + 65 > mainWindow.width) width = mainWindow.width - 65
            }
        }

        MouseArea {
            id: popupMA
            anchors.fill: parent
            onClicked: popup.close()
        }

        Accessible.role: Accessible.AlertMessage
        Accessible.name: popup.text
        Accessible.onPressAction: popupMA.click(mouse)
    }
}
