#ifndef QTCAMERACAPTURE_H
#define QTCAMERACAPTURE_H

#include <QObject>
#include <QAbstractVideoSurface>
#include <QDebug>

class QtCameraCapture : public QAbstractVideoSurface
{
    Q_OBJECT
public:   
    explicit QtCameraCapture(QObject *parent = 0);

    QList<QVideoFrame::PixelFormat> supportedPixelFormats(
            QAbstractVideoBuffer::HandleType handleType = QAbstractVideoBuffer::NoHandle) const;

    bool present(const QVideoFrame &frame) override;

    QVideoFrame convertFormat(const QVideoFrame &inputframe, QVideoFrame::PixelFormat outputFormat);       


signals:
    void frameAvailable(QVideoFrame frame);

};

#endif // QTCAMERACAPTURE_H
