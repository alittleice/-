# Embedded-LinuxQT

1、增加了多线程采集图像

2、增加了另一种opencv采集图像的方法
//读取摄像头数据--方法二
    capture->read(mat_image);
    QImage imag = cvMat2QImage(mat_image);
    mat_image.release();

3、更换为v4l2框架采集图像
