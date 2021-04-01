#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>
using namespace std;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->label_info->setStyleSheet("font-size: 24pt; color:#ECF0F1; background-color: #212F3C; padding: 6px; spacing: 6px;");
    ui->graphicsView->setStyleSheet("font-size: 24pt; color:#ECF0F1; background-color: #212F3C; padding: 6px; spacing: 6px;");
    ui->label_info->setWordWrap(true);
    //QString currentTime = QDateTime::currentDateTime().toString("yyyyMMddhhmmss");
    ui->graphicsView->setScene(new QGraphicsScene(this));
    setGeometry(0,0,800,480);
    ui->graphicsView->scene()->addItem(&pixmap);

    const QList<QCameraInfo> availableCameras = QCameraInfo::availableCameras();
    for (const QCameraInfo &cameraInfo : availableCameras) {
        qDebug() << cameraInfo.description();
    }

    m_camera.reset(new QCamera(QCameraInfo::defaultCamera()));
    connect(m_camera.data(), &QCamera::stateChanged, this, &MainWindow::updateCameraState);
    connect(m_camera.data(), QOverload<QCamera::Error>::of(&QCamera::error), this, &MainWindow::displayCameraError);

    m_cameraCapture = new QtCameraCapture;
    connect(m_cameraCapture, &QtCameraCapture::frameAvailable, this, &MainWindow::processImage);
    m_camera->setViewfinder(m_cameraCapture);

    start();
}

MainWindow::~MainWindow()
{
    stop();
    delete m_cameraCapture;
    delete ui;
}

void MainWindow::updateCameraState(QCamera::State state)
{
    qDebug() << "Camera status: " << m_camera->status() << state;
}

void MainWindow::displayCameraError()
{
    qDebug() << "Error : " <<  m_camera->errorString();
}

QImage MainWindow::convertFrameToImage(QVideoFrame vidFrame)
{
    //QImage::Format imageFormat = QVideoFrame::imageFormatFromPixelFormat(vidFrame.pixelFormat());
    QImage image = vidFrame.image();
    return image;
}

void MainWindow::processImage(QVideoFrame frame)
{  
    QImage img = convertFrameToImage(frame);

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

    pixmap.setPixmap( QPixmap::fromImage(img) );
    ui->graphicsView->fitInView(&pixmap, Qt::KeepAspectRatio);

    //QString boolText = frame.isValid() ? "true" : "false";

    QString info =  "R: " + QString::number(int(avgR))
            + "\nG: " + QString::number(int(avgG))
            + "\nB: "  + QString::number(int(avgB))
            + "\nH: "  + QString::number(int(avgH));

    /*QString info =  "Frame: " + QString::number(frame.startTime()) + "\nFormat: " + pixelFormatToString(frame.pixelFormat()) +
            "\nSize "  + QString::number(frame.width()) + " x " + QString::number(frame.height());*/

    ui->label_info->setText(info);
}

void MainWindow::start()
{
    if (m_camera->state() == QCamera::ActiveState) {
        m_camera->stop();
    }

    m_camera->start();
}

void MainWindow::stop()
{
    if( m_camera )
        m_camera->stop();
}

