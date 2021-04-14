#include "qtcameracapture.h"

QtCameraCapture::QtCameraCapture(QObject *parent) : QAbstractVideoSurface(parent)
{

}

QList<QVideoFrame::PixelFormat> QtCameraCapture::supportedPixelFormats(QAbstractVideoBuffer::HandleType handleType) const
{
    if (handleType == QAbstractVideoBuffer::NoHandle) {
        return QList<QVideoFrame::PixelFormat>()
                << QVideoFrame::Format_ARGB32
                << QVideoFrame::Format_RGB32
                << QVideoFrame::Format_RGB24
                << QVideoFrame::Format_NV12
                << QVideoFrame::Format_NV21;

    } else {
        return QList<QVideoFrame::PixelFormat>();
    }
}

bool QtCameraCapture::present(const QVideoFrame &frame)
{
    if (frame.isValid()) {
        QVideoFrame cloneFrame(frame);
        cloneFrame.map(QAbstractVideoBuffer::ReadOnly);
        if (frame.width() != 0 && frame.height() !=0)
            emit frameAvailable(cloneFrame);
        cloneFrame.unmap();
        return true;
    }
    return false;
}

QVideoFrame QtCameraCapture::convertFormat(const QVideoFrame &inputframe, QVideoFrame::PixelFormat outputFormat)
{
    QVideoFrame outputFrame;
    return outputFrame;
}
