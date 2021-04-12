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
#include <QFile>
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

    void createFile(QString &fileName)
    {
        QString data;

        QFile file(fileName);
        if(!file.open(QIODevice::ReadOnly)) {
            fprintf(stderr, "%s file not opened\n", fileName.toStdString().c_str());
            exit(0);
        }
        else
        {
            data = file.readAll();
        }

        file.close();

        QString filename = fileName.remove(":/opencv/");
        QFile temp(filename);

        if(temp.exists())
            return;

        if (temp.open(QIODevice::ReadWrite)) {
            QTextStream stream(&temp);
            stream << data << endl;
        }
    }


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
    int count{0};

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
