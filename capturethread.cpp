#include "capturethread.h"

#include "RPPG.hpp"


CaptureThread::CaptureThread(QObject* parent)
    :QThread(parent)
{
    qRegisterMetaType<Mat>("Mat&");
}

void CaptureThread::abort()
{
    m_abort = true;
}

void CaptureThread::run()
{
    RPPG rppg = RPPG();
    rppg.load(HAAR_CLASSIFIER_PATH, DNN_PROTO_PATH, DNN_MODEL_PATH);

    Mat frameRGB, frameGray;
    double bpm = 0.0;

    if(!videoCapturer.open(0))
    {
        cout << "Make sure you entered a correct and supported video index" << endl;
        return;
    }

    while(!m_abort)
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
                rppg.processFrame(frameRGB, frameGray, time);
            } else {
                cout << "SKIPPING FRAME TO DOWNSAMPLE!" << endl;
            }

            emit frameCaptured(frameRGB, bpm);
            count++;
        }
    }
}

