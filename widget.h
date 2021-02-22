#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QApplication>
#include <QDir>
#include <QTextCodec>
#include <QDesktopWidget>
#include <QLabel>

#include <QPixmap>
#include <QPushButton>
#include <QSplitter>
#include <QTimer>
#include <QSocketNotifier>
#include <unistd.h>
#include <QTcpSocket>
#include <QBuffer>
#include <QLCDNumber>
#include <QCheckBox>
#include <QTouchEvent>
#include <QEvent>

#include <opencv2/imgproc/imgproc_c.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv/cv.h>
#include <opencv/cxcore.h>

//#include "show_thread.h"

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

using namespace cv;

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

private slots:

    //定时器槽函数
    void ShowFarme();
    void SendFarme();

    //tcp连接断开槽函数
    void connect_suc();
    void client_dis();

    //按钮槽函数
    void on_btnconnect_clicked();
    void on_btndisconnect_clicked();
 //   void on_send_clicked();


    void on_send_clicked();

    void on_btndisconnect_released();

private:
    Ui::Widget *ui;

    VideoCapture *capture;
    Mat mat_image;
    CvCapture *camera;  //视频获取结构，用来作为视频获取函数的一个参数
    QImage imag;        //声明Qlmage对象

    void dealDone(QImage imag, int count);   //线程槽函数

    QImage cvMat2QImage(const Mat & mat);   //返回值为QImage的函数


    void Initcamara();

    QTimer timer_show;
    QTimer timer_send;

    QTcpSocket *mSocketM;

    //show_thread *thread;    //线程对象 定义了thread的属性为show_thread

protected:
    bool eventFilter(QObject *obj, QEvent *event);  //事件过滤器

};
#endif // WIDGET_H
