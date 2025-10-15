#ifndef VIDEOIMAGEPROVIDER_H
#define VIDEOIMAGEPROVIDER_H

#include <QQuickImageProvider>
#include <QImage>
#include <QMutex>

class VideoImageProvider : public QQuickImageProvider
{
public:
    VideoImageProvider() : QQuickImageProvider(QQuickImageProvider::Image) {}

    // Method to update the internal image data from the GStreamer backend
    void updateImage(const QImage& newImage);

    // QQuickImageProvider implementation: provides the image to QML
    QImage requestImage(const QString &id, QSize *size, const QSize &requestedSize) override;

private:
    QImage m_currentImage;
    QMutex m_mutex;
};

#endif // VIDEOIMAGEPROVIDER_H
