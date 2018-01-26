# SE342-Computer-Vision
计算机视觉大作业——Lumir。
## 简介
### 环境
MacOS 10.13.2，Qt Creator 4.4.1。
### 编译
```
qmake src/demo.pro -spec macx-clang CONFIG+=debug CONFIG+=x86_64 CONFIG+=qml_debug && /usr/bin/make qmake_all
make in build
```
### 功能
1. 打开，保存图片。
2.	撤销/恢复 功能。
3.	三通道分离
4.	彩色变灰度
5.	色相/饱和度/亮度调节
6.	二值化（Otsu，双阈值，实时）
7.	缩放/旋转（最近邻，双线性）
8.	加/减/乘
9.	剪裁
10.	对比度调节（分段线性，指数，对数，直方图均衡化）
11.	平滑滤波（均值，高斯，中值；自定义卷积核大小）
12.	边缘检测（sobel，laplace，canny）
13.	霍夫变换（直线，圆)
14.	二值形态学（结构元自定义，膨胀，腐蚀，开，闭，细化，粗化，距离变换，骨架，骨架重构，形态学重构）。
15.	灰度形态学（膨胀，腐蚀，开，闭，形态学重构，分水岭算法）
 
