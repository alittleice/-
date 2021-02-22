#include "widget.h"
#include "ui_widget.h"

#define serverip "192.168.0.111"
#define port     8888

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    this->setAttribute(Qt::WA_AcceptTouchEvents);
    this->installEventFilter(this); //安装事件过滤器,这个安装在widget主窗口下

    //摄像头初始化
    Initcamara();

    //分配线程空间
    //thread = new show_thread(this);         //给对象thread 分配一个空间
    //启动线程，处理数据
    //connect(thread, &show_thread::signal_Done, this, &MainWindow::dealDone);
    //thread->start();

    //初始化套接字对象
    mSocketM = new QTcpSocket;
    ui->btndisconnect->setEnabled(false);   //断开按钮默认不能按下
    ui->state->setText("TCP状态：未连接");    //默认状态为断开

}
void Widget::dealDone(QImage imag, int count) //线程槽函数
{
    //ui->state->setText("TCP状态：OK");         //默认状态为断开
    ui->image->setPixmap(QPixmap::fromImage(imag));
    ui->lcdNumber->display(count);
}

void Widget::Initcamara()//初始化
{
#if 1
    //读取摄像头数据--方法二
    capture = new VideoCapture(0);

    timer_show.start(40);
    connect(&timer_show, SIGNAL(timeout()), this, SLOT(ShowFarme()));

    //timer_send定时器在连接成功槽函数中打开
    connect(&timer_send, SIGNAL(timeout()), this, SLOT(SendFarme()));
#endif
}

//事件过滤器
bool Widget::eventFilter(QObject *obj, QEvent *e)
{
    switch( e->type() )
    {
        case QEvent::TouchBegin:
        {
            ui->label->setText("TouchBegin");
            printf("TouchBegin\n");
            return true;
        }

        case QEvent::TouchUpdate:
        {
            ui->label->setText("TouchUpdate");
            printf("TouchUpdate\n");
            return true;
        }
        case QEvent::TouchEnd:
        {
            ui->label->setText("TouchEnd");
            printf("TouchEnd\n");
            return true;
        }
        case QEvent::MouseButtonPress:
        {
            ui->label->setText("MouseButtonPress");
            return true;
        }
        case QEvent::MouseButtonRelease:
        {
            capture->release();             //触摸屏幕会导致图像错乱,所以松开时重新加载VideoCapture
            capture = new VideoCapture(0);
            ui->label->setText("MouseButtonRelease");
            return true;
        }
        default:
            return QWidget::eventFilter(obj,e);
        }
}

void Widget::ShowFarme()
{
    capture->read(mat_image);
    imag = cvMat2QImage(mat_image);
    ui->image->setPixmap(QPixmap::fromImage(imag));
}


#if 1
void Widget::SendFarme()//这个函数属于Widget
{
    QPixmap pixmap = QPixmap::fromImage(imag);  //把img转成位图，我们要转成jpg格式

    QByteArray ba;
    QBuffer buf(&ba); //把ba绑定到buf上，操作buf就等于操作ba 因为在内存资源很珍贵的情况下，会比较适合使用QByteArray
    pixmap.save(&buf,"jpg",50); //把pixmap保存成jpg，压缩质量50 数据保存到buf

    mSocketM->write("start");
    mSocketM->write(ba);        //发送图像
    //len = m_tcpSocket->write(buf,len);//发送指定大小？
}
#endif


#if 1
void Widget::on_btnconnect_clicked()    //连接服务端槽函数
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

void Widget::on_btndisconnect_clicked()    //断开服务端槽函数
{
    capture->release();             //触摸屏幕会导致图像错乱,所以松开时重新加载VideoCapture
    capture = new VideoCapture(0);

    mSocketM->close();
}

void Widget::connect_suc()    //连接成功槽函数
{
    ui->state->setText("状态:已连接");
    ui->btndisconnect->setEnabled(true);    //断开按钮可以按下
    ui->btnconnect->setEnabled(false);      //连接按钮不能按下

    timer_send.start(30);                   //连接成功，开始发送图像

}

void Widget::client_dis()     //掉线槽函数
{
    ui->state->setText("状态：已断开");
    ui->btndisconnect->setEnabled(false);   //断开按钮不能按下
    ui->btnconnect->setEnabled(true);       //连接按钮可以按下

    timer_send.stop();                   //连接断开，停止发送图像
}
#endif
//void Widget::on_send_clicked()
//{
//    ui->state->setText("点击了发送");
//}

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
        return image.rgbSwapped();				// rgbSwapped是为了显示效果色彩好一些。
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
}


void Widget::on_btndisconnect_released()
{
    capture->release();
    capture = new VideoCapture(0);
}
