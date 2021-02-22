#include "show_thread.h"
#include "mainwindow.h"

show_thread::show_thread(QObject *parent) : QThread(parent)
{

}

void show_thread::run()
{

    capture = new VideoCapture(0);
    timer_show = new QTimer();
    timer_show->setInterval(40);
    connect(timer_show, SIGNAL(timeout()), this, SLOT(ShowFarme()), Qt::QueuedConnection);
    timer_show->start();

    this->exec();   //事件循环exec() 线程不能停止，因为要一直采集图像
}

void show_thread::ShowFarme()//show_thread
{
    //读取摄像头数据--方法二
    capture->read(mat_image);
    QImage imag = cvMat2QImage(mat_image);
    mat_image.release();
    static int count = 0;
    emit signal_Done(imag, count++);
}

QImage show_thread::cvMat2QImage(const Mat& mat)    // Mat 转为 QImage
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

