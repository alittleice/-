#include "mainwindow.h"
#include "ui_mainwindow.h"

//v4l2采集程序demo
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/videodev2.h>
#include <string.h>
#include <sys/mman.h>

#define camera_w  640
#define camera_h  480

#define serverip "192.168.0.111"
#define port     8888

void yuyv_to_rgb(unsigned char *yuyvdata, unsigned char *rgbdata, int w, int h)
{
    //码流Y0 U0 Y1 V1 Y2 U2 Y3 V3 --》YUYV像素[Y0 U0 V1] [Y1 U0 V1] [Y2 U2 V3] [Y3 U2 V3]--》RGB像素
    int r1, g1, b1;
    int r2, g2, b2;
    for(int i = 0; i<w*h/2; i++)
    {
        char data[4];
        memcpy(data, yuyvdata+i*4, 4);
        unsigned char Y0=data[0];
        unsigned char U0=data[1];
        unsigned char Y1=data[2];
        unsigned char V1=data[3];
        //Y0U0Y1V1  -->[Y0 U0 V1] [Y1 U0 V1]
        r1 = Y0+1.4075*(V1-128); if(r1>255)r1=255; if(r1<0)r1=0;
        g1 =Y0- 0.3455 * (U0-128) - 0.7169*(V1-128); if(g1>255)g1=255; if(g1<0)g1=0;
        b1 = Y0 + 1.779 * (U0-128);  if(b1>255)b1=255; if(b1<0)b1=0;

        r2 = Y1+1.4075*(V1-128);if(r2>255)r2=255; if(r2<0)r2=0;
        g2 = Y1- 0.3455 * (U0-128) - 0.7169*(V1-128); if(g2>255)g2=255; if(g2<0)g2=0;
        b2 = Y1 + 1.779 * (U0-128);  if(b2>255)b2=255; if(b2<0)b2=0;

        rgbdata[i*6+0]=r1;
        rgbdata[i*6+1]=g1;
        rgbdata[i*6+2]=b1;
        rgbdata[i*6+3]=r2;
        rgbdata[i*6+4]=g2;
        rgbdata[i*6+5]=b2;
    }
}

int fd = open("/dev/video0", O_RDWR);
unsigned char *mptr[4];//保存映射后用户空间的首地址
void MainWindow::Initcamara()//初始化
{
    //1.打开设备

    if(fd < 0)
    {
        perror("error:打开设备失败");
    }
    //3.设置采集格式
    struct v4l2_format vfmt;
    vfmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;//摄像头采集
    vfmt.fmt.pix.width = camera_w;//设置宽（不能任意）
    vfmt.fmt.pix.height = camera_h;//设置高
    vfmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;//设置视频采集格式
    vfmt.fmt.pix.field = V4L2_FIELD_INTERLACED;

    int ret = ioctl(fd, VIDIOC_S_FMT, &vfmt);
    if(ret < 0)
    {
        perror("error:设置格式失败");
    }

    memset(&vfmt, 0, sizeof(vfmt));
    vfmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    ret  = ioctl(fd, VIDIOC_G_FMT, &vfmt);
    if(ret < 0)
    {
        perror("获取格式失败");
    }

    if(vfmt.fmt.pix.width == camera_w && vfmt.fmt.pix.height == camera_h)
    {
        printf("格式设置成功\n");
    }else
    {
        printf("格式设置失败\n");
    }

    //4.申请内核空间
    struct v4l2_requestbuffers reqbuffer;
    reqbuffer.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    reqbuffer.count = 4; //申请4个缓冲区
    reqbuffer.memory = V4L2_MEMORY_MMAP ;//映射方式
    ret  = ioctl(fd, VIDIOC_REQBUFS, &reqbuffer);
    if(ret < 0)
    {
        perror("error5:申请队列空间失败");
    }

    //5.映射
    unsigned int  size[4];
    struct v4l2_buffer mapbuffer;
    //初始化type, index
    mapbuffer.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    int i = 0;
    for(i =0; i<4; i++)
    {
        mapbuffer.index = i;
        ret = ioctl(fd, VIDIOC_QUERYBUF, &mapbuffer);//从内核空间中查询一个空间做映射
        if(ret < 0)
        {
            perror("error6:查询内核空间队列失败");
        }
        mptr[i] = (unsigned char *)mmap(NULL, mapbuffer.length, PROT_READ|PROT_WRITE,MAP_SHARED, fd, mapbuffer.m.offset);
        size[i]=mapbuffer.length;

        //通知使用完毕--‘放回去’
        ret  = ioctl(fd, VIDIOC_QBUF, &mapbuffer);
        if(ret < 0)
        {
            perror("error7:放回失败");
        }
    }

    //6.开始采集
    int type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    ret = ioctl(fd, VIDIOC_STREAMON, &type);
    if(ret < 0)
    {
        perror("开启失败");
    }

    //定义一个空间解码后的RGB数组
    unsigned char rgbdata[camera_w*camera_h*3];
    int j = 10;
    while(j--)      //循环采集数据
    {
        //7.从队列中提取一帧数据
        struct v4l2_buffer  readbuffer;
        readbuffer.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        ret = ioctl(fd, VIDIOC_DQBUF, &readbuffer);
        if(ret < 0)
        {
            perror("提取数据失败");
        }
        yuyv_to_rgb(mptr[readbuffer.index], rgbdata, camera_w, camera_h);

        //通知内核已经使用完毕
        ret = ioctl(fd, VIDIOC_QBUF, &readbuffer);
        if(ret < 0)
        {
            perror("放回队列失败");
        }

        imag = QImage(rgbdata,camera_w,camera_h,QImage::Format_RGB888);//.mirrored(true,true);
        ui->image->setPixmap(QPixmap::fromImage(imag));
    }


//    //8.停止采集
//    ret = ioctl(fd, VIDIOC_STREAMOFF, &type);
//    //9.释放映射
//    for(i=0; i<4; i++)
//        munmap(mptr[i], size[i]);
//    //10.关闭设备
//    //close(fd);
}
void MainWindow::ShowFarme()//show_thread
{
    int type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    int ret = ioctl(fd, VIDIOC_STREAMON, &type);
    //定义一个空间解码后的RGB数组
    unsigned char rgbdata[camera_w*camera_h*3];

    //7.从队列中提取一帧数据
    struct v4l2_buffer  readbuffer;
    readbuffer.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    ret = ioctl(fd, VIDIOC_DQBUF, &readbuffer);
    if(ret < 0)
    {
        perror("提取数据失败");
    }
    yuyv_to_rgb(mptr[readbuffer.index], rgbdata, camera_w, camera_h);

    //通知内核已经使用完毕
    ret = ioctl(fd, VIDIOC_QBUF, &readbuffer);
    if(ret < 0)
    {
        perror("放回队列失败");
    }

    //在ui界面显示
    imag = QImage(rgbdata,camera_w,camera_h,QImage::Format_RGB888);//.mirrored(true,true);
    ui->image->setPixmap(QPixmap::fromImage(imag));
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //摄像头初始化
    Initcamara();

    connect(&timer_show, SIGNAL(timeout()), this, SLOT(ShowFarme()));
    timer_show.start(10);
    //分配线程空间
    //thread = new show_thread(this);         //给对象thread 分配一个空间
    //启动线程，处理数据
    //connect(thread, &show_thread::signal_Done, this, &MainWindow::dealDone);
    //thread->start();

//    //初始化套接字对象
//    mSocketM = new QTcpSocket;
//    ui->btndisconnect->setEnabled(false);   //断开按钮默认不能按下
    ui->state->setText("TCP状态：已连接");         //默认状态为断开

}

void MainWindow::dealDone() //线程槽函数
{
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

}




