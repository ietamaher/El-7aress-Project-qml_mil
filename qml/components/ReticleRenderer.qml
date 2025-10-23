import QtQuick

Canvas {
    id: canvas

    property int reticleType: 1 // 0=CircleDot, 1=BoxCrosshair, 2=TacticalCrosshair, 3=CCIP, 4=MilDot
    property color color: "#46E2A5"
    property color outlineColor: "#000000"
    property real outlineWidth: 2
    property real currentFov: 45.0

    // CCIP-specific properties
    property bool lacActive: false
    property real rangeMeters: 0
    property real confidenceLevel: 1.0 // 0.0 to 1.0

    width: 200
    height: 200

    onReticleTypeChanged: requestPaint()
    onColorChanged: requestPaint()
    onCurrentFovChanged: requestPaint()
    onLacActiveChanged: requestPaint()
    onRangeMetersChanged: requestPaint()
    onConfidenceLevelChanged: requestPaint()

    onPaint: {
        var ctx = getContext("2d");
        ctx.reset();

        var centerX = width / 2;
        var centerY = height / 2;

        switch (reticleType) {
            case 0: drawCircleDotReticle(ctx, centerX, centerY); break;
            case 1: drawBoxCrosshair(ctx, centerX, centerY); break;
            case 2: drawTacticalCrosshair(ctx, centerX, centerY); break;
            case 3: drawCCIPReticle(ctx, centerX, centerY); break;
            case 4: drawMilDotReticle(ctx, centerX, centerY); break;
            default: drawBoxCrosshair(ctx, centerX, centerY); break;
        }
    }

    // Helper function to draw with outline
    function drawWithOutline(ctx, drawFunc) {
        // Draw outline
        ctx.strokeStyle = canvas.outlineColor;
        ctx.lineWidth = 2 + outlineWidth;
        ctx.lineCap = "round";
        ctx.lineJoin = "round";
        drawFunc(ctx);

        // Draw main
        ctx.strokeStyle = canvas.color;
        ctx.lineWidth = 2;
        drawFunc(ctx);
    }

    // TYPE 0: CIRCLE-DOT RETICLE (Close range, rapid acquisition)
    function drawCircleDotReticle(ctx, cx, cy) {
        var circleRadius = 25; // ~2 MOA at typical distances
        var dotRadius = 2;

        drawWithOutline(ctx, function(c) {
            // Outer circle
            c.beginPath();
            c.arc(cx, cy, circleRadius, 0, 2 * Math.PI);
            c.stroke();
        });

        // Center dot with outline
        ctx.fillStyle = canvas.outlineColor;
        ctx.beginPath();
        ctx.arc(cx, cy, dotRadius + outlineWidth/2, 0, 2 * Math.PI);
        ctx.fill();

        ctx.fillStyle = canvas.color;
        ctx.beginPath();
        ctx.arc(cx, cy, dotRadius, 0, 2 * Math.PI);
        ctx.fill();
    }

    // TYPE 1: BOX CROSSHAIR (General purpose - NATO standard)
    function drawBoxCrosshair(ctx, cx, cy) {
        var lineLen = 80;
        var boxSize = 50;
        var halfBox = boxSize / 2;
        var gap = 2;

        drawWithOutline(ctx, function(c) {
            c.beginPath();
            // Horizontal lines
            c.moveTo(cx - lineLen, cy);
            c.lineTo(cx - halfBox - gap, cy);
            c.moveTo(cx + halfBox + gap, cy);
            c.lineTo(cx + lineLen, cy);

            // Vertical lines
            c.moveTo(cx, cy - lineLen);
            c.lineTo(cx, cy - halfBox - gap);
            c.moveTo(cx, cy + halfBox + gap);
            c.lineTo(cx, cy + lineLen);
            c.stroke();

            // Box
            c.strokeRect(cx - halfBox, cy - halfBox, boxSize, boxSize);
        });
    }

    // TYPE 2: TACTICAL CROSSHAIR (Precision with wind/elevation marks)
    function drawTacticalCrosshair(ctx, cx, cy) {
        var size = 80;
        var gap = 8;
        var hashLength = 4;
        var hashSpacing = 10; // Represents ~2 mils each

        drawWithOutline(ctx, function(c) {
            c.beginPath();
            // Main horizontal crosshair
            c.moveTo(cx - size, cy);
            c.lineTo(cx - gap, cy);
            c.moveTo(cx + gap, cy);
            c.lineTo(cx + size, cy);

            // Main vertical crosshair
            c.moveTo(cx, cy - size);
            c.lineTo(cx, cy - gap);
            c.moveTo(cx, cy + gap);
            c.lineTo(cx, cy + size);

            // Windage hash marks (horizontal axis)
            for (var i = 1; i <= 6; i++) {
                var dist = i * hashSpacing;
                if (dist < size - gap) {
                    // Left side
                    c.moveTo(cx - dist, cy - hashLength);
                    c.lineTo(cx - dist, cy + hashLength);
                    // Right side
                    c.moveTo(cx + dist, cy - hashLength);
                    c.lineTo(cx + dist, cy + hashLength);
                }
            }

            // Elevation holdover marks (vertical axis - below center)
            for (var j = 1; j <= 6; j++) {
                var vDist = j * hashSpacing;
                if (vDist < size - gap) {
                    // Below center only (for bullet drop)
                    c.moveTo(cx - hashLength, cy + vDist);
                    c.lineTo(cx + hashLength, cy + vDist);
                }
            }

            // Stadia lines (longer marks every 3 intervals = ~5 mils)
            var stadiaLength = 8;
            for (var k = 3; k <= 6; k += 3) {
                var sDist = k * hashSpacing;
                if (sDist < size - gap) {
                    // Horizontal stadia
                    c.moveTo(cx - sDist, cy - stadiaLength);
                    c.lineTo(cx - sDist, cy + stadiaLength);
                    c.moveTo(cx + sDist, cy - stadiaLength);
                    c.lineTo(cx + sDist, cy + stadiaLength);

                    // Vertical stadia (below only)
                    c.moveTo(cx - stadiaLength, cy + sDist);
                    c.lineTo(cx + stadiaLength, cy + sDist);
                }
            }

            c.stroke();
        });

        // Center dot
        ctx.fillStyle = canvas.outlineColor;
        ctx.beginPath();
        ctx.arc(cx, cy, 2 + outlineWidth/2, 0, 2 * Math.PI);
        ctx.fill();

        ctx.fillStyle = canvas.color;
        ctx.beginPath();
        ctx.arc(cx, cy, 2, 0, 2 * Math.PI);
        ctx.fill();
    }

    // TYPE 3: CCIP FIRE CONTROL RETICLE (Dynamic fire control)
    function drawCCIPReticle(ctx, cx, cy) {
        var pipperRadius = 12;
        var fpvWingSpan = 20;
        var fpvWingHeight = 6;

        // Center pipper (impact point indicator)
        drawWithOutline(ctx, function(c) {
            c.beginPath();
            c.arc(cx, cy, pipperRadius, 0, 2 * Math.PI);
            c.stroke();

            // Center dot
            c.beginPath();
            c.arc(cx, cy, 2, 0, 2 * Math.PI);
            c.fill();
        });

        // Flight path vector (FPV) symbol - small aircraft symbol
        drawWithOutline(ctx, function(c) {
            c.beginPath();
            // Wings
            c.moveTo(cx - fpvWingSpan, cy);
            c.lineTo(cx - 6, cy);
            c.moveTo(cx + 6, cy);
            c.lineTo(cx + fpvWingSpan, cy);
            // Wing tips up
            c.moveTo(cx - fpvWingSpan, cy);
            c.lineTo(cx - fpvWingSpan, cy - fpvWingHeight);
            c.moveTo(cx + fpvWingSpan, cy);
            c.lineTo(cx + fpvWingSpan, cy - fpvWingHeight);
            c.stroke();
        });

        // LAC indicator (when active) - L-shaped bracket
        if (lacActive) {
            var bracketSize = 25;
            var bracketThick = 3;

            drawWithOutline(ctx, function(c) {
                c.beginPath();
                // Upper-right L bracket
                c.moveTo(cx + pipperRadius + 8, cy - bracketSize);
                c.lineTo(cx + pipperRadius + 8 + bracketSize, cy - bracketSize);
                c.lineTo(cx + pipperRadius + 8 + bracketSize, cy - bracketSize + bracketSize);
                c.stroke();

                // "LAC" text would go here - handled by separate text item in QML
            });
        }

        // Range scale (right side)
        var scaleX = cx + 50;
        var scaleHeight = 60;
        var ranges = [2000, 1500, 1000, 500];

        ctx.font = "bold 10px 'Archivo Narrow'";
        ctx.textAlign = "left";
        ctx.textBaseline = "middle";

        for (var i = 0; i < ranges.length; i++) {
            var rangeY = cy - scaleHeight/2 + (i * scaleHeight / (ranges.length - 1));
            var isCurrentRange = (rangeMeters >= ranges[i] - 250 && rangeMeters <= ranges[i] + 250);

            // Scale tick
            drawWithOutline(ctx, function(c) {
                c.beginPath();
                c.moveTo(scaleX, rangeY);
                c.lineTo(scaleX + (isCurrentRange ? 12 : 8), rangeY);
                c.stroke();
            });

            // Range text with outline
            ctx.fillStyle = canvas.outlineColor;
            for (var ox = -1; ox <= 1; ox++) {
                for (var oy = -1; oy <= 1; oy++) {
                    ctx.fillText(ranges[i] + "m", scaleX + 15 + ox, rangeY + oy);
                }
            }
            ctx.fillStyle = isCurrentRange ? canvas.color : Qt.darker(canvas.color, 1.5);
            ctx.fillText(ranges[i] + "m", scaleX + 15, rangeY);
        }

        // Confidence bars (bottom)
        var barWidth = 40;
        var barHeight = 4;
        var barY = cy + pipperRadius + 25;

        // Background bar
        ctx.fillStyle = canvas.outlineColor;
        ctx.fillRect(cx - barWidth/2 - 1, barY - 1, barWidth + 2, barHeight + 2);

        // Confidence level bar
        ctx.fillStyle = confidenceLevel > 0.7 ? canvas.color :
                        confidenceLevel > 0.4 ? Qt.rgba(1, 1, 0, 1) :
                        Qt.rgba(1, 0, 0, 1);
        ctx.fillRect(cx - barWidth/2, barY, barWidth * confidenceLevel, barHeight);
    }

    // TYPE 4: MIL-DOT RANGING RETICLE (LRF verification, precision)
    function drawMilDotReticle(ctx, cx, cy) {
        var lineSize = 100;
        var dotRadius = 1.8; // 0.2 mil standard
        var numDots = 5;

        var pixelsPerMil = calculatePixelsPerMil(currentFov, width);
        var dotSpacing = pixelsPerMil;

        // Main crosshair (fine, 1px)
        drawWithOutline(ctx, function(c) {
            c.lineWidth = 1;
            c.beginPath();
            c.moveTo(cx - lineSize, cy);
            c.lineTo(cx + lineSize, cy);
            c.moveTo(cx, cy - lineSize);
            c.lineTo(cx, cy + lineSize);
            c.stroke();
        });

        // Mil-dots with outline
        for (var i = 1; i <= numDots; i++) {
            var dist = i * dotSpacing;
            if (dist > lineSize) break;

            // Horizontal dots
            drawDotWithOutline(ctx, cx - dist, cy, dotRadius);
            drawDotWithOutline(ctx, cx + dist, cy, dotRadius);

            // Vertical dots
            drawDotWithOutline(ctx, cx, cy - dist, dotRadius);
            drawDotWithOutline(ctx, cx, cy + dist, dotRadius);

            // Half-mil subtensions (smaller dots) for precision
            if (i < numDots) {
                var halfDist = dist + dotSpacing / 2;
                if (halfDist <= lineSize) {
                    var smallRadius = dotRadius * 0.6;
                    drawDotWithOutline(ctx, cx - halfDist, cy, smallRadius);
                    drawDotWithOutline(ctx, cx + halfDist, cy, smallRadius);
                    drawDotWithOutline(ctx, cx, cy - halfDist, smallRadius);
                    drawDotWithOutline(ctx, cx, cy + halfDist, smallRadius);
                }
            }
        }

        // Mil numbers at 5-mil intervals
        ctx.font = "bold 9px 'Archivo Narrow'";
        ctx.textAlign = "center";
        ctx.textBaseline = "middle";

        for (var j = 5; j <= numDots; j += 5) {
            var numDist = j * dotSpacing;
            if (numDist <= lineSize) {
                // Text with outline
                ctx.fillStyle = canvas.outlineColor;
                for (var ox = -1; ox <= 1; ox++) {
                    for (var oy = -1; oy <= 1; oy++) {
                        ctx.fillText(j.toString(), cx, cy - numDist - 8 + oy);
                    }
                }
                ctx.fillStyle = canvas.color;
                ctx.fillText(j.toString(), cx, cy - numDist - 8);
            }
        }
    }

    function drawDotWithOutline(ctx, x, y, radius) {
        // Outline
        ctx.fillStyle = canvas.outlineColor;
        ctx.beginPath();
        ctx.arc(x, y, radius + outlineWidth/2, 0, 2 * Math.PI);
        ctx.fill();

        // Main dot
        ctx.fillStyle = canvas.color;
        ctx.beginPath();
        ctx.arc(x, y, radius, 0, 2 * Math.PI);
        ctx.fill();
    }

    function calculatePixelsPerMil(hfovDegrees, screenWidth) {
        if (hfovDegrees <= 0 || screenWidth <= 0) return 0;
        var hfovRadians = hfovDegrees * Math.PI / 180.0;
        var visibleWidthAt1000 = 2.0 * 1000.0 * Math.tan(hfovRadians / 2.0);
        var milsAcrossScreen = visibleWidthAt1000;
        return screenWidth / milsAcrossScreen;
    }
}
