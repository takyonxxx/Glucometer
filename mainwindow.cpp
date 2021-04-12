#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "RPPG.hpp"

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
    setGeometry(0,0,1024,600);
    ui->graphicsView->scene()->addItem(&pixmap);

    QString fileName = ":/opencv/deploy.prototxt";
    createFile(fileName);
    fileName = ":/opencv/haarcascade_frontalface_alt.xml";
    createFile(fileName);
    fileName = ":/opencv/res10_300x300_ssd_iter_140000.caffemodel";
    createFile(fileName);

    VideoCapture cap;
    if (cap.open(0))
    {
        cap.release();

        cpThread = new CaptureThread{this};
        QObject::connect(cpThread, &CaptureThread::frameCaptured, this, &MainWindow::processFrame);
        connect(cpThread, &CaptureThread::finished, cpThread, &QObject::deleteLater);
        connect(cpThread, &CaptureThread::finished, [&](){ cpThread = nullptr; });
        cpThread->start();

    }
    else
    {
        cap.release();
        initQCamera();
    }
}

MainWindow::~MainWindow()
{
    stop();
    cpThread->abort();
    delete m_cameraCapture;
    delete ui;
}


void MainWindow::initQCamera()
{
    const QList<QCameraInfo> availableCameras = QCameraInfo::availableCameras();
    for (const QCameraInfo &cameraInfo : availableCameras) {
        qDebug() << cameraInfo.description();
    }

    h_start = QDateTime::currentDateTime();
    m_camera.reset(new QCamera(QCameraInfo::defaultCamera()));
    connect(m_camera.data(), &QCamera::stateChanged, this, &MainWindow::updateCameraState);
    connect(m_camera.data(), QOverload<QCamera::Error>::of(&QCamera::error), this, &MainWindow::displayCameraError);

    m_cameraCapture = new QtCameraCapture;
    connect(m_cameraCapture, &QtCameraCapture::frameAvailable, this, &MainWindow::processImage);
    m_camera->setViewfinder(m_cameraCapture);

    start();
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
    RPPG rppg = RPPG();
    rppg.load(HAAR_CLASSIFIER_PATH, DNN_PROTO_PATH, DNN_MODEL_PATH);
    Mat frameRGB, frameGray;
    double bpm = 0.0;


    if(!frameRGB.empty())
    {
        // Generate grayframe
        cvtColor(frameRGB, frameGray, COLOR_BGR2GRAY);
        equalizeHist(frameGray, frameGray);

        int time;
        time = (cv::getTickCount()*1000.0)/cv::getTickFrequency();

        if (count % DEFAULT_DOWNSAMPLE == 0) {
            rppg.processFrame(frameRGB, frameGray, time);
        } else {
            cout << "SKIPPING FRAME TO DOWNSAMPLE!" << endl;
        }

        QImage qimgOriginal((uchar*)frameRGB.data, frameRGB.cols, frameRGB.rows, frameRGB.step, QImage::Format_RGB888);
        pixmap.setPixmap( QPixmap::fromImage(qimgOriginal.rgbSwapped()));
        ui->graphicsView->fitInView(&pixmap, Qt::KeepAspectRatio);
        QString info =  QString::number(int(bpm)) + " bpm";
        ui->label_info->setText(info);

        count++;
    }
    else
    {
        QImage img = convertFrameToImage(frame);
        pixmap.setPixmap( QPixmap::fromImage(img) );
        ui->graphicsView->fitInView(&pixmap, Qt::KeepAspectRatio);
    }

    /*float avgR = 0.0;
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

    h_end = QDateTime::currentDateTime();
    h_start = h_end;


    QString info =  "R: " + QString::number(int(avgR))
            + "\nG: " + QString::number(int(avgG))
            + "\nB: "  + QString::number(int(avgB))
            + "\nH: "  + QString::number(int(avgH));


    ui->label_info->setText(info);*/
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

void MainWindow::processFrame(Mat &frameRGB, double bpm)
{
    QImage qimgOriginal((uchar*)frameRGB.data, frameRGB.cols, frameRGB.rows, frameRGB.step, QImage::Format_RGB888);
    pixmap.setPixmap( QPixmap::fromImage(qimgOriginal.rgbSwapped()));
    ui->graphicsView->fitInView(&pixmap, Qt::KeepAspectRatio);
    QString info =  QString::number(int(bpm)) + " bpm";
    ui->label_info->setText(info);
}

