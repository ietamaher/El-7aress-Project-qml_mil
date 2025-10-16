import QtQuick
import QtQuick.Shapes

Item {
    id: osdRoot
    anchors.fill: parent

    property var viewModel: osdViewModel // Context property from C++
    property color accentColor: viewModel ? viewModel.accentColor : "#46E2A5"

    // Constants
    readonly property real azIndicatorRadius: 50
    readonly property real elScaleHeight: 120

    // ========================================================================
    // TEXT OVERLAYS (Top-Left Corner)
    // ========================================================================
    Column {
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.margins: 10
        spacing: 5

        // Mode
        Text {
            text: viewModel ? viewModel.modeText : "MODE: IDLE"
            font.pixelSize: 16
            font.bold: true
            font.family: "Archivo Narrow"
            color: osdRoot.accentColor
        }

        // Motion Mode
        Text {
            text: viewModel ? viewModel.motionText : "MOTION: MAN"
            font.pixelSize: 14
            font.family: "Archivo Narrow"
            color: osdRoot.accentColor
        }

        // System Status
        Text {
            text: viewModel ? viewModel.statusText : "SYS: --- SAF NRD"
            font.pixelSize: 14
            font.family: "Archivo Narrow"
            color: osdRoot.accentColor
        }

        // Rate
        Text {
            text: viewModel ? viewModel.rateText : "RATE: SINGLE SHOT"
            font.pixelSize: 14
            font.family: "Archivo Narrow"
            color: osdRoot.accentColor
        }

        // LRF Distance
        Text {
            text: viewModel ? viewModel.lrfText : "LRF: --- m"
            font.pixelSize: 14
            font.family: "Archivo Narrow"
            color: osdRoot.accentColor
        }

        // Zeroing Status
        Text {
            visible: viewModel ? viewModel.zeroingVisible : false
            text: viewModel ? viewModel.zeroingText : ""
            font.pixelSize: 14
            font.family: "Archivo Narrow"
            color: osdRoot.accentColor
        }

        // Windage Status
        Text {
            visible: viewModel ? viewModel.windageVisible : false
            text: viewModel ? viewModel.windageText : ""
            font.pixelSize: 14
            font.family: "Archivo Narrow"
            color: osdRoot.accentColor
        }

        // Lead Angle Status
        Text {
            visible: viewModel ? viewModel.leadAngleVisible : false
            text: viewModel ? viewModel.leadAngleText : ""
            font.pixelSize: 14
            font.family: "Archivo Narrow"
            color: viewModel && viewModel.leadAngleText.includes("LAG") ? "yellow" :
                   (viewModel && viewModel.leadAngleText.includes("ZOOM") ? "#C81428" : osdRoot.accentColor)
        }

        // Scan Name
        Text {
            visible: viewModel ? viewModel.scanNameVisible : false
            text: viewModel ? viewModel.scanNameText : ""
            font.pixelSize: 14
            font.family: "Archivo Narrow"
            color: osdRoot.accentColor
        }
    }

    // ========================================================================
    // TOP-RIGHT CORNER (Speed)
    // ========================================================================
    Text {
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.margins: 10
        text: viewModel ? viewModel.speedText : "SPD: 0.0%"
        font.pixelSize: 16
        font.bold: true
        font.family: "Archivo Narrow"
        color: osdRoot.accentColor
    }

    // ========================================================================
    // BOTTOM-LEFT CORNER (Stab, Camera, FOV)
    // ========================================================================
    Row {
        anchors.left: parent.left
        anchors.bottom: parent.bottom
        anchors.margins: 10
        spacing: 20

        Text {
            text: viewModel ? viewModel.stabText : "STAB: OFF"
            font.pixelSize: 14
            font.family: "Archivo Narrow"
            color: osdRoot.accentColor
        }

        Text {
            text: viewModel ? viewModel.cameraText : "CAM: DAY"
            font.pixelSize: 14
            font.family: "Archivo Narrow"
            color: osdRoot.accentColor
        }

        Text {
            text: viewModel ? viewModel.fovText : "FOV: 45.0°"
            font.pixelSize: 14
            font.family: "Archivo Narrow"
            color: osdRoot.accentColor
        }
    }

    // ========================================================================
    // CENTER WARNING (Zone Warnings)
    // ========================================================================
    Text {
        anchors.centerIn: parent
        anchors.verticalCenterOffset: 50
        visible: viewModel ? viewModel.zoneWarningVisible : false
        text: viewModel ? viewModel.zoneWarningText : ""
        font.pixelSize: 24
        font.bold: true
        font.family: "Archivo Narrow"
        color: "#C81428" // Red for warnings

        // Background for better visibility
        Rectangle {
            anchors.fill: parent
            anchors.margins: -5
            color: "black"
            opacity: 0.7
            radius: 3
            z: -1
        }
    }

    // ========================================================================
    // AZIMUTH INDICATOR (Top-Right)
    // ========================================================================
    AzimuthIndicator {
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.margins: 75
        width: azIndicatorRadius * 2
        height: azIndicatorRadius * 2

        azimuth: viewModel ? viewModel.azimuth : 0
        color: osdRoot.accentColor
    }

    // ========================================================================
    // ELEVATION SCALE (Right Side)
    // ========================================================================
    ElevationScale {
        anchors.right: parent.right
        anchors.verticalCenter: parent.verticalCenter
        anchors.rightMargin: 55
        height: elScaleHeight

        elevation: viewModel ? viewModel.elevation : 0
        color: osdRoot.accentColor
    }

    // ========================================================================
    // TRACKING BOX
    // ========================================================================
    TrackingBox {
        visible: viewModel ? viewModel.trackingBoxVisible : false
        x: viewModel ? viewModel.trackingBox.x : 0
        y: viewModel ? viewModel.trackingBox.y : 0
        width: viewModel ? viewModel.trackingBox.width : 0
        height: viewModel ? viewModel.trackingBox.height : 0

        boxColor: viewModel ? viewModel.trackingBoxColor : "yellow"
        dashed: viewModel ? viewModel.trackingBoxDashed : false
    }

    // ========================================================================
    // ACQUISITION BOX
    // ========================================================================
    Rectangle {
        visible: viewModel ? viewModel.acquisitionBoxVisible : false
        x: viewModel ? viewModel.acquisitionBox.x : 0
        y: viewModel ? viewModel.acquisitionBox.y : 0
        width: viewModel ? viewModel.acquisitionBox.width : 0
        height: viewModel ? viewModel.acquisitionBox.height : 0

        color: "transparent"
        border.color: "yellow"
        border.width: 2
    }

    // ========================================================================
    // RETICLE (Center with offset)
    // ========================================================================
    ReticleRenderer {
        id: reticle
        anchors.centerIn: parent
        x: parent.width / 2 + (viewModel ? viewModel.reticleOffsetX : 0)
        y: parent.height / 2 + (viewModel ? viewModel.reticleOffsetY : 0)

        reticleType: viewModel ? viewModel.reticleType : 1 // 1 = BoxCrosshair
        color: osdRoot.accentColor
        currentFov: viewModel ? viewModel.currentFov : 45.0
    }

    // ========================================================================
    // FIXED LOB MARKER (Screen Center - Always visible)
    // ========================================================================
    Item {
        anchors.centerIn: parent
        width: 10
        height: 10

        // Simple cross marker
        Rectangle {
            width: 10
            height: 2
            anchors.centerIn: parent
            color: osdRoot.accentColor
        }
        Rectangle {
            width: 2
            height: 10
            anchors.centerIn: parent
            color: osdRoot.accentColor
        }
    }
}
