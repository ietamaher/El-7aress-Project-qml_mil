import QtQuick
import QtQuick.Shapes

Item {
    id: root

    property real azimuth: 0
    property color color: "#46E2A5"

    width: 100
    height: 100

    // Outer circle
    Shape {
        anchors.fill: parent

        ShapePath {
            strokeWidth: 2
            strokeColor: root.color
            fillColor: "transparent"

            PathAngleArc {
                centerX: root.width / 2
                centerY: root.height / 2
                radiusX: root.width / 2 - 2
                radiusY: root.height / 2 - 2
                startAngle: 0
                sweepAngle: 360
            }
        }
    }

    // Tick marks (every 30°)
    Repeater {
        model: 12 // 360° / 30° = 12 ticks

        delegate: Rectangle {
            property real angle: index * 30
            property bool isMajor: (index % 3 === 0) // Major ticks at 0, 90, 180, 270

            width: 2
            height: isMajor ? 8 : 4
            color: root.color

            x: root.width / 2 + (root.width / 2 - height) * Math.cos((angle - 90) * Math.PI / 180) - width / 2
            y: root.height / 2 + (root.height / 2 - height) * Math.sin((angle - 90) * Math.PI / 180) - height / 2
            rotation: angle
            transformOrigin: Item.Bottom
        }
    }

    // Cardinal labels (N, E, S, W)
    Repeater {
        model: [
            {angle: 0, label: "N"},
            {angle: 90, label: "E"},
            {angle: 180, label: "S"},
            {angle: 270, label: "W"}
        ]

        delegate: Text {
            text: modelData.label
            font.pixelSize: 12
            font.bold: true
            font.family: "Archivo Narrow"
            color: root.color

            x: root.width / 2 + (root.width / 2 + 12) * Math.cos((modelData.angle - 90) * Math.PI / 180) - width / 2
            y: root.height / 2 + (root.height / 2 + 12) * Math.sin((modelData.angle - 90) * Math.PI / 180) - height / 2
        }
    }

    // Needle
    Rectangle {
        width: 2
        height: root.height / 2 - 10
        color: root.color

        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.verticalCenter

        transformOrigin: Item.Bottom
        rotation: root.azimuth
    }

    // Azimuth text
    Text {
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.bottom
        anchors.topMargin: 5

        text: root.azimuth.toFixed(1) + "°"
        font.pixelSize: 12
        font.family: "Archivo Narrow"
        color: root.color
    }
}
