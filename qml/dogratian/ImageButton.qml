import QtQuick 2.1
import QtQuick.Window 2.1
import QtGraphicalEffects 1.0
Item {
    id: container

    property alias text: buttonLabel.text
    property alias label: buttonLabel
    property alias icon: image.source
    signal clicked
    property alias containsMouse: mouseArea.containsMouse
    property alias pressed: mouseArea.pressed
    property int iconSize: 64
    implicitHeight: Math.max(Screen.pixelDensity * 7, buttonLabel.implicitHeight * 1.2)
    implicitWidth: Math.max(Screen.pixelDensity * 11, buttonLabel.implicitWidth * 1.3)
    height: image.height + buttonLabel.height
    width: image.width

    SystemPalette { id: palette }

//    Rectangle {
//        id: frame
//        anchors.fill: parent
//        color: palette.button
//        gradient: Gradient {
//            GradientStop { position: 0.0; color: mouseArea.pressed ? Qt.darker(palette.button, 1.3) : palette.button }
//            GradientStop { position: 1.0; color: Qt.darker(palette.button, 1.3) }
//        }
//        antialiasing: true
//        radius: height / 6
//        border.color: Qt.darker(palette.button, 1.5)
//        border.width: 1
//    }
    Glow {
        anchors.fill: image
        radius: mouseArea.pressed ? 16 : 0
        samples: 16
        color: "white"
        source: image
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        onClicked: container.clicked()
        hoverEnabled: true
        onEntered: {
            if (Qt.platform.os === "windows")
                buttonLabel.visible = true;
        }
        onExited: buttonLabel.visible = false;
    }

    Image {
        id: image
        fillMode: Image.PreserveAspectFit
        anchors.top: buttonLabel.bottom
        width: iconSize
        height: iconSize
    }

    Text {
        id: buttonLabel
        text: container.text
        color: palette.buttonText
        anchors.horizontalCenter: parent.horizontalCenter
        visible: false
        renderType: Qt.platform.os === "windows" ? Text.NativeRendering : Text.QtRendering
    }
}
