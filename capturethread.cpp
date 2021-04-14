#include "capturethread.h"

#include "RPPG.hpp"
#include <QDebug>

CaptureThread::CaptureThread(QObject* parent)
    :QThread(parent)
{
    qRegisterMetaType<Mat>("Mat&");

    rppg = new RPPG(this);
    connect(rppg, &RPPG::sendInfo, this, &CaptureThread::printInfo);
    rppg->load(0, HAAR_CLASSIFIER_PATH, DNN_PROTO_PATH, DNN_MODEL_PATH);

    if(!videoCapturer.open(0))
    {
        initQCamera();
    }
}

void CaptureThread::abort()
{    
    m_abort = true;
    if( m_camera )
        m_camera->stop();
    if(rppg)
        delete rppg;
}

void CaptureThread::initQCamera()
{
    QCameraInfo cam_face;
    const QList<QCameraInfo> availableCameras = QCameraInfo::availableCameras();
    for (const QCameraInfo &cameraInfo : availableCameras) {
        cam_face = cameraInfo;
        info = cameraInfo.description();
        emit printInfo(info);
    }

    m_camera.reset(new QCamera(cam_face));
    connect(m_camera.data(), &QCamera::stateChanged, this, &CaptureThread::updateCameraState);
    connect(m_camera.data(), QOverload<QCamera::Error>::of(&QCamera::error), this, &CaptureThread::displayCameraError);

    m_cameraCapture = new QtCameraCapture;
    connect(m_cameraCapture, &QtCameraCapture::frameAvailable, this, &CaptureThread::processImage);
    m_camera->setViewfinder(m_cameraCapture);

    if (m_camera->state() == QCamera::ActiveState) {
        m_camera->stop();
    }

    m_camera->start();
}

void CaptureThread::updateCameraState(QCamera::State state)
{
    qDebug() << "Camera status: " << m_camera->status() << state;
}

void CaptureThread::displayCameraError()
{
    qDebug() << "Error : " <<  m_camera->errorString();
}

void CaptureThread::processImage(QVideoFrame frame)
{

    if (frame.isValid())
    {
        QVideoFrame cloneFrame(frame);
        cloneFrame.map(QAbstractVideoBuffer::ReadOnly);
        QImage img = cloneFrame.image();
        cloneFrame.unmap();

        QTransform transf;
        //transf.scale(1, -1);
        transf.rotate(270);
        img = img.transformed(transf);
        img = img.mirrored(true, false);
        img = img.convertToFormat(QImage::Format_RGB888);

        Mat frameRGB(img.height(),
                     img.width(),
                     CV_8UC3,
                     img.bits(),
                     img.bytesPerLine());

        if(!frameRGB.empty())
        {
            Mat frameGray;
            double bpm = 0.0;

            // Generate grayframe
            cvtColor(frameRGB, frameGray, COLOR_BGR2GRAY);
            equalizeHist(frameGray, frameGray);

            int time;
            time = (cv::getTickCount()*1000.0)/cv::getTickFrequency();

            if (count % DEFAULT_DOWNSAMPLE == 0)
            {
                bpm = rppg->processFrame(frameRGB, frameGray, time);
            }
            else
            {
                cout << "SKIPPING FRAME TO DOWNSAMPLE!" << endl;
            }
            emit frameCaptured(frameRGB, false);
            count++;
        }
    }
}

void CaptureThread::printInfo(QString info)
{
    emit sendInfo(info);
}

void CaptureThread::run()
{
    Mat frameRGB, frameGray;
    double bpm = 0.0;

    if(videoCapturer.isOpened())
    {
        info = "opencv cam active";
        emit sendInfo(info);
    }
    else
    {
        info = "qt cam active";
        emit sendInfo(info);
    }

    while(!m_abort)
    {
        if(videoCapturer.isOpened())
        {
            videoCapturer.read(frameRGB);

            if(!frameRGB.empty())
            {
                // Generate grayframe
                cvtColor(frameRGB, frameGray, COLOR_BGR2GRAY);
                equalizeHist(frameGray, frameGray);

                int time;
                time = (cv::getTickCount()*1000.0)/cv::getTickFrequency();

                if (count % DEFAULT_DOWNSAMPLE == 0) {
                    bpm = rppg->processFrame(frameRGB, frameGray, time);
                } else {
                    info = "SKIPPING FRAME TO DOWNSAMPLE!";
                    emit sendInfo(info);
                }

                count++;

                emit frameCaptured(frameRGB, true);

            }
        }
        else{
            QThread::sleep(1);
        }
    }
}
