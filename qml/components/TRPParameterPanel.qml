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
        spacing: 10

        // Title
        Text {
            Layout.fillWidth: true
            text: "TRP Parameters"
            font.pixelSize: 16
            font.bold: true
            font.family: "Archivo Narrow"
            color: accentColor
            horizontalAlignment: Text.AlignHCenter
        }

        // Location Page field
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 35
            radius: 3
            color: (viewModel && viewModel.activeField === 0) ?
                   (viewModel.isEditingValue ? Qt.rgba(255, 152, 0, 1.0) : accentColor) : "transparent"
            border.color: accentColor
            border.width: 1

            RowLayout {
                anchors.fill: parent
                anchors.margins: 8
                spacing: 10

                Text {
                    text: "Location Page:"
                    font.pixelSize: 13
                    font.family: "Archivo Narrow"
                    font.weight: Font.DemiBold
                    color: (viewModel && viewModel.activeField === 0) ? "black" : accentColor
                }

                Text {
                    text: viewModel ? viewModel.locationPage : "1"
                    font.pixelSize: 13
                    font.family: "Archivo Narrow"
                    font.weight: Font.Bold
                    color: (viewModel && viewModel.activeField === 0) ? "black" : accentColor
                }

                Text {
                    text: "[EDIT]"
                    font.pixelSize: 11
                    font.family: "Archivo Narrow"
                    font.italic: true
                    color: Qt.rgba(255, 152, 0, 1.0)
                    visible: viewModel && viewModel.isEditingValue && viewModel.activeField === 0
                }

                Item { Layout.fillWidth: true }
            }
        }

        // TRP In Page field
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 35
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
                    text: "TRP Index:"
                    font.pixelSize: 13
                    font.family: "Archivo Narrow"
                    font.weight: Font.DemiBold
                    color: (viewModel && viewModel.activeField === 1) ? "black" : accentColor
                }

                Text {
                    text: viewModel ? viewModel.trpInPage : "1"
                    font.pixelSize: 13
                    font.family: "Archivo Narrow"
                    font.weight: Font.Bold
                    color: (viewModel && viewModel.activeField === 1) ? "black" : accentColor
                }

                Text {
                    text: "[EDIT]"
                    font.pixelSize: 11
                    font.family: "Archivo Narrow"
                    font.italic: true
                    color: Qt.rgba(255, 152, 0, 1.0)
                    visible: viewModel && viewModel.isEditingValue && viewModel.activeField === 1
                }

                Item { Layout.fillWidth: true }
            }
        }

        // Halt Time field
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 35
            radius: 3
            color: (viewModel && viewModel.activeField === 2) ?
                   (viewModel.isEditingValue ? Qt.rgba(255, 152, 0, 1.0) : accentColor) : "transparent"
            border.color: accentColor
            border.width: 1

            RowLayout {
                anchors.fill: parent
                anchors.margins: 8
                spacing: 10

                Text {
                    text: "Halt Time:"
                    font.pixelSize: 13
                    font.family: "Archivo Narrow"
                    font.weight: Font.DemiBold
                    color: (viewModel && viewModel.activeField === 2) ? "black" : accentColor
                }

                Text {
                    text: viewModel ? viewModel.haltTime.toFixed(1) + " sec" : "1.0 sec"
                    font.pixelSize: 13
                    font.family: "Archivo Narrow"
                    font.weight: Font.Bold
                    color: (viewModel && viewModel.activeField === 2) ? "black" : accentColor
                }

                Text {
                    text: "[EDIT]"
                    font.pixelSize: 11
                    font.family: "Archivo Narrow"
                    font.italic: true
                    color: Qt.rgba(255, 152, 0, 1.0)
                    visible: viewModel && viewModel.isEditingValue && viewModel.activeField === 2
                }

                Item { Layout.fillWidth: true }
            }
        }

        Item { Layout.fillHeight: true }  // Spacer

        // Buttons
        RowLayout {
            Layout.fillWidth: true
            Layout.preferredHeight: 40
            spacing: 15

            Rectangle {
                Layout.fillWidth: true
                Layout.fillHeight: true
                radius: 3
                color: (viewModel && viewModel.activeField === 3) ? accentColor : "transparent"
                border.color: accentColor
                border.width: 2

                Text {
                    text: "Validate"
                    font.pixelSize: 14
                    font.bold: true
                    font.family: "Archivo Narrow"
                    color: (viewModel && viewModel.activeField === 3) ? "black" : accentColor
                    anchors.centerIn: parent
                }
            }

            Rectangle {
                Layout.fillWidth: true
                Layout.fillHeight: true
                radius: 3
                color: (viewModel && viewModel.activeField === 4) ? Qt.rgba(200, 20, 40, 1.0) : "transparent"
                border.color: Qt.rgba(200, 20, 40, 1.0)
                border.width: 2

                Text {
                    text: "Cancel"
                    font.pixelSize: 14
                    font.bold: true
                    font.family: "Archivo Narrow"
                    color: (viewModel && viewModel.activeField === 4) ? "white" : Qt.rgba(200, 20, 40, 1.0)
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
