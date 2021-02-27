#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPainter>
#include <QBuffer>
#include <QLabel>
#include <QImage>
#include <QPixmap>


#define PORT 8888//指定通信所用端口


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
//    show_array.resize(image_len);
//    show_array.fill(0xff);  //图像数组初始化
//    rece_array.resize(image_len);
//    rece_array.fill(0xff);  //图像数组初始化
    //ui->textEdit->setReadOnly(true);

    server = new QTcpServer;
    socket = new QTcpSocket;
    if(!server->listen(QHostAddress::Any, PORT)){   //监听端口
            return;
    }
    connect(server,SIGNAL(newConnection()),this,SLOT(newconnected()));  //连接新创建槽函数
}

void MainWindow::newconnected()
{
    if(server->hasPendingConnections()){    //如果服务端有一个待处理的连接，就返回真，否则返回假
        socket = server->nextPendingConnection();   //将下一个挂起的连接作为已连接的qtcsocket对象返回。
        if(!socket->isValid()){     //套接字是否有效
            return;
        }
        connect(socket,SIGNAL(readyRead()),this,SLOT(recvData()));//连接接收槽函数
        ui->label_state->setText("TCP状态：已连接");
        connect(socket,SIGNAL(disconnected()),this,SLOT(disconnected()));//断开槽函数
    }
}
void MainWindow::disconnected()
{
    ui->label_state->setText("TCP状态：已断开");
}
void MainWindow::recvData()
{
    QByteArray array;
    array = socket->readAll();
    //qDebug() << "array.size:" << array.size();
    if(array.mid(0,5).operator==("start")){
        //qDebug() << "start";

        //说明这又是新的一帧数据，图像保存到显示数组上，清空图像缓冲区，重新接受数据
        show_array.clear();
        show_array.append(rece_array);
        rece_array.clear();
        ShowFarme();
        //去掉非图像信息--“start”
        array.remove(0,5);

        //qDebug() << "show_array.size:" << show_array.size();
    }
    else if(array.mid(0,7).operator==("ap3216c")){
        //qDebug() << "ap3216c:";
        //去掉非图像信息--ap3216c
        array.remove(0,7);
        unsigned short i = ByteArray4Toint(array);
        ui->label->setNum(i);
        array.clear();
        //qDebug() << "array:" << array.data();
    }
    rece_array.append(array);

}
unsigned short MainWindow::ByteArray4Toint(QByteArray ba)
{
    unsigned short i = 0;
    i |= ba.at(3) << 12;
    i |= ba.at(2) << 8;
    i |= ba.at(1) << 4;
    i |= ba.at(0) << 0;
    return i;
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::ShowFarme()
{
    imag.loadFromData(show_array,"JPG");//从show_array中读数据，类型为JPG
    ui->show_image->setPixmap(QPixmap::fromImage(imag));
}

void MainWindow::on_OpenLight_clicked()
{
    QByteArray array = "light on";
    socket->write(array,array.size());  //发送开灯
}

void MainWindow::on_OffLight_clicked()
{
    QByteArray array = "light off";
    socket->write(array,array.size());  //发送关灯
}
