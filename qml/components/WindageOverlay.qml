// WindageOverlay.qml
import QtQuick
import QtQuick.Controls
import QtQuick.Effects

Rectangle {
    id: windageRoot

    width: 500
    height: 380
    radius: 5
    color: Qt.rgba(0, 0, 0, 0.85)
 
    border.width: 2

    visible: windageViewModel ? windageViewModel.visible : false

   property color accentColor: windageViewModel ? windageViewModel.accentColor : "#46E2A5"
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
            text: windageViewModel ? windageViewModel.title : "Windage Setting"
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
            text: windageViewModel ? windageViewModel.instruction : ""
            font.pixelSize: 16
            color: "#EEEEEE"
            wrapMode: Text.WordWrap
            horizontalAlignment: Text.AlignHCenter
            minimumPixelSize: 12
        }

        Item { height: 15 } // Spacer

        // Wind speed display (shown during speed setting and completion)
        Rectangle {
            width: parent.width
            height: 80
            color: Qt.rgba(0.3, 0.8, 0.77, 0.15)
            radius: 5
            border.color: accentColor
            border.width: 2
            visible: windageViewModel ? windageViewModel.showWindSpeed : false

            Column {
                anchors.centerIn: parent
                spacing: 10

                Text {
                    text: windageViewModel ? windageViewModel.windSpeedLabel : "Headwind: 0 knots"
                    font.pixelSize: 20
                    font.bold: true
                    color: accentColor
                    anchors.horizontalCenter: parent.horizontalCenter
                }

                // Visual wind indicator
                Row {
                    spacing: 5
                    anchors.horizontalCenter: parent.horizontalCenter

                    Repeater {
                        model: windageViewModel ? Math.min(10, Math.floor(windageViewModel.windSpeed / 5)) : 0

                        Rectangle {
                            width: 8
                            height: 20
                            color: accentColor
                            radius: 2
                        }
                    }
                }
            }
        }

        //Item { Layout.fillHeight: true } // Push hint to bottom

        // Control hints
        Text {
            width: parent.width
            text: windageViewModel && windageViewModel.showWindSpeed ?
                  "Use UP/DOWN to adjust | MENU/VAL to confirm" :
                  "Press MENU/VAL when aligned"
            font.pixelSize: 12
            color: "#888888"
            horizontalAlignment: Text.AlignHCenter
            anchors.horizontalCenter: parent.horizontalCenter
        }
    }
}
