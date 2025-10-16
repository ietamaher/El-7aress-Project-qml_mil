import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    id: root

    property var viewModel: null
    property color accentColor: Qt.rgba(70, 226, 165, 1.0)

    color: Qt.rgba(0, 0.16, 0.12, 0.8)
    radius: 5
    border.color: accentColor
    border.width: 1

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 15
        spacing: 12

        // Title
        Text {
            Layout.fillWidth: true
            text: "Sector Scan Parameters"
            font.pixelSize: 16
            font.bold: true
            font.family: "Archivo Narrow"
            color: accentColor
            horizontalAlignment: Text.AlignHCenter
        }

        // Enabled field
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 40
            radius: 3
            color: (viewModel && viewModel.activeField === 0) ? accentColor : "transparent"
            border.color: accentColor
            border.width: 1

            RowLayout {
                anchors.fill: parent
                anchors.margins: 8
                spacing: 10

                Text {
                    text: "Enabled:"
                    font.pixelSize: 14
                    font.family: "Archivo Narrow"
                    font.weight: Font.DemiBold
                    color: (viewModel && viewModel.activeField === 0) ? "black" : accentColor
                }

                Rectangle {
                    width: 24
                    height: 24
                    radius: 3
                    border.color: accentColor
                    border.width: 2
                    color: (viewModel && viewModel.isEnabled) ? accentColor : "transparent"

                    Text {
                        text: "✓"
                        font.pixelSize: 18
                        font.bold: true
                        color: "black"
                        anchors.centerIn: parent
                        visible: viewModel && viewModel.isEnabled
                    }
                }

                Item { Layout.fillWidth: true }
            }
        }

        // Scan Speed field
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 40
            radius: 3
            color: (viewModel && viewModel.activeField === 1) ?
                   (viewModel.isEditingValue ? Qt.rgba(255, 152, 0, 1.0) : accentColor) : "transparent"
            border.color: accentColor
            border.width: 1

            RowLayout {
                anchors.fill: parent
                anchors.margins: 8
                spacing: 10

                Text {
                    text: "Scan Speed:"
                    font.pixelSize: 14
                    font.family: "Archivo Narrow"
                    font.weight: Font.DemiBold
                    color: (viewModel && viewModel.activeField === 1) ? "black" : accentColor
                }

                Text {
                    text: viewModel ? viewModel.scanSpeed + " deg/s" : "5 deg/s"
                    font.pixelSize: 14
                    font.family: "Archivo Narrow"
                    font.weight: Font.Bold
                    color: (viewModel && viewModel.activeField === 1) ? "black" : accentColor
                }

                Text {
                    text: "[EDIT]"
                    font.pixelSize: 12
                    font.family: "Archivo Narrow"
                    font.italic: true
                    color: Qt.rgba(255, 152, 0, 1.0)
                    visible: viewModel && viewModel.isEditingValue && viewModel.activeField === 1
                }

                Item { Layout.fillWidth: true }
            }
        }

        Item { Layout.fillHeight: true }  // Spacer

        // Buttons
        RowLayout {
            Layout.fillWidth: true
            Layout.preferredHeight: 45
            spacing: 15

            Rectangle {
                Layout.fillWidth: true
                Layout.fillHeight: true
                radius: 3
                color: (viewModel && viewModel.activeField === 2) ? accentColor : "transparent"
                border.color: accentColor
                border.width: 2

                Text {
                    text: "Validate"
                    font.pixelSize: 14
                    font.bold: true
                    font.family: "Archivo Narrow"
                    color: (viewModel && viewModel.activeField === 2) ? "black" : accentColor
                    anchors.centerIn: parent
                }
            }

            Rectangle {
                Layout.fillWidth: true
                Layout.fillHeight: true
                radius: 3
                color: (viewModel && viewModel.activeField === 3) ? Qt.rgba(200, 20, 40, 1.0) : "transparent"
                border.color: Qt.rgba(200, 20, 40, 1.0)
                border.width: 2

                Text {
                    text: "Cancel"
                    font.pixelSize: 14
                    font.bold: true
                    font.family: "Archivo Narrow"
                    color: (viewModel && viewModel.activeField === 3) ? "white" : Qt.rgba(200, 20, 40, 1.0)
                    anchors.centerIn: parent
                }
            }
        }

        // Instructions
        Text {
            Layout.fillWidth: true
            text: viewModel && viewModel.isEditingValue ?
                  "UP/DOWN to adjust value • MENU/VAL to confirm" :
                  "UP/DOWN to navigate • MENU/VAL to edit/confirm"
            font.pixelSize: 10
            font.family: "Archivo Narrow"
            color: Qt.rgba(136, 136, 136, 1.0)
            horizontalAlignment: Text.AlignHCenter
            wrapMode: Text.WordWrap
        }
    }
}
