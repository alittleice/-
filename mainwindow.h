#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpServer>
#include <QTcpSocket>

#define camera_w  640
#define camera_h  480
#define image_len camera_w*camera_h*3

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    QByteArray rece_array;  //网络接收到的图像
    QByteArray show_array;  //屏幕上显示的图像

    //利用QImage 画图
    QImage imag;        //声明Qlmage对象

private slots:
    void newconnected();
    void recvData();
    void disconnected();    //tcp断开槽函数

    void on_OpenLight_clicked();

    void on_OffLight_clicked();

    void on_disconnect_clicked();

private:
    Ui::MainWindow *ui;

    QTcpServer *server;
    QTcpSocket *socket;
    void ShowFarme();
    unsigned short ByteArray4Toint(QByteArray ba);

protected:


};
#endif // MAINWINDOW_H
