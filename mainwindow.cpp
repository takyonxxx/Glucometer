#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->label_info->setStyleSheet("font-size: 24pt; color:#ECF0F1; background-color: #0E6655; padding: 6px; spacing: 6px;");
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
    qDebug() << m_camera->errorString();
}

void MainWindow::processImage(QVideoFrame frame)
{  
    const QImage image(frame.bits(),
                       frame.width(),
                       frame.height(),
                       QVideoFrame::imageFormatFromPixelFormat(frame.pixelFormat()));

    //QImage outImage = image.convertToFormat(QImage::Format_RGB32);

    pixmap.setPixmap( QPixmap::fromImage(image) );
    ui->graphicsView->fitInView(&pixmap, Qt::KeepAspectRatio);

    QString boolText = frame.isValid() ? "true" : "false";

    QString info =  "Frame: " + QString::number(frame.startTime()) + " format: " + pixelFormatToString(frame.pixelFormat()) +
            " valid? : " +  boolText + " size "  + QString::number(frame.width()) + " x " + QString::number(frame.height());

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

