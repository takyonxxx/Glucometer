#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QCamera>
#include <QCameraInfo>
#include <QDateTime>
#include <QVideoFrame>
#include <QGraphicsPixmapItem>
#include "qtcameracapture.h"
#include "capturethread.h"
#include <QDebug>


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void start();
    void stop();


    QString pixelFormatToString( QVideoFrame::PixelFormat f )
    {
        switch( f ) {
        default:
        case QVideoFrame::Format_Invalid:          return QLatin1String("Invalid");
        case QVideoFrame::Format_ARGB32:       return QLatin1String("ARGB32");
        case QVideoFrame::Format_NV21:       return QLatin1String("NV21");
        }
    }

private:

    void initHeartbeat();
    void initQCamera();

    CaptureThread* cpThread{};

    QScopedPointer<QCamera> m_camera{};
    QtCameraCapture *m_cameraCapture{};
    QGraphicsPixmapItem pixmap;
    QDateTime h_start{};
    QDateTime h_end{};
    QString info{};


private slots:
    void updateCameraState(QCamera::State);
    void displayCameraError();
    void processImage(QVideoFrame);
    void processFrame(Mat&, double);
    QImage convertFrameToImage(QVideoFrame vidFrame);

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
