import QtQuick
import QtQuick.Controls

Rectangle {
    id: menuroot

    property var viewModel: null
    property color osdColor: "green"
    // --- END OF FIX ---

    // The rest of the file uses these properties
    visible: viewModel ? viewModel.visible : false
    width: 350
    height: 500
    color: "rgba(0, 0, 0, 0.8)"
    border.color: "gray"

    Column {
        anchors.fill: parent
        anchors.margins: 10
        spacing: 5

        Text {
            text: viewModel ? viewModel.title : "Menu"
            font.pixelSize: 24
            font.bold: true
            color: root.osdColor // Using the property here
        }

        Text {
            text: viewModel ? viewModel.description : ""
            font.pixelSize: 16
            color: "lightgray"
            visible: text.length > 0
        }

        Rectangle { width: parent.width; height: 1; color: "gray"; anchors.topMargin: 5 }

        ListView {
            id: listView
            width: parent.width
            height: parent.height - 120
            model: viewModel ? viewModel.optionsModel : null
            currentIndex: viewModel ? viewModel.currentIndex : -1
            focus: true
            clip: true

            delegate: ItemDelegate {
                width: listView.width
                text: modelData
                readonly property bool isSeparator: modelData.startsWith("---")

                enabled: !isSeparator
                font.italic: isSeparator
                font.bold: isSeparator
                //color: isSeparator ? "gray" : (highlighted ? "black" : root.osdColor)
                background: Rectangle { color: highlighted ? root.osdColor : "transparent" }
                highlighted: ListView.isCurrentItem
            }
        }
    }
}
