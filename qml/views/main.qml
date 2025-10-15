import QtQuick
import QtQuick.Controls
import "qrc:/qml/components"
import "qrc:/qml/views"

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

    MainMenu {
        // This is where your errors were. These properties now exist in Menu.qml
        viewModel: mainMenuViewModel
        //osdColor: osdViewModelInstance.osdColor
        id: menu

        x: 50
        y: 150
    }
    Row {
        anchors.bottom: parent.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.margins: 20
        spacing: 15

        Button {
            text: "UP"
            onClicked: {
                // Call the C++ slot on the appController instance we exposed
                console.log("[UI] UP button clicked")
                appController.onUpButtonPressed()
            }
        }

        Button {
            text: "DOWN"
            onClicked: {
                console.log("[UI] DOWN button clicked")
                appController.onDownButtonPressed()
            }
        }

        Button {
            text: "MENU / SELECT"
            onClicked: {
                console.log("[UI] MENU button clicked")
                appController.onMenuButtonPressed()
            }
        }
        Button {
            text: "SELECT"
            onClicked: {
                console.log("[UI] MENU button clicked")
                appController.onSelectButtonPressed()
            }
        }
    }
}
