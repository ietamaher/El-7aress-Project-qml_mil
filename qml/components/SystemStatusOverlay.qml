import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Effects


Rectangle {
    id: statusRoot
    width: Math.min(parent.width - 20, 800)
    height: Math.min(parent.height - 20, 600)
    radius: 8
    color: "#0A0A0A"
    border.width: 1
    border.color: "#1A1A1A"

    visible: viewModel ? viewModel.visible : false
    property var viewModel: systemStatusViewModel
    property color accentColor: viewModel ? viewModel.accentColor : "#46E2A5"

    layer.enabled: true
    layer.effect: MultiEffect {
        shadowEnabled: true
        shadowColor: "#80000000"
        shadowBlur: 0.5
        shadowVerticalOffset: 10
    }

    Column {
        anchors.fill: parent
        anchors.margins: 0
        spacing: 0

        // ====================================================================
        // HEADER SECTION
        // ====================================================================
        Rectangle {
            width: parent.width
            height: 80
            color: "transparent"

            Column {
                anchors.centerIn: parent
                spacing: 8

                Text {
                    text: "System Status"
                    font.pixelSize: 22
                    font.weight: Font.Bold
                    font.family: "Segoe UI"
                    color: "#FFFFFF"
                    horizontalAlignment: Text.AlignHCenter
                    anchors.horizontalCenter: parent.horizontalCenter
                }

                Text {
                    text: "Hardware Health Monitor"
                    font.pixelSize: 12
                    font.family: "Segoe UI"
                    color: "#808080"
                    horizontalAlignment: Text.AlignHCenter
                    anchors.horizontalCenter: parent.horizontalCenter
                }
            }
        }

        Rectangle {
            width: parent.width
            height: 1
            color: "#151515"
        }

        // ====================================================================
        // MAIN CONTENT - 3 COLUMNS
        // ====================================================================
        Item {
            width: parent.width
            height: parent.height - 130

            Row {
                anchors.fill: parent
                anchors.margins: 15
                spacing: 12

                // ============================================================
                // COLUMN 1: MOTION SYSTEMS
                // ============================================================
                Column {
                    width: (parent.width - 24) / 3
                    height: parent.height
                    spacing: 10

                    // --- Azimuth Servo ---
                    DeviceCard {
                        title: "Azimuth Servo"
                        width: parent.width
                        connected: viewModel ? viewModel.azConnected : false
                        hasError: viewModel ? viewModel.azFault : false
                        accent: accentColor

                        Column {
                            anchors.fill: parent
                            spacing: 3

                            StatusRow { label: "Position:"; value: viewModel ? viewModel.azPositionText : "N/A" }
                            StatusRow { label: "RPM:"; value: viewModel ? viewModel.azRpmText : "N/A" }
                            StatusRow { label: "Torque:"; value: viewModel ? viewModel.azTorqueText : "N/A" }
                            StatusRow { label: "Motor:"; value: viewModel ? viewModel.azMotorTempText : "N/A" }
                            StatusRow { label: "Driver:"; value: viewModel ? viewModel.azDriverTempText : "N/A" }

                            Text {
                                text: viewModel && viewModel.azFault ? "⚠ FAULT" : "✓ OK"
                                font.pixelSize: 11
                                font.bold: true
                                font.family: "Segoe UI"
                                color: viewModel && viewModel.azFault ? "#FF6B6B" : accentColor
                                topPadding: 2
                            }
                        }
                    }

                    // --- Elevation Servo ---
                    DeviceCard {
                        title: "Elevation Servo"
                        width: parent.width
                        connected: viewModel ? viewModel.elConnected : false
                        hasError: viewModel ? viewModel.elFault : false
                        accent: accentColor

                        Column {
                            anchors.fill: parent
                            spacing: 3

                            StatusRow { label: "Position:"; value: viewModel ? viewModel.elPositionText : "N/A" }
                            StatusRow { label: "RPM:"; value: viewModel ? viewModel.elRpmText : "N/A" }
                            StatusRow { label: "Torque:"; value: viewModel ? viewModel.elTorqueText : "N/A" }
                            StatusRow { label: "Motor:"; value: viewModel ? viewModel.elMotorTempText : "N/A" }
                            StatusRow { label: "Driver:"; value: viewModel ? viewModel.elDriverTempText : "N/A" }

                            Text {
                                text: viewModel && viewModel.elFault ? "⚠ FAULT" : "✓ OK"
                                font.pixelSize: 11
                                font.bold: true
                                font.family: "Segoe UI"
                                color: viewModel && viewModel.elFault ? "#FF6B6B" : accentColor
                                topPadding: 2
                            }
                        }
                    }

                    // --- Servo Actuator ---
                    DeviceCard {
                        title: "Servo Actuator"
                        width: parent.width
                        connected: viewModel ? viewModel.actuatorConnected : false
                        hasError: viewModel ? viewModel.actuatorFault : false
                        accent: accentColor

                        Column {
                            anchors.fill: parent
                            spacing: 3

                            StatusRow { label: "Position:"; value: viewModel ? viewModel.actuatorPositionText : "N/A" }
                            StatusRow { label: "Velocity:"; value: viewModel ? viewModel.actuatorVelocityText : "N/A" }
                            StatusRow { label: "Temp:"; value: viewModel ? viewModel.actuatorTempText : "N/A" }
                            StatusRow { label: "Voltage:"; value: viewModel ? viewModel.actuatorVoltageText : "N/A" }
                            StatusRow { label: "Torque:"; value: viewModel ? viewModel.actuatorTorqueText : "N/A" }

                            Text {
                                text: viewModel && viewModel.actuatorMotorOff ? "⚠ MOTOR OFF" :
                                      viewModel && viewModel.actuatorFault ? "⚠ FAULT" : "✓ OK"
                                font.pixelSize: 11
                                font.bold: true
                                font.family: "Segoe UI"
                                color: (viewModel && (viewModel.actuatorMotorOff || viewModel.actuatorFault)) ? "#FF6B6B" : accentColor
                                topPadding: 2
                            }
                        }
                    }


                }

                // ============================================================
                // COLUMN 2: SENSORS
                // ============================================================
                Column {
                    width: (parent.width - 24) / 3
                    height: parent.height
                    spacing: 10

                    // --- PLC Status ---
                    DeviceCard {
                        title: "Control Systems"
                        width: parent.width
                        connected: viewModel ? (viewModel.plc21Connected && viewModel.plc42Connected) : false
                        hasError: false
                        accent: accentColor

                        Column {
                            anchors.fill: parent
                            spacing: 3

                            StatusRow {
                                label: "PLC21:"
                                value: viewModel && viewModel.plc21Connected ? "Connected" : "Disconnected"
                                valueColor: viewModel && viewModel.plc21Connected ? accentColor : "#FF6B6B"
                            }
                            StatusRow {
                                label: "PLC42:"
                                value: viewModel && viewModel.plc42Connected ? "Connected" : "Disconnected"
                                valueColor: viewModel && viewModel.plc42Connected ? accentColor : "#FF6B6B"
                            }
                            StatusRow {
                                label: "Station:"
                                value: viewModel && viewModel.stationEnabled ? "ENABLED" : "DISABLED"
                                valueColor: viewModel && viewModel.stationEnabled ? accentColor : "#FFA500"
                            }
                            StatusRow {
                                label: "Gun:"
                                value: viewModel && viewModel.gunArmed ? "ARMED" : "SAFE"
                                valueColor: viewModel && viewModel.gunArmed ? "#C81428" : accentColor
                            }
                        }
                    }
                    // --- IMU ---
                    DeviceCard {
                        title: "IMU"
                        width: parent.width
                        connected: viewModel ? viewModel.imuConnected : false
                        hasError: false
                        accent: accentColor

                        Column {
                            anchors.fill: parent
                            spacing: 3

                            StatusRow { label: "Roll:"; value: viewModel ? viewModel.imuRollText : "N/A" }
                            StatusRow { label: "Pitch:"; value: viewModel ? viewModel.imuPitchText : "N/A" }
                            StatusRow { label: "Yaw:"; value: viewModel ? viewModel.imuYawText : "N/A" }
                            StatusRow { label: "Temp:"; value: viewModel ? viewModel.imuTempText : "N/A" }
                        }
                    }

                    // --- LRF ---
                    DeviceCard {
                        title: "Laser Rangefinder"
                        width: parent.width
                        connected: viewModel ? viewModel.lrfConnected : false
                        hasError: viewModel ? viewModel.lrfFault : false
                        accent: accentColor

                        Column {
                            anchors.fill: parent
                            spacing: 3

                            StatusRow { label: "Distance:"; value: viewModel ? viewModel.lrfDistanceText : "N/A" }
                            StatusRow { label: "Temp:"; value: viewModel ? viewModel.lrfTempText : "N/A" }
                            StatusRow { label: "Shots:"; value: viewModel ? viewModel.lrfLaserCountText : "N/A" }

                            Text {
                                text: viewModel ? viewModel.lrfFaultText : "N/A"
                                font.pixelSize: 10
                                font.family: "Segoe UI"
                                color: viewModel && viewModel.lrfFault ? "#FF6B6B" : accentColor
                                topPadding: 2
                                wrapMode: Text.WordWrap
                                width: parent.width
                            }
                        }
                    }


                }

                // ============================================================
                // COLUMN 3: CAMERAS
                // ============================================================
                Column {
                    width: (parent.width - 24) / 3
                    height: parent.height
                    spacing: 10

                    // --- Day Camera ---
                    DeviceCard {
                        title: "Day Camera"
                        width: parent.width
                        connected: viewModel ? viewModel.dayCamConnected : false
                        hasError: viewModel ? viewModel.dayCamError : false
                        accent: accentColor
                        isActive: viewModel ? viewModel.dayCamActive : false

                        Column {
                            anchors.fill: parent
                            spacing: 3

                            StatusRow { label: "FOV:"; value: viewModel ? viewModel.dayCamFovText : "N/A" }
                            StatusRow { label: "Zoom:"; value: viewModel ? viewModel.dayCamZoomText : "N/A" }
                            StatusRow { label: "Focus:"; value: viewModel ? viewModel.dayCamFocusText : "N/A" }
                            StatusRow {
                                label: "AF:"
                                value: viewModel && viewModel.dayCamAutofocus ? "ON" : "OFF"
                                valueColor: viewModel && viewModel.dayCamAutofocus ? accentColor : "#808080"
                            }

                            Text {
                                text: viewModel && viewModel.dayCamError ? "⚠ ERROR" : "✓ OK"
                                font.pixelSize: 11
                                font.bold: true
                                font.family: "Segoe UI"
                                color: viewModel && viewModel.dayCamError ? "#FF6B6B" : accentColor
                                topPadding: 2
                            }
                        }
                    }

                    // --- Night Camera ---
                    DeviceCard {
                        title: "Night Camera"
                        width: parent.width
                        connected: viewModel ? viewModel.nightCamConnected : false
                        hasError: viewModel ? viewModel.nightCamError : false
                        accent: accentColor
                        isActive: viewModel ? viewModel.nightCamActive : false

                        Column {
                            anchors.fill: parent
                            spacing: 3

                            StatusRow { label: "FOV:"; value: viewModel ? viewModel.nightCamFovText : "N/A" }
                            StatusRow { label: "Zoom:"; value: viewModel ? viewModel.nightCamZoomText : "N/A" }
                            StatusRow {
                                label: "FFC:"
                                value: viewModel && viewModel.nightCamFfcInProgress ? "IN PROGRESS" : "IDLE"
                                valueColor: viewModel && viewModel.nightCamFfcInProgress ? "#FFA500" : accentColor
                            }

                            Text {
                                text: viewModel && viewModel.nightCamError ? "⚠ ERROR" : "✓ OK"
                                font.pixelSize: 11
                                font.bold: true
                                font.family: "Segoe UI"
                                color: viewModel && viewModel.nightCamError ? "#FF6B6B" : accentColor
                                topPadding: 2
                            }
                        }
                    }

                    // --- Alarms Section ---
                    Rectangle {
                        width: parent.width
                        height: 140
                        color: Qt.rgba(accentColor.r, accentColor.g, accentColor.b, 0.05)
                        radius: 5
                        border.color: viewModel && viewModel.hasAlarms ? "#C81428" : Qt.rgba(accentColor.r, accentColor.g, accentColor.b, 0.3)
                        border.width: 1

                        Column {
                            anchors.fill: parent
                            anchors.margins: 8
                            spacing: 5

                            Text {
                                text: "⚠ System Alarms"
                                font.pixelSize: 12
                                font.weight: Font.Bold
                                font.family: "Segoe UI"
                                color: viewModel && viewModel.hasAlarms ? "#FF6B6B" : accentColor
                            }

                            ListView {
                                width: parent.width
                                height: parent.height - 50
                                clip: true
                                model: viewModel ? viewModel.alarmsList : []

                                delegate: Text {
                                    text: modelData
                                    font.pixelSize: 10
                                    font.family: "Segoe UI"
                                    color: modelData.startsWith("⚠") ? "#FF6B6B" :
                                           modelData.startsWith("✓") ? accentColor : "#CCCCCC"
                                    wrapMode: Text.WordWrap
                                    width: parent ? parent.width : 100
                                }

                                ScrollBar.vertical: ScrollBar {
                                    policy: ScrollBar.AsNeeded
                                }
                            }

                            /*Button {
                                text: "Clear Alarms"
                                width: parent.width
                                height: 24
                                enabled: viewModel ? viewModel.hasAlarms : false

                                onClicked: {
                                    if (viewModel && viewModel.clearAlarmsRequested)
                                        viewModel.clearAlarmsRequested()
                                }

                                background: Rectangle {
                                    color: parent.enabled ? accentColor : "#333333"
                                    radius: 3
                                }

                                contentItem: Text {
                                    text: parent.text
                                    font.pixelSize: 10
                                    font.bold: true
                                    font.family: "Segoe UI"
                                    color: parent.enabled ? "#0A0A0A" : "#666666"
                                    horizontalAlignment: Text.AlignHCenter
                                    verticalAlignment: Text.AlignVCenter
                                }
                            }*/
                        }
                    }
                }
            }
        }

        // ====================================================================
        // FOOTER SECTION
        // ====================================================================
        Rectangle {
            width: parent.width
            height: 1
            color: "#151515"
        }

        Rectangle {
            width: parent.width
            height: 50
            color: "#0D0D0D"

            Text {
                anchors.centerIn: parent
                text: "Press BACK/MENU to close"
                font.pixelSize: 12
                font.family: "Segoe UI"
                color: "#606060"
                horizontalAlignment: Text.AlignHCenter
            }
        }
    }

    // ========================================================================
    // COMPONENT DEFINITIONS
    // ========================================================================

    component DeviceCard: Rectangle {
        id: card
        property string title: "Device"
        property bool connected: false
        property bool hasError: false
        property bool isActive: false
        property color accent: "#46E2A5"

        height: 140
        color: Qt.rgba(accent.r, accent.g, accent.b, 0.05)
        radius: 5
        border.color: hasError ? "#C81428" : Qt.rgba(accent.r, accent.g, accent.b, 0.3)
        border.width: 1

        default property alias content: contentArea.data

        Column {
            anchors.fill: parent
            spacing: 0

            // Header
            Rectangle {
                width: parent.width
                height: 28
                color: Qt.rgba(accent.r, accent.g, accent.b, 0.15)
                radius: 5

                Rectangle {
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.bottom: parent.bottom
                    height: parent.height / 2
                    color: parent.color
                }

                Row {
                    anchors.fill: parent
                    anchors.leftMargin: 8
                    anchors.rightMargin: 8
                    spacing: 6

                    // Connection indicator
                    Rectangle {
                        width: 8
                        height: 8
                        radius: 4
                        color: connected ? accent : "#606060"
                        anchors.verticalCenter: parent.verticalCenter
                    }

                    Text {
                        text: card.title
                        font.pixelSize: 11
                        font.weight: Font.Bold
                        font.family: "Segoe UI"
                        color: "#FFFFFF"
                        anchors.verticalCenter: parent.verticalCenter
                    }

                    //Item { Layout.fillWidth: true; width: 1 }

                    // Active indicator
                    Text {
                        text: isActive ? "[ACTIVE]" : ""
                        font.pixelSize: 9
                        font.weight: Font.Bold
                        font.family: "Segoe UI"
                        color: accent
                        anchors.verticalCenter: parent.verticalCenter
                        visible: isActive
                    }
                }
            }

            // Content area
            Item {
                id: contentArea
                width: parent.width
                height: parent.height - 28

                anchors.margins: 8
                anchors.topMargin: 6
                anchors.leftMargin: 10
                anchors.rightMargin: 10
            }
        }
    }

    component StatusRow: Row {
        property string label: "Label:"
        property string value: "Value"
        property color valueColor: "#CCCCCC"

        spacing: 5
        height: 16

        Text {
            text: label
            font.pixelSize: 10
            font.family: "Segoe UI"
            color: "#808080"
            width: 55
        }

        Text {
            text: value
            font.pixelSize: 10
            font.bold: true
            font.family: "Segoe UI"
            color: valueColor
        }
    }
}
