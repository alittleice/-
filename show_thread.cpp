#include "show_thread.h"
#include "mainwindow.h"

show_thread::show_thread(QObject *parent) : QThread(parent)
{

}

void show_thread::run()
{
    timer_show = new QTimer();
    timer_show->setInterval(40);
    connect(timer_show, SIGNAL(timeout()), this, SLOT(ShowFarme()), Qt::QueuedConnection);
    timer_show->start();

    this->exec();   //事件循环exec() 线程不能停止，因为要一直采集图像
}

void show_thread::ShowFarme()//show_thread
{

}
