import QtQuick
import QtQuick.Shapes

Item {
    id: root
    property real elevation: 0
    property color color: "#46E2A5"
    property color outlineColor: "#000000"
    property real outlineWidth: 1

    readonly property real minElevation: -20
    readonly property real maxElevation: 60
    readonly property real elevationRange: maxElevation - minElevation

    width: 60
    height: 120

    // Main vertical scale line (thicker, more prominent)
    Rectangle {
        id: lineOutline
        width: 3 + root.outlineWidth
        height: parent.height
        x: 0
        color: root.outlineColor
    }

    Rectangle {
        id: line
        width: 3
        height: parent.height
        x: 0
        color: root.color
    }

    // Major tick marks (every 10 degrees) with labels at key points
    Repeater {
        model: {
            var ticks = [];
            for (var deg = -20; deg <= 60; deg += 10) {
                ticks.push(deg);
            }
            return ticks;
        }

        delegate: Item {
            property real elValue: modelData
            property real normPos: (elValue - root.minElevation) / root.elevationRange
            property bool isMajor: (elValue === 60 || elValue === 30 || elValue === 0 || elValue === -20)
            property real tickLength: isMajor ? 12 : 6

            y: root.height - (normPos * root.height) - 1
            x: 0

            // Tick outline
            Rectangle {
                width: parent.tickLength
                height: 2.5 + root.outlineWidth
                color: root.outlineColor
                x: 0
                y: -root.outlineWidth / 2
            }

            // Tick main
            Rectangle {
                width: parent.tickLength
                height: 2.5
                color: root.color
                x: 0
            }

            // Label for major ticks only
            Text {
                visible: parent.isMajor
                anchors.right: parent.horizontalCenter
                anchors.rightMargin:   5
                anchors.verticalCenter: parent.verticalCenter
                text: parent.elValue.toFixed(0)
                font.pixelSize: 11
                font.bold: true
                font.family: "Archivo Narrow"
                color: root.color
                style: Text.Outline
                styleColor: root.outlineColor
            }
        }
    }

    // Current elevation indicator - professional style with horizontal reference bar
    Item {
        id: indicatorContainer
        property real normPos: Math.max(0, Math.min(1, (root.elevation - root.minElevation) / root.elevationRange))
        property real yPos: root.height - (normPos * root.height)

        y: yPos
        x: 0
        width: root.width
        height: 1

        // Horizontal reference line extending from scale (outline)
        Rectangle {
            width: 20
            height: 2 + root.outlineWidth
            x: -2
            y: -1 - root.outlineWidth / 2
            color: root.outlineColor
        }

        // Horizontal reference line (main)
        Rectangle {
            width: 20
            height: 2
            x: -2
            y: -1
            color: root.color
        }

        // Triangle pointer (outline) - larger and more prominent
        Shape {
            x: 18
            y: -6
            width: 10
            height: 12

            ShapePath {
                strokeWidth: 1 + root.outlineWidth
                strokeColor: root.outlineColor
                fillColor: root.outlineColor
                startX: 0; startY: 6
                PathLine { x: 10; y: 0 }
                PathLine { x: 10; y: 12 }
                PathLine { x: 0; y: 6 }
            }
        }

        // Triangle pointer (main)
        Shape {
            x: 18
            y: -6
            width: 10
            height: 12

            ShapePath {
                strokeWidth: 1
                strokeColor: root.color
                fillColor: root.color
                startX: 0; startY: 6
                PathLine { x: 10; y: 0 }
                PathLine { x: 10; y: 12 }
                PathLine { x: 0; y: 6 }
            }
        }
    }

    // Current elevation value display box (background)
    Rectangle {
        id: valueBox
        property real normPos: Math.max(0, Math.min(1, (root.elevation - root.minElevation) / root.elevationRange))

        width: 45
        height: 18
        x: 30
        y: root.height - (normPos * root.height) - height / 2
        color: root.outlineColor
        opacity: 0.8
        radius: 2
    }

    // Current elevation value text
    Text {
        property real normPos: Math.max(0, Math.min(1, (root.elevation - root.minElevation) / root.elevationRange))

        anchors.centerIn: valueBox
        text: root.elevation.toFixed(1) + "°"
        font.pixelSize: 12
        font.bold: true
        font.family: "Archivo Narrow"
        color: root.color
    }
}
