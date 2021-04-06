#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QCamera>
#include <QCameraInfo>
#include <QDateTime>
#include <QVideoFrame>
#include <QGraphicsPixmapItem>
#include "qtcameracapture.h"
#include <QDebug>
#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <algorithm>

#include "RPPG.hpp"


#define DEFAULT_RPPG_ALGORITHM "g"
#define DEFAULT_FACEDET_ALGORITHM "haar"
#define DEFAULT_RESCAN_FREQUENCY 1
#define DEFAULT_SAMPLING_FREQUENCY 1
#define DEFAULT_MIN_SIGNAL_SIZE 5
#define DEFAULT_MAX_SIGNAL_SIZE 5
#define DEFAULT_DOWNSAMPLE 1 // x means only every xth frame is used

#define HAAR_CLASSIFIER_PATH "haarcascade_frontalface_alt.xml"
#define DNN_PROTO_PATH "opencv/deploy.prototxt"
#define DNN_MODEL_PATH "opencv/res10_300x300_ssd_iter_140000.caffemodel"

using namespace cv;
using namespace std;

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

    rPPGAlgorithm to_rppgAlgorithm(string s) {
        rPPGAlgorithm result;
        if (s == "g") result = g;
        else if (s == "pca") result = pca;
        else if (s == "xminay") result = xminay;
        else {
            std::cout << "Please specify valid rPPG algorithm (g, pca, xminay)!" << std::endl;
            exit(0);
        }
        return result;
    }

    faceDetAlgorithm to_faceDetAlgorithm(string s) {
        faceDetAlgorithm result;
        if (s == "haar") result = haar;
        else if (s == "deep") result = deep;
        else {
            std::cout << "Please specify valid face detection algorithm (haar, deep)!" << std::endl;
            exit(0);
        }
        return result;
    }

    bool to_bool(string s) {
        bool result;
        transform(s.begin(), s.end(), s.begin(), ::tolower);
        istringstream is(s);
        is >> boolalpha >> result;
        return result;
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

    QScopedPointer<QCamera> m_camera{};
    QtCameraCapture *m_cameraCapture{};
    QGraphicsPixmapItem pixmap;
    QDateTime h_start{};
    QDateTime h_end{};
    QString info{};
    bool switches_on;
    map<string, string> switch_map;

private slots:
    void updateCameraState(QCamera::State);
    void displayCameraError();
    void processImage(QVideoFrame);
    QImage convertFrameToImage(QVideoFrame vidFrame);

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
