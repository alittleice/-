#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
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




#include "show_thread.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();



private slots:

    //定时器槽函数
    void ReadFarme();
    void ShowFarme();
    void SendFarme();

    //tcp连接断开槽函数
    void connect_suc();
    void client_dis();

    //按钮槽函数
    void on_btnconnect_clicked();
    void on_btndisconnect_clicked();
    void on_send_clicked();

    //线程结束槽函数
    void dealDone();

private:
    Ui::MainWindow *ui;

    CvCapture *camera;  //视频获取结构，用来作为视频获取函数的一个参数
    IplImage *frame;    //申请 Ipllmage类型指针，就是申请内存空间来存放每一帧图像
    QImage imag;        //声明Qlmage对象

    void Initcamara();

    QTimer timer_read;

    QTimer timer_send;

    QTcpSocket *mSocketM;

    show_thread *thread;    //线程对象 定义了thread的属性为show_thread


};
#endif // MAINWINDOW_H
