#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "capturethread.h"
#include <QFile>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void createFile(QString &fileName)
    {
        QString data;

        QFile file(fileName);
        if(!file.open(QIODevice::ReadOnly)) {
            fprintf(stderr, "%s file not opened\n", fileName.toStdString().c_str());
            exit(0);
        }
        else
        {
            data = file.readAll();
        }

        file.close();

        QString filename = fileName.remove(":/opencv/");
        QFile temp(filename);

        if(temp.exists())
            return;

        if (temp.open(QIODevice::ReadWrite)) {
            QTextStream stream(&temp);
            stream << data << Qt::endl;
        }
    }

private:

    CaptureThread* cpThread{};
    QGraphicsPixmapItem pixmap;

private slots:    
    void processFrame(Mat&, bool);
    void printInfo(QString);
private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
