#include "mainwindow.h"
#include "ui_mainwindow.h"

#define serverip "192.168.0.111"
#define port     8888

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //摄像头初始化
    //Initcamara();

    //分配线程空间
    thread = new show_thread(this);         //给对象thread 分配一个空间
//    timer_show = new QTimer();
//    timer_show->moveToThread(thread);    //将该定时器放在线程中
//    timer_show->setInterval(40);         //定时器间隔时间
//    connect(thread, SIGNAL(started()), timer_show, SLOT(start()));  //通过线程开启来触发定时器开启
//    connect(timer_show, &QTimer::timeout, this, &show_thread::ShowFarme, Qt::DirectConnection);//定时器信号发出后立即调用槽函数，槽函数在发出信号的线程中执行
//    connect(thread, &QThread::finished, this, &QObject::deleteLater);//线程结束，自动关闭线程
//    //启动线程，处理数据
    connect(thread, &show_thread::signal_Done, this, &MainWindow::dealDone);
    thread->start();

//    //初始化套接字对象
//    mSocketM = new QTcpSocket;
//    ui->btndisconnect->setEnabled(false);   //断开按钮默认不能按下
    ui->state->setText("TCP状态：已连接");         //默认状态为断开

    //sleep(10);
}

void MainWindow::dealDone(QImage imag, int count) //线程槽函数
{
    //ui->state->setText("TCP状态：OK");         //默认状态为断开
    ui->image->setPixmap(QPixmap::fromImage(imag));
    ui->lcdNumber->display(count);
}

void MainWindow::Initcamara()//初始化
{
#if 1
    //打开摄像头,采集原始图像默认640*480
    //camera = cvCreateCameraCapture(0);

    //读取摄像头数据--方法二
//    capture = new VideoCapture(0);

//    QTimer *timer_show = new QTimer();
//    timer_show->moveToThread(thread);    //将该定时器放在线程中
//    timer_show->setInterval(40);         //定时器间隔时间
//    connect(thread, SIGNAL(started()), timer_show, SLOT(start()));  //通过线程开启来触发定时器开启
//    connect(timer_show, SIGNAL(timeout()), this, SLOT(ShowFarme()), Qt::DirectConnection);//定时器信号发出后立即调用槽函数，槽函数在发出信号的线程中执行
//    connect(thread, &QThread::finished, this, &QObject::deleteLater);//线程结束，自动关闭线程
//    //启动线程，处理数据
//    thread->start(QThread::InheritPriority);



//    timer_read.start(40);
//    connect(&timer_read, SIGNAL(timeout()), this, SLOT(ReadFarme()));
//    timer_show->start(40);
//    connect(timer_show, SIGNAL(timeout()), this, SLOT(ShowFarme()));

//    connect(&timer_send, SIGNAL(timeout()), this, SLOT(SendFarme()));
#endif


}

void MainWindow::ReadFarme()//这个函数属于Widget
{
    //从摄像头读取一张图片
    //这个函数cVQueryFrame作用是从摄像头或者文件中抓取一帧
    //参数为视频获取结构也就是我在初始化摄像头声明的CVCapture*camera；
//    frame=cvQueryFrame(camera);
//    cvFlip(frame,NULL,-1);  //沿X-轴和Y-轴翻转（即关于原点对称）使图像显示正常


}



#if 0
void MainWindow::SendFarme()//这个函数属于Widget
{
    frame=cvQueryFrame(camera);
    cvFlip(frame,NULL,-1);

    imag = QImage((const uchar*)frame->imageData,
                frame->width,frame->height,
                QImage::Format_RGB888).rgbSwapped();//.mirrored(true,true);

    QPixmap pixmap = QPixmap::fromImage(imag);  //把img转成位图，我们要转成jpg格式

//    QByteArray ba;
//    QBuffer buf(&ba); //把ba绑定到buf上，操作buf就等于操作ba 因为在内存资源很珍贵的情况下，会比较适合使用QByteArray
//    pixmap.save(&buf,"jpg",50); //把pixmap保存成jpg，压缩质量50 数据保存到buf

//    mSocketM->write("start");
//    mSocketM->write(ba);        //发送图像
    //len = m_tcpSocket->write(buf,len);//发送指定大小？
}
#endif

MainWindow::~MainWindow()
{
    delete ui;
    //delete mSocketM;

    //退出子线程
    thread->quit();
    //回收资源
    thread->wait();
    delete thread;
}

#if 0
void MainWindow::on_btnconnect_clicked()    //连接服务端槽函数
{
    //检测链接成功信号关联槽函数
    connect(mSocketM, SIGNAL(connected()), this, SLOT(connect_suc()));
    //检测掉线信号
    connect(mSocketM, SIGNAL(disconnected()), this, SLOT(client_dis()));
    //连接服务器，设置ip和端口号
    mSocketM->connectToHost(serverip, port);
}

void MainWindow::on_btndisconnect_clicked()    //断开服务端槽函数
{
    mSocketM->close();
}

void MainWindow::connect_suc()    //连接成功槽函数
{
    ui->state->setText("状态:已连接");
    ui->btndisconnect->setEnabled(true);    //断开按钮可以按下
    ui->btnconnect->setEnabled(false);      //连接按钮不能按下

    timer_send.start(30);                   //连接成功，开始发送图像

}

void MainWindow::client_dis()     //掉线槽函数
{
    ui->state->setText("状态：已断开");
    ui->btndisconnect->setEnabled(false);   //断开按钮不能按下
    ui->btnconnect->setEnabled(true);       //连接按钮可以按下

    timer_send.stop();                   //连接断开，停止发送图像
}
#endif
void MainWindow::on_send_clicked()
{
#if 0
    frame=cvQueryFrame(camera);
    cvFlip(frame,NULL,-1);

    imag = QImage((const uchar*)frame->imageData,
                frame->width,frame->height,
                QImage::Format_RGB888).rgbSwapped();//.mirrored(true,true);
    ui->image->setPixmap(QPixmap::fromImage(imag));

    //QByteArray dataArray = frame->imageData;
    //ui->label->setNum(frame->imageSize);
    //ui->label_2->setNum(dataArray.size());
    ui->label_2->setNum(frame->width);
    ui->label_4->setNum(frame->height);


    QByteArray dataArray = frame->imageData;
    //将图片格式转换成Qlmage格式，否则不能再lable上显示
    imag = QImage((const uchar*)frame->imageData,
                frame->width,frame->height,
                QImage::Format_RGB888).rgbSwapped();//.mirrored(true,true);

    //用label显示一张图片
    ui->image->setPixmap(QPixmap::fromImage(imag));

    QPixmap pixmap = QPixmap::fromImage(imag);  //把img转成位图，我们要转成jpg格式
    QByteArray ba;
    QBuffer buf(&ba); //把ba绑定到buf上，操作buf就等于操作ba 因为在内存资源很珍贵的情况下，会比较适合使用QByteArray
    pixmap.save(&buf,"jpg",50); //把pixmap保存成jpg，压缩质量50 数据保存到buf

    ui->label_2->setNum(ba.size());
    ui->label_4->setNum(dataArray.size());

    mSocketM->write("start");
    mSocketM->write(ba);//(frame->imageData);
#endif


}




