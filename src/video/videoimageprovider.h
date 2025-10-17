#ifndef VIDEOIMAGEPROVIDER_H
#define VIDEOIMAGEPROVIDER_H

#include <QQuickImageProvider>
#include <QImage>
#include <QMutex>

/**
 * @brief VideoImageProvider - Thread-safe image provider for QML
 *
 * Provides video frames to QML Image components via "image://video/..." URL.
 * Thread-safe for use with CameraVideoStreamDevice running in separate threads.
 */
class VideoImageProvider : public QQuickImageProvider
{
public:
    VideoImageProvider();

    /**
     * @brief Update the current image (called from camera threads)
     * @param newImage The new video frame to display
     */
    void updateImage(const QImage& newImage);

    /**
     * @brief QML calls this to get the image
     * @param id Image identifier (usually "camera")
     * @param size Output parameter for image size
     * @param requestedSize Requested size (ignored, we return actual size)
     * @return The current video frame
     */
    QImage requestImage(const QString &id, QSize *size, const QSize &requestedSize) override;

private:
    QImage m_currentImage;
    mutable QMutex m_mutex;
};

#endif // VIDEOIMAGEPROVIDER_H
