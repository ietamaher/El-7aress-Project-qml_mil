// AreaZoneParameterPanel.qml
import QtQuick
import QtQuick.Controls

Rectangle {
    id: root

    property var viewModel: null
    property color accentColor: "#4ECDC4"

    color: Qt.rgba(0, 0.16, 0.12, 0.8)
    radius: 5

    Column {
        anchors.fill: parent
        anchors.margins: 15
        spacing: 12

        // Title
        Text {
            text: "Area Zone Parameters"
            font.pixelSize: 16
            font.bold: true
            color: accentColor
            anchors.horizontalCenter: parent.horizontalCenter
        }

        // Enabled field
        Row {
            width: parent.width
            height: 35
            spacing: 10

            Rectangle {
                width: parent.width
                height: parent.height
                radius: 3
                color: (viewModel && viewModel.activeField === 0) ? accentColor : "transparent"
                border.color: accentColor
                border.width: 1

                Row {
                    anchors.fill: parent
                    anchors.margins: 8
                    spacing: 10

                    Text {
                        text: "Enabled:"
                        font.pixelSize: 14
                        color: (viewModel && viewModel.activeField === 0) ? "black" : "white"
                        anchors.verticalCenter: parent.verticalCenter
                    }

                    Rectangle {
                        width: 20
                        height: 20
                        radius: 3
                        border.color: accentColor
                        border.width: 2
                        color: (viewModel && viewModel.isEnabled) ? accentColor : "transparent"
                        anchors.verticalCenter: parent.verticalCenter

                        // Checkmark
                        Text {
                            text: "✓"
                            font.pixelSize: 16
                            font.bold: true
                            color: "black"
                            anchors.centerIn: parent
                            visible: viewModel && viewModel.isEnabled
                        }
                    }
                }
            }
        }

        // Overridable field
        Row {
            width: parent.width
            height: 35
            spacing: 10

            Rectangle {
                width: parent.width
                height: parent.height
                radius: 3
                color: (viewModel && viewModel.activeField === 1) ? accentColor : "transparent"
                border.color: accentColor
                border.width: 1

                Row {
                    anchors.fill: parent
                    anchors.margins: 8
                    spacing: 10

                    Text {
                        text: "Overridable:"
                        font.pixelSize: 14
                        color: (viewModel && viewModel.activeField === 1) ? "black" : "white"
                        anchors.verticalCenter: parent.verticalCenter
                    }

                    Rectangle {
                        width: 20
                        height: 20
                        radius: 3
                        border.color: accentColor
                        border.width: 2
                        color: (viewModel && viewModel.isOverridable) ? accentColor : "transparent"
                        anchors.verticalCenter: parent.verticalCenter

                        Text {
                            text: "✓"
                            font.pixelSize: 16
                            font.bold: true
                            color: "black"
                            anchors.centerIn: parent
                            visible: viewModel && viewModel.isOverridable
                        }
                    }
                }
            }
        }

        Item { height: 10 }  // Spacer

        // Buttons
        Row {
            width: parent.width
            height: 40
            spacing: 15

            Rectangle {
                width: (parent.width - 15) / 2
                height: parent.height
                radius: 3
                color: (viewModel && viewModel.activeField === 2) ? accentColor : "transparent"
                border.color: accentColor
                border.width: 2

                Text {
                    text: "Validate"
                    font.pixelSize: 14
                    font.bold: true
                    color: (viewModel && viewModel.activeField === 2) ? "black" : accentColor
                    anchors.centerIn: parent
                }
            }

            Rectangle {
                width: (parent.width - 15) / 2
                height: parent.height
                radius: 3
                color: (viewModel && viewModel.activeField === 3) ? "#C81428" : "transparent"
                border.color: "#C81428"
                border.width: 2

                Text {
                    text: "Cancel"
                    font.pixelSize: 14
                    font.bold: true
                    color: (viewModel && viewModel.activeField === 3) ? "white" : "#C81428"
                    anchors.centerIn: parent
                }
            }
        }

        // Instructions
        Text {
            width: parent.width
            text: "UP/DOWN to navigate • MENU/VAL to toggle/confirm"
            font.pixelSize: 11
            color: "#888888"
            horizontalAlignment: Text.AlignHCenter
            wrapMode: Text.WordWrap
        }
    }
}
