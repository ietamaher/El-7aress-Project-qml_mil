import QtQuick
import QtQuick.Shapes // Required for complex paths and shapes

// OsdOverlay.qml
Item {
    id: osdRoot

    // 1. OSD Data Model Reference (The link to C++)
    property var osdViewModel: null // Public property to receive the C++ model
    property color osdColor: "green"

    // --- Configuration Constants (matching OsdRenderer's internal constants) ---
    readonly property real trackingCornerLength: 15.0
    readonly property real reticleLineWidth: 2.0
    readonly property real screenCenterX: width / 2.0
    readonly property real screenCenterY: height / 2.0

    // --- Text Statuses ---
    Text {
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.margins: 25
        text: osdViewModel ? osdViewModel.modeText : "" // Bind to C++ property
        color: osdRoot.osdColor
        font.pointSize: 16
    }

    // --- Tracking Box (Dynamic element based on QRectF data) ---
    // This replaces C++ logic in OsdRenderer::updateTrackingCorners and drawDetectionBox.
    Item {
        id: trackingBoxItem
        visible: osdViewModel ? osdViewModel.trackingBox.width > 0 : false

        // Bind position/size from C++ QRectF property (or directly from trackingRect's members)
        x: osdViewModel ? osdViewModel.trackingBox.x : 0
        y: osdViewModel ? osdViewModel.trackingBox.y : 0
        width: osdViewModel ? osdViewModel.trackingBox.width : 0
        height: osdViewModel ? osdViewModel.trackingBox.height : 0

        // Create the four corners (using Repeater for efficiency)
        Repeater {
            model: 4 // Four corners
            delegate: Rectangle {
                // Define individual corner position based on index (i)
                width: 2 // Line thickness
                height: trackingCornerLength

                // Top-left corner (index 0)
                // Top-right corner (index 1)
                // Bottom-left corner (index 2)
                // Bottom-right corner (index 3)
                // ... logic to position corners based on index (model.index) ...
                // Example: Top-left corner
                // x: model.index == 0 ? 0 : (model.index == 1 ? parent.width - width : ...)
                // ... for simplicity here, we'll draw a full box ...

                // QML Best Practice: Use a Shape for complex, non-rectangular drawings like corners.
            }
        }

        // --- Simpler demonstration of Tracking Box (Full Box instead of corners) ---
        // Let's create a simpler, full-box version first.
        Rectangle {
            anchors.fill: parent
            color: "transparent"
            border.color: osdRoot.osdColor
            border.width: 2
        }
    }

    // --- Reticle Group (Dynamic position based on zeroing/lead offset) ---
    // This replaces C++ logic in OsdRenderer::applyReticlePosition.
    Item {
        id: reticleGroup
        // Position relative to screen center + C++ offset calculation
        x: screenCenterX + (osdViewModel ? osdViewModel.reticleOffsetPx.x : 0)
        y: screenCenterY + (osdViewModel ? osdViewModel.reticleOffsetPx.y : 0)

        // The reticle itself (e.g., a simple crosshair)
        // This replaces C++ logic in OsdRenderer::createStandardCrosshairReticle.
        Rectangle {
            width: 50; height: reticleLineWidth
            anchors.verticalCenter: parent.verticalCenter
            anchors.horizontalCenter: parent.horizontalCenter
            color: osdRoot.osdColor
        }
        Rectangle {
            width: reticleLineWidth; height: 50
            anchors.verticalCenter: parent.verticalCenter
            anchors.horizontalCenter: parent.horizontalCenter
            color: osdRoot.osdColor
        }
    }

    // --- Azimuth Indicator (Dynamic rotation based on C++ data) ---
    // This replaces C++ logic in OsdRenderer::updateAzimuthIndicator.
    Item {
        id: azimuthIndicatorRoot
        x: osdRoot.width - 75 // Position from right edge (matching C++ constant)
        y: 75

        // The Azimuth Needle itself (rotates based on C++ property)
        Rectangle {
            id: needle
            anchors.centerIn: parent
            width: 2 // Line thickness
            height: 50 // Needle length
            color: osdRoot.osdColor
            transformOrigin: Item.Center // Rotate around center point
            // Bind QML rotation to C++ azimuth property.
            rotation: osdViewModel ? osdViewModel.azimuth : 0
        }
        // Azimuth Text (bind to C++ property, format in QML)
        Text {
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: needle.bottom
            anchors.topMargin: 10
            text: osdViewModel ? osdViewModel.azimuth.toFixed(1) + "°" : ""
            color: osdRoot.osdColor
            font.pointSize: 12
        }
    }
}
