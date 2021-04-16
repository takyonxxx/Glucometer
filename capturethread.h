#ifndef CAPTURETHREAD_H
#define CAPTURETHREAD_H
#include <QThread>
#include <QCamera>
#include <QCameraInfo>
#include <QVideoFrame>
#include <QGraphicsPixmapItem>
#include "RPPG.hpp"
#include "qtcameracapture.h"

using namespace std;
using namespace cv;

Q_DECLARE_METATYPE(cv::Mat);

class CaptureThread : public QThread
{
    Q_OBJECT

public:

    explicit CaptureThread(QObject *parent = nullptr);
    void abort();

    void setOrientation(const Qt::ScreenOrientation &orientation);

    void setPause(bool pause);

protected:
    void run();

signals:
    void frameCaptured(Mat&, double, bool);
    void sendInfo(QString);

private slots:
    void updateCameraState(QCamera::State);
    void displayCameraError();
    void processImage(QVideoFrame);
    void printInfo(QString);

private:   
    void initQCamera();

    RPPG *rppg{};
    cv::VideoCapture videoCapturer{};
    QScopedPointer<QCamera> m_camera{};
    QtCameraCapture *m_cameraCapture{};
    QString info{};
    bool m_abort {false};
    bool m_pause {false};
    int count{0};
    Qt::ScreenOrientation m_orientation{Qt::ScreenOrientation::PortraitOrientation};
};


#endif // CAPTURETHREAD_H
