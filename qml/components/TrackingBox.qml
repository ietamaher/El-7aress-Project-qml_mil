import QtQuick

Item {
    id: root

    property color boxColor: "yellow"
    property bool dashed: false
    readonly property real cornerLength: 15

    // Eight corner lines (2 per corner)
    Repeater {
        model: 8

        delegate: Rectangle {
            property bool isHorizontal: index % 2 === 0
            property int corner: Math.floor(index / 2) // 0=TL, 1=TR, 2=BL, 3=BR

            width: isHorizontal ? cornerLength : 2
            height: isHorizontal ? 2 : cornerLength
            color: root.boxColor

            x: {
                if (corner === 0 || corner === 2) return 0; // Left corners
                if (isHorizontal) return root.width - cornerLength; // Right horizontal
                return root.width - 2; // Right vertical
            }

            y: {
                if (corner === 0 || corner === 1) return 0; // Top corners
                if (isHorizontal) return root.height - 2; // Bottom horizontal
                return root.height - cornerLength; // Bottom vertical
            }

            // Dashed effect (simple)
            opacity: root.dashed ? 0.8 : 1.0
        }
    }

    // Optional: Full box outline (if not using corners only)
    /*
    Rectangle {
        anchors.fill: parent
        color: "transparent"
        border.color: root.boxColor
        border.width: 2

        // Dashed border effect
        Rectangle {
            anchors.fill: parent
            color: "transparent"
            border.color: root.boxColor
            border.width: 2
            visible: root.dashed

            // Simple dash simulation with opacity
            opacity: 0.6
        }
    }
    */
}
