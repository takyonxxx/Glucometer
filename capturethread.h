#ifndef CAPTURETHREAD_H
#define CAPTURETHREAD_H
#include <QThread>
#include <QCamera>
#include <QCameraInfo>
#include <QDateTime>
#include <QVideoFrame>
#include <QGraphicsPixmapItem>
#include "RPPG.hpp"
#include "qtcameracapture.h"
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

Q_DECLARE_METATYPE(cv::Mat);

class CaptureThread : public QThread
{
    Q_OBJECT

public:

    explicit CaptureThread(QObject *parent);
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

    QString convert_to_rgb(QImage &img)
    {
        float avgR = 0.0;
        float avgG = 0.0;
        float avgB = 0.0;
        float avgH = 0.0;

        int totalPixels = img.height() * img.width();

        for ( int row = 0; row < img.height() ; row++)
            for ( int col = 0; col < img.width() ; col++ )
            {
                QColor clrCurrent( img.pixel( col, row ));
                avgR += clrCurrent.red();
                avgG += clrCurrent.green();
                avgB += clrCurrent.blue();
                avgH += clrCurrent.hue();
            }

        avgR = avgR / totalPixels;
        avgG = avgG / totalPixels;
        avgB = avgB / totalPixels;
        avgH = avgH / totalPixels;

        QString rgb =  "R: " + QString::number(int(avgR))
                + "\nG: " + QString::number(int(avgG))
                + "\nB: "  + QString::number(int(avgB))
                + "\nH: "  + QString::number(int(avgH));

        return rgb;
    }
};


#endif // CAPTURETHREAD_H
