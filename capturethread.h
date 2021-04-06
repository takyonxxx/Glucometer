#ifndef CAPTURETHREAD_H
#define CAPTURETHREAD_H
#include <QThread>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

Q_DECLARE_METATYPE(cv::Mat);

class CaptureThread : public QThread
{
    Q_OBJECT

public:

    explicit CaptureThread(QObject *parent);
    void abort();

protected:
    void run();

signals:
    void frameCaptured(Mat&, double);

private:   
    cv::VideoCapture videoCapturer;
    bool m_abort {false};
    int count{0};
};


#endif // CAPTURETHREAD_H
