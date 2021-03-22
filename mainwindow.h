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
