import QtQuick 2.1
import QtQuick.Controls 1.1
import QtGraphicalEffects 1.0
import "dogratian"

Rectangle {
    id: applicationTop
    width: 350
    height: 600
    property int buttonSize: 64
    property bool showCelsius: true


    // Functions - Append Message to Log
    function appendToLog (aMsg)
    {
        var now = new Date();
        listLog.append ({"text":aMsg, "timestamp":now});
        tableLog.positionViewAtRow (listLog.count -1, ListView.Contain)
    }

    // Objects
    ListModel {
        id: listLog
    }


    // User Interface Area
    Rectangle {
        id: rectInterface
        anchors.fill: parent;
        property double buttonHeight: rectInterface.height * 0.1

        Image {
            anchors.fill: parent
            fillMode: Image.PreserveAspectCrop
            source: "qrc:///asset/background.jpg"
        }

        Grid {
            id: gridInfo
            width: parent.width
            columns: 2
            spacing: 2
            property double colWidth1: 0.2

            //
            Text {
                text: qsTr ("Name:")
                width: parent.width * parent.colWidth1
                font.bold: true
                horizontalAlignment: Text.AlignRight
                clip: true
                renderType: Qt.platform.os === "windows" ? Text.NativeRendering : Text.QtRendering
            }
            Text {
                id: textDeviceName
                text: qsTr ("-")
                clip: true
                renderType: Qt.platform.os === "windows" ? Text.NativeRendering : Text.QtRendering
            }

            //
            Text {
                text: qsTr ("VID:")
                width: parent.width * parent.colWidth1
                font.bold: true
                horizontalAlignment: Text.AlignRight
                clip: true
                renderType: Qt.platform.os === "windows" ? Text.NativeRendering : Text.QtRendering
            }
            Text {
                id: textVID
                text: qsTr ("-")
                clip: true
                renderType: Qt.platform.os === "windows" ? Text.NativeRendering : Text.QtRendering
            }

            //
            Text {
                text: qsTr ("PID:")
                width: parent.width * parent.colWidth1
                font.bold: true
                horizontalAlignment: Text.AlignRight
                clip: true
                renderType: Qt.platform.os === "windows" ? Text.NativeRendering : Text.QtRendering
            }
            Text {
                id: textPID
                text: qsTr ("-")
                clip: true
                renderType: Qt.platform.os === "windows" ? Text.NativeRendering : Text.QtRendering
            }
        }

        Column {
            id: colReading
            anchors.top: gridInfo.bottom
            width: parent.width
            spacing: applicationTop.height / 10

            Item {
                width: 1
                height: 1
            }

            Item {
                id: itemTemperature
                width: rowTemperature.width
                height: rowTemperature.height
                anchors.horizontalCenter: parent.horizontalCenter

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        var celsius = showCelsius;
                        var temp;

                        if (celsius)
                            celsius = false;
                        else
                            celsius = true;
                        showCelsius = celsius

                        temp = parseFloat (ctrlTop.temperature);

                        if (isNaN (temp))
                        {
                            textTempreature.text = "-"
                        }
                        else
                        {
                            if (celsius)
                            {
                                textTempreature.text = temp.toFixed (1);
                                textTemperatureUnit.text = " \xB0C";
                            }
                            else
                            {
                                temp = ctrlTop.temperature * 9 / 5 + 32;
                                textTempreature.text = temp.toFixed (1);
                                textTemperatureUnit.text = " \xB0F";
                            }
                        }
                    }
                }

                Row
                {
                    id: rowTemperature

                    Text {
                        id: textTempreature
                        anchors.bottom: parent.bottom
                        text: "-"
                        color: "white"
                        font.pointSize: 72
                        font.bold: true
                        style: Text.Raised
                        clip: true
                        renderType: Qt.platform.os === "windows" ? Text.NativeRendering : Text.QtRendering
                    }
                    Text {
                        id: textTemperatureUnit
                        anchors.bottom: parent.bottom
                        text: " \xB0C"
                        color: "white"
                        font.pointSize: 36
                        font.bold: true
                        style: Text.Raised
                        clip: true
                        renderType: Qt.platform.os === "windows" ? Text.NativeRendering : Text.QtRendering
                    }
                }
            }


            Item {
                id: itemHumidity
                width: rowHumidity.width
                height: rowHumidity.height
                anchors.horizontalCenter: parent.horizontalCenter

                Row
                {
                    id: rowHumidity

                    Text {
                        id: textHumidity
                        anchors.bottom: parent.bottom
                        text: "-"
                        color: "white"
                        font.pointSize: 72
                        font.bold: true
                        style: Text.Raised
                        clip: true
                        renderType: Qt.platform.os === "windows" ? Text.NativeRendering : Text.QtRendering
                    }
                    Text {
                        anchors.bottom: parent.bottom
                        text: " %RH"
                        color: "white"
                        font.pointSize: 36
                        font.bold: true
                        style: Text.Raised
                        clip: true
                        renderType: Qt.platform.os === "windows" ? Text.NativeRendering : Text.QtRendering
                    }
                }
            }

            Item {
                id: itemPressure
                width: rowPressure.width
                height: rowPressure.height
                anchors.horizontalCenter: parent.horizontalCenter
                visible: false

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                    }
                }

                Row {
                    id: rowPressure
                    Text {
                        id: textPressure
                        anchors.bottom: parent.bottom
                        text: "-"
                        color: "white"
                        font.pointSize: 72
                        font.bold: true
                        style: Text.Raised
                        clip: true
                        renderType: Qt.platform.os === "windows" ? Text.NativeRendering : Text.QtRendering
                    }
                    Text {
                        id: textPressureUnit
                        anchors.bottom: parent.bottom
                        text: " hPa"
                        color: "white"
                        font.pointSize: 36
                        font.bold: true
                        style: Text.Raised
                        clip: true
                        renderType: Qt.platform.os === "windows" ? Text.NativeRendering : Text.QtRendering
                    }
                }
            }
        }

        Row {
            id: colButtons
            anchors.bottom: textStatus.top
            anchors.horizontalCenter: parent.horizontalCenter
            spacing: (parent.width - buttonConnect.width * 3) / 3

            ImageButton {
                id: buttonConnect
                text: qsTr ("Connect")
                icon: "qrc:///asset/link_broken.png"
                iconSize: buttonSize
                onClicked: {
                    if (ctrlTop.isOpen)
                    {
                        ctrlTop.close();
                        textStatus.text = "Disconnected.";
                        buttonConnect.icon = "qrc:///asset/link_broken.png";
                        buttonConnect.text = "Connect";
                    }
                    else
                    {
                        if (ctrlTop.check() >= 0)
                        {
                            if (ctrlTop.open() >= 0)
                            {
                                textStatus.text = "Device connected.";
                                buttonConnect.icon = "qrc:///asset/link.png";
                                buttonConnect.text = "Disconnect";
                            }
                        }
                    }
                }
            }

            ImageButton {
                id: buttonLog
                text: qsTr ("Log")
                icon: "qrc:///asset/document.png"
                iconSize: buttonSize
                onClicked: {
                    if (itemLog.visible)
                        itemLog.visible = false;
                    else
                        itemLog.visible = true;
                }
            }

            ImageButton {
                id: buttonExit
                text: qsTr ("Exit")
                icon: "qrc:///asset/cross.png"
                iconSize: buttonSize
                onClicked: {
                    ctrlTop.close();
                    Qt.quit();
                }
            }
        }

        // Status Bar
        Text {
            id: textStatus
            text: qsTr("Copyright (c) 2014 DogRatIan")
            anchors.bottom: parent.bottom
            width: parent.width
            wrapMode: Text.NoWrap
            clip: true
            renderType: Qt.platform.os === "windows" ? Text.NativeRendering : Text.QtRendering
        }
    }

    // Log
    Item {
        id: itemLog
        anchors.centerIn: parent
        width: parent.width * 0.8
        height: parent.height * 0.5
        visible: false

        Rectangle {
            id: rectLogTitle
            width: parent.width
            height: textLogTitle.height
            color: "#aaaaaa"
            Text {
                id: textLogTitle
                text: "Log"
                width: parent.width
                horizontalAlignment: Text.AlignHCenter
                renderType: Qt.platform.os === "windows" ? Text.NativeRendering : Text.QtRendering
            }
        }

        TableView {
            id: tableLog
            model: listLog
            headerVisible: false
            width: parent.width
            anchors.top: rectLogTitle.bottom
            anchors.bottom: parent.bottom

            TableViewColumn {
                role: "timestamp"
                width: tableLog.width * 0.25
            }
            TableViewColumn {
                role: "text"
                width: tableLog.width * 0.75
            }
            itemDelegate: Item {

                Text {
                    anchors.verticalCenter: parent.verticalCenter
                    color: styleData.textColor
                    elide: styleData.elideMode
                    width: parent.width
                    font.pointSize: 10
                    text: {
                        if (styleData.column === 0)
                        {
                            Qt.formatDateTime(styleData.value, "hh:mm:ss");
                        }
                        else
                        {
                            styleData.value;
                        }
                    }
                    clip: true
                    renderType: Qt.platform.os === "windows" ? Text.NativeRendering : Text.QtRendering
                }
            }
        }
    }

    // Signal/Slot connection
    Connections {
        target: ctrlTop
        onLogMessage: {
            appendToLog (aStr);
        }
        onDeviceDisconnect: {
            ctrlTop.close();
            textStatus.text = "Device disconnected.";
            buttonConnect.icon = "qrc:///asset/link_broken.png";
            buttonConnect.text = "Connect";
            textTempreature.text = "-";
            textHumidity.text = "-";
            textPressure.text = "-";
        }
        onDeviceConnect: textStatus.text = ctrlTop.sensorName + " " + ctrlTop.sensorVersion + " connected."
        onDeviceInfoChanged: {
            var str;

            str = ctrlTop.deviceName;
            if (str.length === 0)
                textDeviceName.text = "-";
            else
                textDeviceName.text = str;

            textPID.text = qmlUtils.intToString("%04X", ctrlTop.devicePid);
            textVID.text = qmlUtils.intToString("%04X", ctrlTop.deviceVid);
        }
        onSensorChanged: {
            var temp;
            var humi;

            temp = parseFloat (ctrlTop.temperature);
            humi = parseFloat (ctrlTop.humidity)

            if (isNaN (temp))
            {
                textTempreature.text = "-"
            }
            else
            {
                if (showCelsius)
                {
                    textTempreature.text = temp.toFixed (1);
                }
                else
                {
                    temp = ctrlTop.temperature * 9 / 5 + 32;
                    textTempreature.text = temp.toFixed (1);
                }
            }

            if (isNaN (humi))
                textHumidity.text = "-";
            else
                textHumidity.text = humi.toFixed (1);

        }
    }


    // Event - completed
    Component.onCompleted: {
        ctrlTop.init();
    }

    // Event - width changed
    onWidthChanged: {
        var size;

        size = applicationTop.width / 5;
        if (size < 64)
            size = 64;
        buttonSize = size;
        console.log ("ButtonSize=" + size + " " + applicationTop.width + "x" + applicationTop.height);
    }

    // Event - height changed
    onHeightChanged: {
        var h;

        h = applicationTop.height;
        colReading.spacing = h / 10;
    }
}

