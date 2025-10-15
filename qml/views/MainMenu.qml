import QtQuick
import QtQuick.Controls
import QtQuick.Effects

Rectangle {
    id: mainmenuroot
    property var osdViewModel: null // Public property to receive the C++ model

    width: 380
    height: 520
    radius: 5
    color: Qt.rgba(0, 0, 0, 0.6)
    border.color: "#404040"
    border.width: 1
    visible: viewModel ? viewModel.visible : false

    property var viewModel: null
    property color accentColor: "#00FF99"

    // Subtle drop shadow using new Effects API
    layer.enabled: true
    layer.effect: MultiEffect {
        shadowEnabled: true
        shadowColor: "#80000000"
        shadowBlur: 0.3
        shadowVerticalOffset: 6
    }

    Column {
        anchors.fill: parent
        anchors.margins: 20
        spacing: 12

        Text {
            text: viewModel ? viewModel.title : "Main Menu"
            font.pixelSize: 28
            font.bold: true
            color: accentColor
            horizontalAlignment: Text.AlignHCenter
            anchors.horizontalCenter: parent.horizontalCenter
        }

        Text {
            text: viewModel ? viewModel.description : "Select an option"
            font.pixelSize: 16
            color: "#CCCCCC"
            horizontalAlignment: Text.AlignHCenter
            anchors.horizontalCenter: parent.horizontalCenter
        }

        Rectangle {
            width: parent.width
            height: 1
            color: "#505050"
            opacity: 0.6
            anchors.horizontalCenter: parent.horizontalCenter
        }

        ListView {
            id: listView
            width: parent.width
            height: parent.height - 140
            spacing: 6
            clip: true

            model: viewModel ? viewModel.optionsModel : null

            // KEY FIX: Bind the ListView's currentIndex to the ViewModel's currentIndex
            currentIndex: viewModel ? viewModel.currentIndex : -1

            // Ensure the current item is always visible when selection changes
            highlightFollowsCurrentItem: true
            highlightMoveDuration: 200
            highlightMoveVelocity: -1

            delegate: ItemDelegate {
                width: listView.width
                text: model.display
                font.pixelSize: 18
                font.bold: model.display.startsWith("---")
                opacity: model.display.startsWith("---") ? 0.5 : 1.0
                enabled: !model.display.startsWith("---")

                background: Rectangle {
                    radius: 3
                    color: highlighted ? accentColor : Qt.rgba(1, 1, 1, 0.08)
                    border.color: highlighted ? "#FFFFFF" : "transparent"
                    border.width: highlighted ? 2 : 0

                    Behavior on color { ColorAnimation { duration: 200 } }
                    Behavior on border.color { ColorAnimation { duration: 200 } }
                }

                contentItem: Text {
                    text: model.display
                    color: highlighted ? "black" : "white"
                    font.pixelSize: 18
                    font.bold: model.display.startsWith("---")
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignHCenter
                    anchors.fill: parent
                }

                // This is the key property that determines highlighting
                highlighted: ListView.isCurrentItem
            }

            ScrollBar.vertical: ScrollBar {
                policy: ScrollBar.AsNeeded
                active: true
            }
        }
    }
}
