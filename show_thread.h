#ifndef SHOW_THREAD_H
#define SHOW_THREAD_H

#include <QMainWindow>
#include <QThread>
#include <QTimer>
#include <QImage>

#include <opencv2/imgproc/imgproc_c.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv/cv.h>

using namespace cv;

class show_thread : public QThread
{
    Q_OBJECT
public:
    explicit show_thread(QObject *parent = nullptr);

public slots:
        void ShowFarme();

private:
        QTimer *timer_show;

        Mat mat_image;
        VideoCapture *capture;

        QImage cvMat2QImage(const Mat & mat);   //返回值为QImage的函数

protected:
    //QThread的虚函数
    //线程处理函数
    //不能直接调用，通过start间接调用
    void run();

signals:
    void signal_Done(QImage imag, int count);    //线程结束槽函数

};

#endif // SHOW_THREAD_H
