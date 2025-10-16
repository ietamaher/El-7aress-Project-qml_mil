import QtQuick
import QtQuick.Controls
import "qrc:/qml/components"
import "qrc:/qml/views"
import "../components"

ApplicationWindow {
    id: appWindow
    width: 1024
    height: 768
    visible: true
    title: "V2 QML OSD Application"

    // --- Video Feed Background ---
    Image {
        id: videoDisplay
        anchors.fill: parent
        fillMode: Image.PreserveAspectFit // Fill screen with video, keeping aspect ratio
        source: "image://video/camera"

        // Update the image source (as per previous solution)
        Timer {
            interval: 33 // ~30 FPS refresh rate
            running: true
            repeat: true
            onTriggered: {
                // To force refresh from QQuickImageProvider, change source (e.g., add timestamp)
                videoDisplay.source = "image://video/camera" + "?" + Date.now()
            }
        }
    }

    // --- OSD Layer (New QML Component) ---
    // This is where all your OSD elements will be drawn.
    OsdOverlay {
        anchors.fill: parent
        // Pass the C++ model to the overlay component
        osdViewModel: osdViewModelInstance
    }

    // === MAIN MENU ===
    MainMenu {
        id: mainMenu
        anchors.centerIn: parent
        viewModel: mainMenuViewModel
        osdViewModel: osdViewModelInstance
        z: 100
    }

    // === RETICLE MENU ===
    MainMenu {
        id: reticleMenu
        anchors.centerIn: parent
        viewModel: reticleMenuViewModel
        osdViewModel: osdViewModelInstance
        z: 100
    }

    // === COLOR MENU ===
    MainMenu {
        id: colorMenu
        anchors.centerIn: parent
        viewModel: colorMenuViewModel
        osdViewModel: osdViewModelInstance
        z: 100
    }

    // === ZEROING OVERLAY ===
    ZeroingOverlay {
        id: zeroingOverlay
        anchors.centerIn: parent
        z: 100
    }

    // === WINDAGE OVERLAY ===
    WindageOverlay {
        id: windageOverlay
        anchors.centerIn: parent
        z: 100
    }

    ZoneDefinitionOverlay {
        id: zoneDefinitionOverlay
        anchors.fill: parent
        z: 100  // Ensure it's on top of other overlays

        // ViewModels are accessible via context properties
        // Already bound in ZoneDefinitionOverlay.qml
    }

    // Physical Button Handlers - ONLY 3 BUTTONS
    Row {
        anchors.bottom: parent.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottomMargin: 20
        spacing: 15
        z: 200

        Button {
            text: "UP"
            width: 80
            height: 40
            onClicked: {
                console.log("[UI] UP button clicked")
                appController.onUpButtonPressed()
            }
        }

        Button {
            text: "MENU/VAL"
            width: 100
            height: 40
            highlighted: true  // Make it stand out as the primary button
            onClicked: {
                console.log("[UI] MENU/VAL button clicked")
                appController.onMenuValButtonPressed()
            }
        }

        Button {
            text: "DOWN"
            width: 80
            height: 40
            onClicked: {
                console.log("[UI] DOWN button clicked")
                appController.onDownButtonPressed()
            }
        }
    }
}
