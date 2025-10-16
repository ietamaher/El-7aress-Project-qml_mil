// ZoneMapCanvas.qml
import QtQuick
import QtQuick.Shapes

Canvas {
    id: canvas

    property var viewModel: null

    // Redraw when data changes
    onViewModelChanged: requestPaint()

    Connections {
        target: viewModel
        function onGimbalAzChanged() { canvas.requestPaint() }
        function onGimbalElChanged() { canvas.requestPaint() }
        function onAreaZonesChanged() { canvas.requestPaint() }
        function onSectorScansChanged() { canvas.requestPaint() }
        function onTrpsChanged() { canvas.requestPaint() }
        function onWipZoneChanged() { canvas.requestPaint() }
    }

    onPaint: {
        if (!viewModel) return

        var ctx = getContext("2d")
        ctx.reset()

        // Background
        ctx.fillStyle = "#282828"
        ctx.fillRect(0, 0, width, height)

        // Grid
        drawGrid(ctx)
        drawAxesLabels(ctx)

        // Saved zones
        drawAreaZones(ctx, viewModel.areaZones)
        drawSectorScans(ctx, viewModel.sectorScans)
        drawTRPs(ctx, viewModel.trps)

        // WIP zone
        if (viewModel.hasWipZone) {
            drawWipZone(ctx)
        }

        // Gimbal indicator (on top)
        drawGimbalIndicator(ctx)
    }

    function drawGrid(ctx) {
        ctx.strokeStyle = "#505050"
        ctx.lineWidth = 1

        // Azimuth grid lines (every 30°)
        for (var az = 0; az <= 360; az += 30) {
            var x = az / 360.0 * width
            ctx.beginPath()
            ctx.moveTo(x, 0)
            ctx.lineTo(x, height)
            ctx.stroke()
        }

        // Elevation grid lines (every 10°)
        var elRange = 90 - (-20)  // EL_MAX - EL_MIN
        for (var el = -20; el <= 90; el += 10) {
            var y = height - ((el - (-20)) / elRange * height)
            ctx.beginPath()
            ctx.moveTo(0, y)
            ctx.lineTo(width, y)
            ctx.stroke()
        }
    }

    function drawAxesLabels(ctx) {
        ctx.fillStyle = "white"
        ctx.font = "10px sans-serif"

        // Azimuth labels
        for (var az = 0; az <= 360; az += 60) {
            var x = az / 360.0 * width
            ctx.fillText(az + "°", x - 10, height - 5)
        }

        // Elevation labels
        var elRange = 90 - (-20)
        for (var el = -20; el <= 90; el += 20) {
            var y = height - ((el - (-20)) / elRange * height)
            ctx.fillText(el + "°", 5, y + 5)
        }

        // Axis titles
        ctx.fillText("Azimuth (0-360°)", width / 2 - 40, height - 20)
        ctx.save()
        ctx.translate(15, height / 2)
        ctx.rotate(-Math.PI / 2)
        ctx.fillText("Elevation", -25, 0)
        ctx.restore()
    }

    function drawGimbalIndicator(ctx) {
        if (!viewModel) return

        var pos = viewModel.azElToPixel(
            viewModel.gimbalAz,
            viewModel.gimbalEl,
            width,
            height
        )

        ctx.strokeStyle = "yellow"
        ctx.lineWidth = 2
        ctx.fillStyle = "yellow"

        // Crosshair
        ctx.beginPath()
        ctx.moveTo(pos.x - 10, pos.y)
        ctx.lineTo(pos.x + 10, pos.y)
        ctx.moveTo(pos.x, pos.y - 10)
        ctx.lineTo(pos.x, pos.y + 10)
        ctx.stroke()

        // Center dot
        ctx.beginPath()
        ctx.arc(pos.x, pos.y, 3, 0, 2 * Math.PI)
        ctx.fill()
    }

    function drawAreaZones(ctx, zones) {
        for (var i = 0; i < zones.length; i++) {
            var zone = zones[i]
            if (!zone.isEnabled) continue

            var color = getZoneColor(zone.type)
            var isHighlighted = (zone.id === viewModel.highlightedZoneId)

            drawZoneRect(ctx, zone, color, isHighlighted)
        }
    }

    function drawZoneRect(ctx, zone, color, isHighlighted) {
        var topLeft = viewModel.azElToPixel(zone.startAzimuth, zone.maxElevation, width, height)
        var bottomRight = viewModel.azElToPixel(zone.endAzimuth, zone.minElevation, width, height)

        var rectWidth = bottomRight.x - topLeft.x
        var rectHeight = bottomRight.y - topLeft.y

        // Fill
        ctx.fillStyle = color + "33"  // Add alpha
        ctx.fillRect(topLeft.x, topLeft.y, rectWidth, rectHeight)

        // Border
        ctx.strokeStyle = color
        ctx.lineWidth = isHighlighted ? 3 : 2
        ctx.setLineDash(zone.isOverridable ? [5, 5] : [])
        ctx.strokeRect(topLeft.x, topLeft.y, rectWidth, rectHeight)
        ctx.setLineDash([])

        // ID label
        ctx.fillStyle = "white"
        ctx.font = "10px sans-serif"
        ctx.fillText("ID:" + zone.id, topLeft.x + 5, topLeft.y + 15)
    }

    function drawSectorScans(ctx, scans) {
        for (var i = 0; i < scans.length; i++) {
            var scan = scans[i]
            if (!scan.isEnabled) continue

            var p1 = viewModel.azElToPixel(scan.az1, scan.el1, width, height)
            var p2 = viewModel.azElToPixel(scan.az2, scan.el2, width, height)

            ctx.strokeStyle = "#4A90E2"
            ctx.lineWidth = 2
            ctx.beginPath()
            ctx.moveTo(p1.x, p1.y)
            ctx.lineTo(p2.x, p2.y)
            ctx.stroke()

            // Endpoints
            ctx.fillStyle = "#4A90E2"
            ctx.beginPath()
            ctx.arc(p1.x, p1.y, 3, 0, 2 * Math.PI)
            ctx.arc(p2.x, p2.y, 3, 0, 2 * Math.PI)
            ctx.fill()
        }
    }

    function drawTRPs(ctx, trps) {
        for (var i = 0; i < trps.length; i++) {
            var trp = trps[i]
            var pos = viewModel.azElToPixel(trp.azimuth, trp.elevation, width, height)

            ctx.strokeStyle = "yellow"
            ctx.lineWidth = 2

            // Cross marker
            ctx.beginPath()
            ctx.moveTo(pos.x - 6, pos.y)
            ctx.lineTo(pos.x + 6, pos.y)
            ctx.moveTo(pos.x, pos.y - 6)
            ctx.lineTo(pos.x, pos.y + 6)
            ctx.stroke()

            // ID label
            ctx.fillStyle = "white"
            ctx.font = "10px sans-serif"
            ctx.fillText("ID:" + trp.id, pos.x + 8, pos.y - 8)
        }
    }

    function drawWipZone(ctx) {
        var wip = viewModel.wipZone
        var type = viewModel.wipZoneType

        ctx.strokeStyle = "#00FF99"  // Green for WIP
        ctx.lineWidth = 2
        ctx.setLineDash([5, 5])

        if (type === 1) {  // AreaZone
            var topLeft = viewModel.azElToPixel(wip.startAzimuth, wip.maxElevation, width, height)
            var bottomRight = viewModel.azElToPixel(wip.endAzimuth, wip.minElevation, width, height)

            var rectWidth = bottomRight.x - topLeft.x
            var rectHeight = bottomRight.y - topLeft.y

            ctx.fillStyle = "#00FF9933"
            ctx.fillRect(topLeft.x, topLeft.y, rectWidth, rectHeight)
            ctx.strokeRect(topLeft.x, topLeft.y, rectWidth, rectHeight)

        } else if (type === 2) {  // SectorScan
            var p1 = viewModel.azElToPixel(wip.az1, wip.el1, width, height)
            var p2 = viewModel.azElToPixel(wip.az2, wip.el2, width, height)

            ctx.beginPath()
            ctx.moveTo(p1.x, p1.y)
            ctx.lineTo(p2.x, p2.y)
            ctx.stroke()

            ctx.fillStyle = "#00FF99"
            ctx.beginPath()
            ctx.arc(p1.x, p1.y, 4, 0, 2 * Math.PI)
            ctx.arc(p2.x, p2.y, 4, 0, 2 * Math.PI)
            ctx.fill()

        } else if (type === 3) {  // TRP
            var pos = viewModel.azElToPixel(wip.azimuth, wip.elevation, width, height)

            ctx.beginPath()
            ctx.moveTo(pos.x - 8, pos.y)
            ctx.lineTo(pos.x + 8, pos.y)
            ctx.moveTo(pos.x, pos.y - 8)
            ctx.lineTo(pos.x, pos.y + 8)
            ctx.stroke()
        }

        ctx.setLineDash([])
    }

    function getZoneColor(type) {
        // type: 1=Safety, 2=NoTraverse, 3=NoFire
        switch(type) {
            case 1: return "#00FFFF"  // Cyan for Safety
            case 2: return "#C81428"  // Red for NoTraverse
            case 3: return "#FF00FF"  // Magenta for NoFire
            default: return "#808080" // Gray
        }
    }
}
