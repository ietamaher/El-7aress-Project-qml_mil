import QtQuick

Canvas {
    id: canvas

    property int reticleType: 1 // 0=Basic, 1=BoxCrosshair, 2=Standard, 3=Precision, 4=MilDot
    property color color: "#46E2A5"
    property real currentFov: 45.0

    width: 200
    height: 200

    onReticleTypeChanged: requestPaint()
    onColorChanged: requestPaint()
    onCurrentFovChanged: requestPaint()

    onPaint: {
        var ctx = getContext("2d");
        ctx.reset();

        ctx.strokeStyle = canvas.color;
        ctx.lineWidth = 2;
        ctx.lineCap = "round";
        ctx.lineJoin = "round";

        var centerX = width / 2;
        var centerY = height / 2;

        switch (reticleType) {
            case 0: drawBasicReticle(ctx, centerX, centerY); break;
            case 1: drawBoxCrosshair(ctx, centerX, centerY); break;
            case 2: drawStandardCrosshair(ctx, centerX, centerY); break;
            case 3: drawPrecisionCrosshair(ctx, centerX, centerY); break;
            case 4: drawMilDotReticle(ctx, centerX, centerY); break;
            default: drawStandardCrosshair(ctx, centerX, centerY); break;
        }
    }

    function drawBasicReticle(ctx, cx, cy) {
        var size = 20;
        ctx.beginPath();
        ctx.moveTo(cx - size, cy);
        ctx.lineTo(cx + size, cy);
        ctx.moveTo(cx, cy - size);
        ctx.lineTo(cx, cy + size);
        ctx.stroke();
    }

    function drawBoxCrosshair(ctx, cx, cy) {
        var lineLen = 80;
        var boxSize = 50;
        var halfBox = boxSize / 2;
        var gap = 2;

        // Horizontal lines
        ctx.beginPath();
        ctx.moveTo(cx - lineLen, cy);
        ctx.lineTo(cx - halfBox - gap, cy);
        ctx.moveTo(cx + halfBox + gap, cy);
        ctx.lineTo(cx + lineLen, cy);

        // Vertical lines
        ctx.moveTo(cx, cy - lineLen);
        ctx.lineTo(cx, cy - halfBox - gap);
        ctx.moveTo(cx, cy + halfBox + gap);
        ctx.lineTo(cx, cy + lineLen);
        ctx.stroke();

        // Box
        ctx.strokeRect(cx - halfBox, cy - halfBox, boxSize, boxSize);
    }

    function drawStandardCrosshair(ctx, cx, cy) {
        var size = 60;
        var gap = 10;

        ctx.beginPath();
        // Horizontal
        ctx.moveTo(cx - size, cy);
        ctx.lineTo(cx - gap, cy);
        ctx.moveTo(cx + gap, cy);
        ctx.lineTo(cx + size, cy);

        // Vertical
        ctx.moveTo(cx, cy - size);
        ctx.lineTo(cx, cy - gap);
        ctx.moveTo(cx, cy + gap);
        ctx.lineTo(cx, cy + size);
        ctx.stroke();
    }

    function drawPrecisionCrosshair(ctx, cx, cy) {
        var size = 100;
        var dotRadius = 2;
        var tickLen = 5;
        var tickSpacing = 15;
        var numTicks = 5;

        // Center dot
        ctx.beginPath();
        ctx.arc(cx, cy, dotRadius, 0, 2 * Math.PI);
        ctx.fill();

        // Main lines
        ctx.beginPath();
        ctx.moveTo(cx - size, cy);
        ctx.lineTo(cx + size, cy);
        ctx.moveTo(cx, cy - size);
        ctx.lineTo(cx, cy + size);
        ctx.stroke();

        // Tick marks
        ctx.beginPath();
        for (var i = 1; i <= numTicks; i++) {
            var dist = i * tickSpacing;
            // Horizontal ticks
            ctx.moveTo(cx - dist, cy - tickLen);
            ctx.lineTo(cx - dist, cy + tickLen);
            ctx.moveTo(cx + dist, cy - tickLen);
            ctx.lineTo(cx + dist, cy + tickLen);

            // Vertical ticks
            ctx.moveTo(cx - tickLen, cy - dist);
            ctx.lineTo(cx + tickLen, cy - dist);
            ctx.moveTo(cx - tickLen, cy + dist);
            ctx.lineTo(cx + tickLen, cy + dist);
        }
        ctx.stroke();
    }

    function drawMilDotReticle(ctx, cx, cy) {
        var lineSize = 120;
        var dotRadius = 1.5;
        var numDots = 4;

        // Calculate pixels per mil based on FOV
        var pixelsPerMil = calculatePixelsPerMil(currentFov, width);
        var dotSpacing = pixelsPerMil;

        // Main lines
        ctx.beginPath();
        ctx.moveTo(cx - lineSize, cy);
        ctx.lineTo(cx + lineSize, cy);
        ctx.moveTo(cx, cy - lineSize);
        ctx.lineTo(cx, cy + lineSize);
        ctx.stroke();

        // Dots
        ctx.fillStyle = canvas.color;
        for (var i = 1; i <= numDots; i++) {
            var dist = i * dotSpacing;
            if (dist > lineSize) break;

            // Horizontal dots
            ctx.beginPath();
            ctx.arc(cx - dist, cy, dotRadius, 0, 2 * Math.PI);
            ctx.arc(cx + dist, cy, dotRadius, 0, 2 * Math.PI);
            ctx.fill();

            // Vertical dots
            ctx.beginPath();
            ctx.arc(cx, cy - dist, dotRadius, 0, 2 * Math.PI);
            ctx.arc(cx, cy + dist, dotRadius, 0, 2 * Math.PI);
            ctx.fill();
        }
    }

    function calculatePixelsPerMil(hfovDegrees, screenWidth) {
        if (hfovDegrees <= 0 || screenWidth <= 0) return 0;

        var hfovRadians = hfovDegrees * Math.PI / 180.0;
        var visibleWidthAt1000 = 2.0 * 1000.0 * Math.tan(hfovRadians / 2.0);
        var milsAcrossScreen = visibleWidthAt1000;

        return screenWidth / milsAcrossScreen;
    }
}
