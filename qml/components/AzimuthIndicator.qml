import QtQuick
import QtQuick.Shapes

Item {
    id: root

    // === INPUT PROPERTIES ===
    property real azimuth: 0                // Gimbal azimuth (relative to vehicle, 0-360°)
    property real vehicleHeading: 0         // Vehicle heading from IMU (0-360°, 0=North)
    property bool imuConnected: false       // Is IMU working?

    // === DISPLAY PROPERTIES ===
    property color color: "#46E2A5"
    property color outlineColor: "#000000"
    property real outlineWidth: 2
    property color relativeColor: "yellow"  // Color for relative angle indicator

    width: 100
    height: 100

    // === CALCULATED VALUES ===
    // True bearing (absolute): Where gimbal actually points in world coordinates
    readonly property real trueBearing: {
        if (!imuConnected) return azimuth;  // Fallback to relative if no IMU

        var combined = azimuth + vehicleHeading;

        // Normalize to 0-360°
        while (combined >= 360.0) combined -= 360.0;
        while (combined < 0.0) combined += 360.0;

        return combined;
    }

    // Relative angle: How far gimbal is from vehicle forward (always available)
    readonly property real relativeAngle: azimuth

    // === COMPASS ROSE (Rotates to align with true north) ===
    Item {
        id: compassRose
        anchors.fill: parent

        // Rotate compass so North points to true north
        // (Only when IMU connected)
        rotation: imuConnected ? -vehicleHeading : 0

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

        // Tick marks (every 30°)
        Repeater {
            model: 12
            delegate: Item {
                property real angle: index * 30
                property bool isMajor: (index % 3 === 0)
                property real tickLength: isMajor ? 10 : 5
                property real angleRad: (90 - angle) * Math.PI / 180
                property real outerRadius: root.width / 2 - 4
                property real innerRadius: outerRadius - tickLength

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
                style: Text.Outline
                styleColor: root.outlineColor
                x: root.width / 2 + (root.width / 2 + 12) * Math.cos((modelData.angle - 90) * Math.PI / 180) - width / 2
                y: root.height / 2 + (root.height / 2 + 12) * Math.sin((modelData.angle - 90) * Math.PI / 180) - height / 2
            }
        }
    }

    // === VEHICLE FORWARD INDICATOR (Yellow Reference Line) ===
    // Shows where vehicle is pointing - only visible when IMU connected
    Item {
        anchors.fill: parent
        visible: root.imuConnected

        // Yellow line pointing up = vehicle forward direction
        Rectangle {
            width: 2
            height: root.height / 2 - 8
            color: root.relativeColor
            opacity: 0.7
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.bottom: parent.verticalCenter
            transformOrigin: Item.Bottom
        }

        // Small triangle at the end
        Rectangle {
            width: 8
            height: 8
            color: root.relativeColor
            opacity: 0.7
            rotation: 45
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.bottom: parent.top
            anchors.bottomMargin: root.height / 2 - 16
        }

        // "V" label for vehicle
        Text {
            text: "V"
            font.pixelSize: 10
            font.bold: true
            font.family: "Archivo Narrow"
            color: root.relativeColor
            style: Text.Outline
            styleColor: root.outlineColor
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.bottom: parent.top
            anchors.bottomMargin: root.height / 2 - 28
        }
    }

    // === TARGET NEEDLE (Green) ===
    // Points to where gimbal is actually aiming in world coordinates
    Item {
        anchors.fill: parent
        rotation: root.trueBearing
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

        // Main needle (green)
        Rectangle {
            width: 3
            height: root.height / 2 - 10
            color: root.color
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.bottom: parent.verticalCenter
            transformOrigin: Item.Bottom
        }

        // Arrow tip
        Rectangle {
            width: 10
            height: 10
            color: root.color
            rotation: 45
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.bottom: parent.top
            anchors.bottomMargin: root.height / 2 - 20
        }
    }

    // === TEXT DISPLAY ===
    Column {
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.bottom
        anchors.topMargin: 5
        spacing: 3

        // === TRUE BEARING (Primary - Bold) ===
        Row {
            anchors.horizontalCenter: parent.horizontalCenter
            spacing: 3

            Text {
                text: root.trueBearing.toFixed(1) + "°"
                font.pixelSize: 16
                font.bold: true
                font.family: "Archivo Narrow"
                color: root.color
                style: Text.Outline
                styleColor: root.outlineColor
            }

            // "T" for True (only show if IMU connected)
            Text {
                visible: root.imuConnected
                text: "T"
                font.pixelSize: 10
                font.bold: true
                font.family: "Archivo Narrow"
                color: root.color
                style: Text.Outline
                styleColor: root.outlineColor
                anchors.verticalCenter: parent.verticalCenter
            }
        }

        // === RELATIVE ANGLE (Secondary - Smaller, Yellow) ===
        // Shows how far gimbal is from vehicle forward
        Row {
            anchors.horizontalCenter: parent.horizontalCenter
            spacing: 3
            visible: root.imuConnected  // Only show when we have IMU (otherwise same as true bearing)

            Text {
                text: root.relativeAngle.toFixed(1) + "°"
                font.pixelSize: 11
                font.family: "Archivo Narrow"
                color: root.relativeColor
                style: Text.Outline
                styleColor: root.outlineColor
            }

            Text {
                text: "REL"
                font.pixelSize: 8
                font.family: "Archivo Narrow"
                color: root.relativeColor
                style: Text.Outline
                styleColor: root.outlineColor
                anchors.verticalCenter: parent.verticalCenter
            }
        }

        // === IMU STATUS WARNING ===
        Text {
            anchors.horizontalCenter: parent.horizontalCenter
            visible: !root.imuConnected
            text: "NO IMU"
            font.pixelSize: 9
            font.bold: true
            font.family: "Archivo Narrow"
            color: "#C81428"  // Red
            style: Text.Outline
            styleColor: root.outlineColor
        }
    }

    // === OPTIONAL: Vehicle Heading Readout (Bottom) ===
    Text {
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.bottom
        anchors.topMargin: 55
        visible: root.imuConnected
        text: "VEH: " + root.vehicleHeading.toFixed(1) + "°"
        font.pixelSize: 9
        font.family: "Archivo Narrow"
        color: root.relativeColor
        style: Text.Outline
        styleColor: root.outlineColor
        opacity: 0.8
    }
}
