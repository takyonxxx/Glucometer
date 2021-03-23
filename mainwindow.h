#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QCamera>
#include <QCameraInfo>
#include <QGraphicsPixmapItem>
#include "qtcameracapture.h"
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
        qDebug() << f;
        switch( f ) {
        default:
        case QVideoFrame::Format_Invalid:          return QLatin1String("Invalid");
        case QVideoFrame::Format_ARGB32:       return QLatin1String("ARGB32");
        case QVideoFrame::Format_NV21:       return QLatin1String("NV21");
        }
    }

private:
    QScopedPointer<QCamera> m_camera{};
    QtCameraCapture *m_cameraCapture{};
    QGraphicsPixmapItem pixmap;

private slots:
    void updateCameraState(QCamera::State);
    void displayCameraError();
    void processImage(QVideoFrame);

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
