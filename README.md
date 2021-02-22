# Embedded-LinuxQT

1、增加了多线程采集图像

2、增加了另一种opencv采集图像的方法
//读取摄像头数据--方法二
    capture->read(mat_image);
    QImage imag = cvMat2QImage(mat_image);
    mat_image.release();

3、更换为v4l2框架采集图像

4、
2021/02/22 22.05
图像分屏问题总结：
与单线程干扰无关，这个问题可能是触摸事件干扰了采集图像的缓冲区导致的
解决方法：在鼠标释放事件里加上“关闭-开启VideoCapture的动作即可解决
取消了双线程
