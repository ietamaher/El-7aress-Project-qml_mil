#include "videoimageprovider.h"

void VideoImageProvider::updateImage(const QImage& newImage)
{
    QMutexLocker locker(&m_mutex);
    m_currentImage = newImage.copy(); // Copy to ensure thread safety
}

QImage VideoImageProvider::requestImage(const QString &id, QSize *size, const QSize &requestedSize)
{
    QMutexLocker locker(&m_mutex);
    if (m_currentImage.isNull()) {
        if (size) *size = QSize(0, 0);
        return QImage();
    }

    if (size) *size = m_currentImage.size();
    return m_currentImage;
}
