# SE342-Computer-Vision
Course Project——Lumir。
## Introduction
### Environment
MacOS 10.13.2，Qt Creator 4.4.1。
### Compile
```
qmake src/demo.pro -spec macx-clang CONFIG+=debug CONFIG+=x86_64 CONFIG+=qml_debug && /usr/bin/make qmake_all
make in build
```
### Demo
![Picture](https://github.com/JosephKim6/SE342-Computer-Vision/blob/master/demo.png)
### Feature

* RGB channel seperation
* RGB to GRAY
* Hue/Saturation/Value adjustment
* Binarization (Otsu algorithm or double threshold)
* Rotation/Scaling (nearest neighbor interpolation or bilinear interpolation)
* Image add/minus/multiply operation
* Clipping
* Contrast adjustment (piecewise linear, exponent, logarithm or histogram equalization)
* Smoothing filter (mean filter, gaussian filter or median filter)
* Edge detection (Sobel operator, Laplace operator or Canny operator)
* Hough transform (detect line or circle)
* Binary morphology (dilation, erosion, open, close, thinning, thickening)
* Distance transform
* Skeleton extraction and reconstruction
* Grayscale morphology (dilation, erosion, open, close)
* Watershed algorithm
 
