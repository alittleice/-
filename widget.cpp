#include "widget.h"
#include "ui_widget.h"

#define serverip "192.168.43.115"
//#define serverip "192.168.1.226"
//#define serverip "192.168.1.131"
#define port     9000

int fd_ap3216c;     //光敏传感器文件句柄
int fd_led;         //LED灯文件句柄

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    //安装事件过滤器
    this->setAttribute(Qt::WA_AcceptTouchEvents);
    this->installEventFilter(this); //安装事件过滤器,这个安装在widget主窗口下

    //初始化外设
    fd_ap3216c = open("/dev/ap3216c", O_RDWR);   //打开光敏传感器
    if(fd_ap3216c < 0)
            printf("can't open file /dev/ap3216c\r\n");
    fd_led = open("/dev/dtsplatled", O_RDWR);   //打开光敏传感器
    if(fd_led < 0)
            printf("can't open file /dev/dtsplatled\r\n");

    //初始化套接字对象
    mSocketM = new QTcpSocket;
    ui->btndisconnect->setEnabled(false);   //断开按钮默认不能按下
    ui->state->setText("TCP状态：未连接");    //默认状态为断开

    //timer_send定时器在连接成功槽函数中打开
    connect(&timer_send, SIGNAL(timeout()), this, SLOT(SendFarme()));
    connect(mSocketM,SIGNAL(readyRead()),this,SLOT(recvData()));//连接接收槽函数

    //默认灯光为关闭
    write_led(0);
    ui->label->setText("light off");

    //摄像头初始化
    Initcamara();

}
//读取光敏传感器数据
unsigned short  Widget::read_ap3216c()
{
    int ret;
    unsigned short  als;
    unsigned short databuf[3];
    ret = read(fd_ap3216c, databuf, sizeof(databuf));
    if(ret == 0) { 			/* 数据读取成功 */
        als = databuf[1]; 	/* 光强传感器数据 */
        //printf("als = %d\r\n",als);
    }
    else
        return -1;
    return als;
}
//设置led灯开关状态
short Widget::write_led(unsigned short state)
{
    int ret;
    ret = write(fd_led, &state, sizeof(state));
    if(ret == 0) { 			/* 数据读取成功 */
        return 0;
    }
    else
        return -1;
}

void Widget::Initcamara()//初始化
{
#if 1
    //读取摄像头数据--方法二
    capture = new VideoCapture(0);

    connect(&timer_read, SIGNAL(timeout()), this, SLOT(ReadFarme()));
    timer_read.start(30);

    connect(&timer_show, SIGNAL(timeout()), this, SLOT(ShowFarme()));
    timer_show.start(40);


#endif
}

//事件过滤器
bool Widget::eventFilter(QObject *obj, QEvent *e)
{
    switch( e->type() )
    {
        case QEvent::MouseButtonRelease:
        {
            capture->release();             //触摸屏幕会导致图像错乱,所以松开时重新加载VideoCapture
            capture = new VideoCapture(0);
            return true;
        }
        default:
            return QWidget::eventFilter(obj,e);
        }
}

void Widget::ReadFarme()
{
    capture->read(mat_image);
    imag = cvMat2QImage(mat_image);
}

void Widget::ShowFarme()
{
    ui->image->setPixmap(QPixmap::fromImage(imag));
}


#if 1
void Widget::SendFarme()//这个函数属于Widget
{
    static int send_count = 0;
    if(send_count < 20) //20帧图发送一次传感器数据
    {
        QPixmap pixmap = QPixmap::fromImage(imag);  //把img转成位图，我们要转成jpg格式

        QByteArray ba;
        QBuffer buf(&ba); //把ba绑定到buf上，操作buf就等于操作ba 因为在内存资源很珍贵的情况下，会比较适合使用QByteArray
        pixmap.save(&buf,"jpg",50); //把pixmap保存成jpg，压缩质量50 数据保存到buf

        mSocketM->write("start");
        //mSocketM->write(ba);        //发送图像 ba保存着图像数据
        mSocketM->write(ba,ba.size());//发送指定大小？
    }
    else
    {
        unsigned short  als = read_ap3216c();
        QByteArray ba = intTo4ByteArray(als);
        mSocketM -> write("ap3216c");
        mSocketM -> write(ba);
        send_count = 0;
    }
    send_count++;

}
void Widget::recvData()
{
    QByteArray array;
    array = mSocketM->readAll();
    //判断开关灯状态
    if(array.operator==("light on")){
        write_led(1);
    }
    else if(array.operator==("light off")){
        write_led(0);
    }
    ui->label->setText(array);

}

QByteArray Widget::intTo4ByteArray(unsigned short i)
{
    QByteArray ba;
    ba.resize(4);
    ba[0] = (uchar)( 0x000f&i);
    ba[1] = (uchar)((0x00f0&i)>>4);
    ba[2] = (uchar)((0x0f00&i)>>8);
    ba[3] = (uchar)((0xf000&i)>>12);
    return ba;
}
#endif


#if 1
void Widget::on_btnconnect_clicked()    //连接按钮槽函数
{
    capture->release();             //触摸屏幕会导致图像错乱,所以松开时重新加载VideoCapture
    capture = new VideoCapture(0);

    //检测链接成功信号关联槽函数
    connect(mSocketM, SIGNAL(connected()), this, SLOT(connect_suc()));
    //检测掉线信号
    connect(mSocketM, SIGNAL(disconnected()), this, SLOT(client_dis()));

    //连接服务器，设置ip和端口号
    mSocketM->connectToHost(serverip, port);

}

void Widget::on_btndisconnect_clicked()    //断开按钮槽函数
{
    capture->release();             //触摸屏幕会导致图像错乱,所以松开时重新加载VideoCapture
    capture = new VideoCapture(0);

    mSocketM->close();
}

void Widget::connect_suc()    //连接成功槽函数
{
    ui->state->setText("TCP状态：已连接");
    ui->btndisconnect->setEnabled(true);    //断开按钮可以按下
    ui->btnconnect->setEnabled(false);      //连接按钮不能按下

    timer_send.start(40);                   //连接成功，开始发送图像
}

void Widget::client_dis()     //掉线槽函数
{
    ui->state->setText("TCP状态：已断开");
    ui->btndisconnect->setEnabled(false);   //断开按钮不能按下
    ui->btnconnect->setEnabled(true);       //连接按钮可以按下

    timer_send.stop();                   //连接断开，停止发送图像
}
#endif

QImage Widget::cvMat2QImage(const Mat& mat)    // Mat 转为 QImage
{
    if (mat.type() == CV_8UC1)					// 单通道
    {
        QImage image(mat.cols, mat.rows, QImage::Format_Indexed8);
        image.setColorCount(256);				// 灰度级数256
        for (int i = 0; i < 256; i++)
        {
            image.setColor(i, qRgb(i, i, i));
        }
        uchar *pSrc = mat.data;					// 复制mat数据
        for (int row = 0; row < mat.rows; row++)
        {
            uchar *pDest = image.scanLine(row);
            memcpy(pDest, pSrc, mat.cols);
            pSrc += mat.step;
        }
        return image;
    }

    else if (mat.type() == CV_8UC3)				// 3通道
    {
        const uchar *pSrc = (const uchar*)mat.data;			// 复制像素
        QImage image(pSrc, mat.cols, mat.rows, mat.step, QImage::Format_RGB888);	// R, G, B 对应 0,1,2
        return image.rgbSwapped().mirrored(true,true);				// rgbSwapped是为了显示效果色彩好一些。
    }
    else if (mat.type() == CV_8UC4)
    {
        const uchar *pSrc = (const uchar*)mat.data;			// 复制像素
                                                            // Create QImage with same dimensions as input Mat
        QImage image(pSrc, mat.cols, mat.rows, mat.step, QImage::Format_ARGB32);		// B,G,R,A 对应 0,1,2,3
        return image.copy();
    }
    else
    {
        return QImage();
    }
}

Widget::~Widget()
{
    delete ui;
    delete mSocketM;

    //退出子线程
    //thread->quit();
    //回收资源
    //thread->wait();
    //delete thread;
}


void Widget::on_send_clicked()
{
    capture->release();             //触摸屏幕会导致图像错乱,所以松开时重新加载VideoCapture
    capture = new VideoCapture(0);

//    QPixmap pixmap = QPixmap::fromImage(imag);  //把img转成位图，我们要转成jpg格式

//    QByteArray ba;
//    QBuffer buf(&ba); //把ba绑定到buf上，操作buf就等于操作ba 因为在内存资源很珍贵的情况下，会比较适合使用QByteArray
//    pixmap.save(&buf,"jpg",50); //把pixmap保存成jpg，压缩质量50 数据保存到buf

//    mSocketM->write("start");
//    mSocketM->write(ba);        //发送图像
    //mSocketM->write(ba,ba.size());//发送指定大小？


}


void Widget::on_btndisconnect_released()
{
    capture->release();
    capture = new VideoCapture(0);
}
