import QtQuick
import QtQuick.Shapes

Item {
    id: root

    property real elevation: 0
    property color color: "#46E2A5"

    readonly property real minElevation: -20
    readonly property real maxElevation: 90
    readonly property real elevationRange: maxElevation - minElevation

    width: 40
    height: 120

    // Vertical line
    Rectangle {
        width: 2
        height: parent.height
        anchors.right: parent.right
        color: root.color
    }

    // Tick marks
    Repeater {
        model: [60, 30, 0, -20] // Major elevation marks

        delegate: Item {
            property real elValue: modelData
            property real normPos: (elValue - root.minElevation) / root.elevationRange

            y: root.height - (normPos * root.height) - 1
            x: root.width - 10
            width: 10
            height: 2

            Rectangle {
                anchors.fill: parent
                color: root.color
            }

            Text {
                anchors.right: parent.left
                anchors.rightMargin: 3
                anchors.verticalCenter: parent.verticalCenter

                text: elValue + "°"
                font.pixelSize: 10
                font.family: "Archivo Narrow"
                color: root.color
            }
        }
    }

    // Elevation indicator (triangle)
    Shape {
        property real normPos: (root.elevation - root.minElevation) / root.elevationRange
        property real yPos: root.height - (normPos * root.height)

        x: root.width - 18
        y: yPos - 4
        width: 8
        height: 8

        ShapePath {
            strokeWidth: 1
            strokeColor: root.color
            fillColor: root.color

            startX: 8; startY: 4
            PathLine { x: 0; y: 0 }
            PathLine { x: 0; y: 8 }
            PathLine { x: 8; y: 4 }
        }
    }

    // Elevation value text
    Text {
        anchors.right: parent.left
        anchors.rightMargin: 25
        y: {
            var normPos = (root.elevation - root.minElevation) / root.elevationRange;
            return root.height - (normPos * root.height) - height / 2;
        }

        text: root.elevation.toFixed(1) + "°"
        font.pixelSize: 12
        font.bold: true
        font.family: "Archivo Narrow"
        color: root.color
    }
}
