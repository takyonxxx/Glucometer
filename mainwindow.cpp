#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <opencv2/imgcodecs.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include "opencv.hpp"

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
    initHeartbeat();
}

MainWindow::~MainWindow()
{
    stop();
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

    h_end = QDateTime::currentDateTime();
    h_start = h_end;

    pixmap.setPixmap( QPixmap::fromImage(img) );
    ui->graphicsView->fitInView(&pixmap, Qt::KeepAspectRatio);

    QString info =  "R: " + QString::number(int(avgR))
            + "\nG: " + QString::number(int(avgG))
            + "\nB: "  + QString::number(int(avgB))
            + "\nH: "  + QString::number(int(avgH));


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

void MainWindow::initHeartbeat()
{
    // algorithm setting
    rPPGAlgorithm rPPGAlg;
    rPPGAlg = to_rppgAlgorithm(DEFAULT_RPPG_ALGORITHM);
    cout << "Using rPPG algorithm " << rPPGAlg << "." << endl;

    // face detection algorithm setting
    faceDetAlgorithm faceDetAlg;
    faceDetAlg = to_faceDetAlgorithm(DEFAULT_FACEDET_ALGORITHM);
    cout << "Using face detection algorithm " << faceDetAlg << "." << endl;

    // rescanFrequency setting
    double rescanFrequency;
    rescanFrequency = DEFAULT_RESCAN_FREQUENCY;

    // samplingFrequency setting
    double samplingFrequency;
    samplingFrequency = DEFAULT_SAMPLING_FREQUENCY;

    // max signal size setting
    int maxSignalSize;
    maxSignalSize = DEFAULT_MAX_SIGNAL_SIZE;

    // min signal size setting
    int minSignalSize;
    minSignalSize = DEFAULT_MIN_SIGNAL_SIZE;

    if (minSignalSize > maxSignalSize) {
        std::cout << "Max signal size must be greater or equal min signal size!" << std::endl;
        exit(0);
    }

    // Reading gui setting
    bool gui;
    gui = true;

    // Reading log setting
    bool log;
    log = false;

    // Reading downsample setting
    int downsample;
    downsample = DEFAULT_DOWNSAMPLE;

    std::ifstream test1(HAAR_CLASSIFIER_PATH);
    if (!test1) {
        std::cout << "Face classifier xml not found!" << std::endl;
        exit(0);
    }

    std::ifstream test2(DNN_PROTO_PATH);
    if (!test2) {
        std::cout << "DNN proto file not found!" << std::endl;
        exit(0);
    }

    std::ifstream test3(DNN_MODEL_PATH);
    if (!test3) {
        std::cout << "DNN model file not found!" << std::endl;
        exit(0);
    }

    VideoCapture cap;
    cap.open(0);
    if (!cap.isOpened()) {
        qDebug() << "cam not opened";
    }

    bool offlineMode = false;

    std::string title = offlineMode ? "rPPG offline" : "rPPG online";
    cout << title << endl;

    // Configure logfile path
    string LOG_PATH;

    std::ostringstream filepath;
    filepath << "Live_ffmpeg";
    LOG_PATH = filepath.str();

    // Load video information
    const int WIDTH = cap.get(cv::CAP_PROP_FRAME_WIDTH);
    const int HEIGHT = cap.get(cv::CAP_PROP_FRAME_HEIGHT);
    const double FPS = cap.get(cv::CAP_PROP_FPS);
    const double TIME_BASE = 0.001;

    // Print video information
    cout << "SIZE: " << WIDTH << "x" << HEIGHT << endl;
    cout << "FPS: " << FPS << endl;
    cout << "TIME BASE: " << TIME_BASE << endl;

    std::ostringstream window_title;
    window_title << title << " - " << WIDTH << "x" << HEIGHT << " -rppg " << rPPGAlg << " -facedet " << faceDetAlg << " -r " << rescanFrequency << " -f " << samplingFrequency << " -min " << minSignalSize << " -max " << maxSignalSize << " -ds " << downsample;

    // Set up rPPG
    RPPG rppg = RPPG();
    rppg.load(rPPGAlg, faceDetAlg,
              WIDTH, HEIGHT, TIME_BASE, downsample,
              samplingFrequency, rescanFrequency,
              minSignalSize, maxSignalSize,
              LOG_PATH, HAAR_CLASSIFIER_PATH,
              DNN_PROTO_PATH, DNN_MODEL_PATH,
              log, gui);

    cout << "START ALGORITHM" << endl;

    int i = 0;
    Mat frameRGB, frameGray;

    while (true) {

        // Grab RGB frame
        cap.read(frameRGB);

        if (frameRGB.empty())
            break;

        // Generate grayframe
        cvtColor(frameRGB, frameGray, COLOR_BGR2GRAY);
        equalizeHist(frameGray, frameGray);

        int time;
        if (offlineMode) time = (int)cap.get(CAP_PROP_POS_MSEC);
        else time = (cv::getTickCount()*1000.0)/cv::getTickFrequency();

        if (i % downsample == 0) {
            rppg.processFrame(frameRGB, frameGray, time);
        } else {
            cout << "SKIPPING FRAME TO DOWNSAMPLE!" << endl;
        }

        if (gui) {
            imshow(window_title.str(), frameRGB);
            if (waitKey(30) >= 0) break;
        }

        i++;
    }

}

