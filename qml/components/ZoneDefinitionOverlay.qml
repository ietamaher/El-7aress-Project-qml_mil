import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    id: root

    // Get ViewModels from context
    property var viewModel: zoneDefinitionViewModel
    property var mapViewModel: zoneMapViewModel
    property color accentColor: viewModel ? viewModel.accentColor : Qt.rgba(70, 226, 165, 1.0)

    visible: viewModel ? viewModel.visible : false
    color: Qt.rgba(0, 0, 0, 0.59) // Semi-transparent background
    anchors.fill: parent

    // Main content container
    Rectangle {
        id: contentContainer
        anchors.centerIn: parent
        width: Math.min(parent.width - 20, 800)
        height: Math.min(parent.height - 20, 700)
        color: Qt.rgba(0, 0, 0, 0.9)
        border.color: accentColor
        border.width: 2
        radius: 5



        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 10
            spacing: 10

            // Title
            Text {
                Layout.fillWidth: true
                text: viewModel ? viewModel.title : ""
                font.pixelSize: 18
                font.bold: true
                color: accentColor
                horizontalAlignment: Text.AlignHCenter
            }

            // Instruction text
            Text {
                Layout.fillWidth: true
                text: viewModel ? viewModel.instruction : ""
                font.pixelSize: 12
                color: accentColor
                horizontalAlignment: Text.AlignHCenter
                wrapMode: Text.WordWrap
            }

            // Gimbal position display
            RowLayout {
                Layout.fillWidth: true
                Layout.preferredHeight: 25
                spacing: 20

                Text {
                    text: "WS Pos:"
                    font.pixelSize: 11
                    color: accentColor
                }

                Text {
                    text: viewModel ? "Az: " + viewModel.gimbalAz.toFixed(1) + "°" : "Az: ---°"
                    font.pixelSize: 11
                    color: accentColor
                }

                Item { Layout.fillWidth: true }

                Text {
                    text: viewModel ? "El: " + viewModel.gimbalEl.toFixed(1) + "°" : "El: ---°"
                    font.pixelSize: 11
                    color: accentColor
                }
            }

            // Main content area (menu OR map)
            Item {
                Layout.fillWidth: true
                Layout.fillHeight: true

                // Main Menu List
                ListView {
                    id: mainMenuList
                    anchors.fill: parent
                    visible: viewModel ? viewModel.showMainMenu : false
                    model: viewModel ? viewModel.menuOptions : []
                    currentIndex: viewModel ? viewModel.currentIndex : 0
                    clip: true

                    delegate: Rectangle {
                        width: mainMenuList.width
                        height: 35
                        color: index === mainMenuList.currentIndex ?
                               accentColor : "transparent"
                        border.color: accentColor
                        border.width: 1

                        Text {
                            anchors.centerIn: parent
                            text: modelData
                            font.pixelSize: 13
                            font.family: "Archivo Narrow"
                            font.weight: Font.DemiBold
                            color: index === mainMenuList.currentIndex ?
                                   "black" : accentColor
                        }
                    }
                }

                // Zone Selection List
                ListView {
                    id: zoneSelectionList
                    anchors.fill: parent
                    visible: viewModel ? viewModel.showZoneSelectionList : false
                    model: viewModel ? viewModel.menuOptions : []
                    currentIndex: viewModel ? viewModel.currentIndex : 0
                    clip: true

                    delegate: Rectangle {
                        width: zoneSelectionList.width
                        height: 35
                        color: index === zoneSelectionList.currentIndex ?
                               accentColor : "transparent"
                        border.color: accentColor
                        border.width: 1

                        Text {
                            anchors.centerIn: parent
                            text: modelData
                            font.pixelSize: 12
                            font.family: "Archivo Narrow"
                            font.weight: Font.DemiBold
                            color: index === zoneSelectionList.currentIndex ?
                                   "black" : accentColor
                        }
                    }
                }

                // Confirm Dialog
                ListView {
                    id: confirmList
                    anchors.centerIn: parent
                    width: parent.width * 0.5
                    height: 80
                    visible: viewModel ? viewModel.showConfirmDialog : false
                    model: viewModel ? viewModel.menuOptions : []
                    currentIndex: viewModel ? viewModel.currentIndex : 0
                    clip: true
                    spacing: 10
                    orientation: ListView.Horizontal

                    delegate: Rectangle {
                        width: 120
                        height: 50
                        color: index === confirmList.currentIndex ?
                               accentColor : "transparent"
                        border.color: accentColor
                        border.width: 2
                        radius: 3

                        Text {
                            anchors.centerIn: parent
                            text: modelData
                            font.pixelSize: 14
                            font.bold: true
                            color: index === confirmList.currentIndex ?
                                   "black" : accentColor
                        }
                    }
                }
            }

            // Zone Map
            ZoneMapCanvas {
                id: zoneMap
                Layout.fillWidth: true
                Layout.preferredHeight: 250
                visible: viewModel ? viewModel.showMap : true
                viewModel: mapViewModel
            }

            // Parameter Panel Container
            Loader {
                id: parameterPanelLoader
                Layout.fillWidth: true
                Layout.preferredHeight: 200
                visible: viewModel ? viewModel.showParameterPanel : false

                sourceComponent: {
                    if (!viewModel || !viewModel.showParameterPanel) return null

                    switch (viewModel.activePanelType) {
                        case 1: return areaZonePanelComponent
                        case 2: return sectorScanPanelComponent
                        case 3: return trpPanelComponent
                        default: return null
                    }
                }
            }
        }
    }

    // Parameter Panel Components
    Component {
        id: areaZonePanelComponent
        AreaZoneParameterPanel {
            viewModel: areaZoneParameterViewModel
        }
    }

    Component {
        id: sectorScanPanelComponent
        SectorScanParameterPanel {
            viewModel: sectorScanParameterViewModel
        }
    }

    Component {
        id: trpPanelComponent
        TRPParameterPanel {
            viewModel: trpParameterViewModel
        }
    }
}
