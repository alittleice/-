#include "show_thread.h"

show_thread::show_thread(QObject *parent) : QThread(parent)
{

}

void show_thread::run()
{
    QTimer timer_show;
    //很复杂的数据处理
    connect(&timer_show, SIGNAL(timeout()), this, SLOT(ShowFarme()));
    timer_show.start(40);
    this->exec();   //事件循环exec()
}


