// ZoneDefinitionOverlay.qml
import QtQuick
import QtQuick.Controls
import QtQuick.Effects

Rectangle {
    id: root

    width: 600
    height: 550
    radius: 5
    color: Qt.rgba(0, 0, 0, 0.85)
    border.color: "#4ECDC4"
    border.width: 2

    visible: zoneDefViewModel ? zoneDefViewModel.visible : false

    property color accentColor: "#4ECDC4"

    // Drop shadow
    layer.enabled: true
    layer.effect: MultiEffect {
        shadowEnabled: true
        shadowColor: "#80000000"
        shadowBlur: 0.4
        shadowVerticalOffset: 8
    }

    Column {
        anchors.fill: parent
        anchors.margins: 20
        spacing: 15

        // Title
        Text {
            text: zoneDefViewModel ? zoneDefViewModel.title : "Zone Definition"
            font.pixelSize: 22
            font.bold: true
            color: accentColor
            horizontalAlignment: Text.AlignHCenter
            anchors.horizontalCenter: parent.horizontalCenter
        }

        // Instructions
        Text {
            width: parent.width
            text: zoneDefViewModel ? zoneDefViewModel.instruction : ""
            font.pixelSize: 14
            color: "#CCCCCC"
            wrapMode: Text.WordWrap
            horizontalAlignment: Text.AlignHCenter
        }

        // Gimbal Position Display
        Row {
            anchors.horizontalCenter: parent.horizontalCenter
            spacing: 30
            visible: zoneDefViewModel ? zoneDefViewModel.showMap : false

            Text {
                text: "WS Pos:"
                font.pixelSize: 12
                color: "#AAAAAA"
            }
            Text {
                text: zoneDefViewModel ?
                      "Az: " + zoneDefViewModel.gimbalAz.toFixed(1) + "°" : "Az: ---"
                font.pixelSize: 12
                font.bold: true
                color: accentColor
            }
            Text {
                text: zoneDefViewModel ?
                      "El: " + zoneDefViewModel.gimbalEl.toFixed(1) + "°" : "El: ---"
                font.pixelSize: 12
                font.bold: true
                color: accentColor
            }
        }

        Rectangle {
            width: parent.width
            height: 1
            color: "#505050"
            opacity: 0.6
        }

        // Main Menu List
        ListView {
            id: mainMenuList
            width: parent.width
            height: 250
            spacing: 6
            clip: true
            visible: zoneDefViewModel ? zoneDefViewModel.showMainMenu : false

            model: zoneDefViewModel ? zoneDefViewModel.menuOptions : []
            currentIndex: zoneDefViewModel ? zoneDefViewModel.currentIndex : 0

            delegate: ItemDelegate {
                width: mainMenuList.width
                height: 40
                text: modelData

                background: Rectangle {
                    radius: 3
                    color: ListView.isCurrentItem ? accentColor : Qt.rgba(1, 1, 1, 0.08)
                    border.color: ListView.isCurrentItem ? "#FFFFFF" : "transparent"
                    border.width: ListView.isCurrentItem ? 2 : 0
                    Behavior on color { ColorAnimation { duration: 200 } }
                }

                contentItem: Text {
                    text: modelData
                    color: ListView.isCurrentItem ? "black" : "white"
                    font.pixelSize: 16
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignHCenter
                }

                highlighted: ListView.isCurrentItem
            }

            ScrollBar.vertical: ScrollBar {
                policy: ScrollBar.AsNeeded
                active: true
            }
        }

        // Zone Selection List (for modify/delete)
        ListView {
            id: zoneSelectionList
            width: parent.width
            height: 250
            spacing: 6
            clip: true
            visible: zoneDefViewModel ? zoneDefViewModel.showZoneSelectionList : false

            model: zoneDefViewModel ? zoneDefViewModel.menuOptions : []
            currentIndex: zoneDefViewModel ? zoneDefViewModel.currentIndex : 0

            delegate: ItemDelegate {
                width: zoneSelectionList.width
                height: 40
                text: modelData

                background: Rectangle {
                    radius: 3
                    color: ListView.isCurrentItem ? accentColor : Qt.rgba(1, 1, 1, 0.08)
                    border.color: ListView.isCurrentItem ? "#FFFFFF" : "transparent"
                    border.width: ListView.isCurrentItem ? 2 : 0
                }

                contentItem: Text {
                    text: modelData
                    color: ListView.isCurrentItem ? "black" : "white"
                    font.pixelSize: 14
                    verticalAlignment: Text.AlignVCenter
                }

                highlighted: ListView.isCurrentItem
            }
        }

        // Zone Map Canvas
        ZoneMapCanvas {
            id: zoneMap
            width: parent.width
            height: 200
            visible: zoneDefViewModel ? zoneDefViewModel.showMap : false
            viewModel: zoneMapViewModel
        }

        // Parameter Panel Stack
        Loader {
            id: parameterPanelLoader
            width: parent.width
            height: 180
            visible: zoneDefViewModel ? zoneDefViewModel.showParameterPanel : false

            sourceComponent: {
                if (!zoneDefViewModel) return null

                switch (zoneDefViewModel.activePanelType) {
                    case 1: return areaZonePanelComponent  // AreaZone
                    case 2: return sectorScanPanelComponent // SectorScan
                    case 3: return trpPanelComponent        // TRP
                    default: return null
                }
            }
        }
    }

    // Panel Components
    Component {
        id: areaZonePanelComponent
        AreaZoneParameterPanel {
            viewModel: areaZoneParamViewModel
        }
    }

    Component {
        id: sectorScanPanelComponent
        SectorScanParameterPanel {
            viewModel: sectorScanParamViewModel
        }
    }

    Component {
        id: trpPanelComponent
        TRPParameterPanel {
            viewModel: trpParamViewModel
        }
    }
}
