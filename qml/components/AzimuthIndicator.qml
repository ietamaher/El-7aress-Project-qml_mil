import QtQuick
import QtQuick.Shapes

Item {
    id: root
    property real azimuth: 0
    property color color: "#46E2A5"
    property color outlineColor: "#000000"
    property real outlineWidth: 2

    width: 100
    height: 100

    // Outer circle with outline
    Shape {
        anchors.fill: parent

        // Outline
        ShapePath {
            strokeWidth: 2 + root.outlineWidth
            strokeColor: root.outlineColor
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

        // Main circle
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

    // Tick marks with outlines (every 30°)
    Repeater {
        model: 12
        delegate: Item {
            property real angle: index * 30
            property bool isMajor: (index % 3 === 0)
            property real tickLength: isMajor ? 10 : 5
            property real angleRad: (90 - angle) * Math.PI / 180
            property real outerRadius: root.width / 2 - 4
            property real innerRadius: outerRadius - tickLength

            // Calculate start and end points (matching C++ logic)
            property real endX: root.width / 2 + innerRadius * Math.cos(angleRad)
            property real endY: root.height / 2 - innerRadius * Math.sin(angleRad)
            property real startX: root.width / 2 + outerRadius * Math.cos(angleRad)
            property real startY: root.height / 2 - outerRadius * Math.sin(angleRad)

            // Outline
            Rectangle {
                width: 2 + root.outlineWidth
                height: tickLength
                color: root.outlineColor
                x: startX - width / 2
                y: startY
                rotation: angle
                transformOrigin: Item.Top
            }

            // Main tick
            Rectangle {
                width: 2
                height: tickLength
                color: root.color
                x: startX - width / 2
                y: startY
                rotation: angle
                transformOrigin: Item.Top
            }
        }
    }

    // Cardinal labels with outline (N, E, S, W)
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
            style: Text.Outline
            styleColor: root.outlineColor
            x: root.width / 2 + (root.width / 2 + 12) * Math.cos((modelData.angle - 90) * Math.PI / 180) - width / 2
            y: root.height / 2 + (root.height / 2 + 12) * Math.sin((modelData.angle - 90) * Math.PI / 180) - height / 2
        }
    }

    // Needle with outline
    Item {
        anchors.fill: parent
        rotation: root.azimuth
        transformOrigin: Item.Center

        // Outline
        Rectangle {
            width: 2 + root.outlineWidth
            height: root.height / 2 - 10
            color: root.outlineColor
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.bottom: parent.verticalCenter
            transformOrigin: Item.Bottom
        }

        // Main needle
        Rectangle {
            width: 2
            height: root.height / 2 - 10
            color: root.color
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.bottom: parent.verticalCenter
            transformOrigin: Item.Bottom
        }
    }

    // Azimuth text with outline
    Text {
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.bottom
        anchors.topMargin: 20
        text: "Azimuth: " + root.azimuth.toFixed(1) + "°"
        font.pixelSize: 12
        font.family: "Archivo Narrow"
        color: root.color
        style: Text.Outline
        styleColor: root.outlineColor
    }
}
