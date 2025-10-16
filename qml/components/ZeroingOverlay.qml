// ZeroingOverlay.qml
import QtQuick
import QtQuick.Controls
import QtQuick.Effects

Rectangle {
    id: zeroingRoot

    width: 500
    height: 400
    radius: 5
    color: Qt.rgba(0, 0, 0, 0.85)
    
    border.width: 2

    visible: zeroingViewModel ? zeroingViewModel.visible : false

    property color accentColor: zeroingViewModel ? zeroingViewModel.accentColor : "#46E2A5"
    border.color: accentColor

    // Subtle drop shadow
    layer.enabled: true
    layer.effect: MultiEffect {
        shadowEnabled: true
        shadowColor: "#80000000"
        shadowBlur: 0.3
        shadowVerticalOffset: 6
    }

    Column {
        anchors.fill: parent
        anchors.margins: 25
        spacing: 20

        // Title
        Text {
            text: zeroingViewModel ? zeroingViewModel.title : "Weapon Zeroing"
            font.pixelSize: 24
            font.bold: true
            color: accentColor
            horizontalAlignment: Text.AlignHCenter
            anchors.horizontalCenter: parent.horizontalCenter
        }

        Rectangle {
            width: parent.width
            height: 2
            color: accentColor
            opacity: 0.3
            anchors.horizontalCenter: parent.horizontalCenter
        }

        // Instruction text
        Text {
            width: parent.width
            text: zeroingViewModel ? zeroingViewModel.instruction : ""
            font.pixelSize: 16
            color: "#EEEEEE"
            wrapMode: Text.WordWrap
            horizontalAlignment: Text.AlignHCenter
            minimumPixelSize: 12
        }

        Item { height: 10 } // Spacer

        // Status text
        Text {
            width: parent.width
            text: zeroingViewModel ? zeroingViewModel.status : ""
            font.pixelSize: 18
            font.bold: true
            color: "#00FF99"
            wrapMode: Text.WordWrap
            horizontalAlignment: Text.AlignHCenter
        }

        // Optional: Show offsets when in completed state
        Rectangle {
            width: parent.width
            height: 60
            color: Qt.rgba(0, 1, 0.6, 0.1)
            radius: 3
            border.color: accentColor
            border.width: 1
            visible: zeroingViewModel ? zeroingViewModel.showOffsets : false

            Column {
                anchors.centerIn: parent
                spacing: 5

                Text {
                    text: "Azimuth Offset: " + (zeroingViewModel ? zeroingViewModel.azimuthOffset.toFixed(2) : "0.00")
                    font.pixelSize: 14
                    color: "#FFFFFF"
                    anchors.horizontalCenter: parent.horizontalCenter
                }

                Text {
                    text: "Elevation Offset: " + (zeroingViewModel ? zeroingViewModel.elevationOffset.toFixed(2) : "0.00")
                    font.pixelSize: 14
                    color: "#FFFFFF"
                    anchors.horizontalCenter: parent.horizontalCenter
                }
            }
        }

        //Item { Layout.fillHeight: true } // Push button to bottom

        // Return button hint
        Text {
            width: parent.width
            text: "Press MENU/VAL to confirm"
            font.pixelSize: 12
            color: "#888888"
            horizontalAlignment: Text.AlignHCenter
            anchors.horizontalCenter: parent.horizontalCenter
        }
    }
}
