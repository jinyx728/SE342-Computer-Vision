#include "cvAlgorithms.h"
#include "WaterShedAlgorithm.h"
#include <cmath>
#include <QtDebug>
#include <algorithm>
#include <queue>
#include <vector>

namespace Algo
{
QImage cvMat2QImage(const cv::Mat& mat)
{
    // 8-bits unsigned, NO. OF CHANNELS = 1
    if(mat.type() == CV_8UC1)
    {
        QImage image(mat.cols, mat.rows, QImage::Format_Indexed8);
        // Set the color table (used to translate colour indexes to qRgb values)
        image.setColorCount(256);
        for(int i = 0; i < 256; i++)
        {
            image.setColor(i, qRgb(i, i, i));
        }
        // Copy input Mat
        uchar *pSrc = mat.data;
        for(int row = 0; row < mat.rows; row ++)
        {
            uchar *pDest = image.scanLine(row);
            memcpy(pDest, pSrc, mat.cols);
            pSrc += mat.step;
        }
        return image;
    }
    // 8-bits unsigned, NO. OF CHANNELS = 3
    else if(mat.type() == CV_8UC3)
    {
        // Copy input Mat
        const uchar *pSrc = (const uchar*)mat.data;
        // Create QImage with same dimensions as input Mat
        QImage image(pSrc, mat.cols, mat.rows, mat.step, QImage::Format_RGB888);
        return image.rgbSwapped();
    }
    else if(mat.type() == CV_8UC4)
    {
        // Copy input Mat
        const uchar *pSrc = (const uchar*)mat.data;
        // Create QImage with same dimensions as input Mat
        QImage image(pSrc, mat.cols, mat.rows, mat.step, QImage::Format_ARGB32);
        return image.copy();
    }
    else
    {
        qDebug() << "ERROR: Mat could not be converted to QImage.";
        return QImage();
    }
}

cv::Mat QImage2cvMat(QImage image)
{
    cv::Mat mat;
    switch(image.format())
    {
    case QImage::Format_ARGB32:
    case QImage::Format_RGB32:
    case QImage::Format_ARGB32_Premultiplied:
        mat = cv::Mat(image.height(), image.width(), CV_8UC4, (void*)image.constBits(), image.bytesPerLine());
        break;
    case QImage::Format_RGB888:
        mat = cv::Mat(image.height(), image.width(), CV_8UC3, (void*)image.constBits(), image.bytesPerLine());
        cv::cvtColor(mat, mat, CV_BGR2RGB);
        break;
    case QImage::Format_Indexed8:
    case QImage::Format_Grayscale8:
        mat = cv::Mat(image.height(), image.width(), CV_8UC1, (void*)image.constBits(), image.bytesPerLine());
        break;
    default:
        break;
    }
    return mat;
}

void split(const cv::Mat& src, std::vector<cv::Mat> &dst)
{
    uchar* pSrc = (uchar*)src.data;
    int cn = src.channels();
    int len = src.rows * src.cols;
    for (int i = 0; i < cn; i++) {
        dst.push_back(cv::Mat(src.rows, src.cols, CV_8UC1));
    }
    for (int i = 0; i < cn; i++) {
        uchar* pDst = (uchar*)dst[i].data;
        for (int j = 0; j < len; j++) {
            pDst[j] = pSrc[j * cn + i];
        }
    }
}

cv::Mat toGray(const cv::Mat& mat)
{
    std::vector<cv::Mat> channels;
    split(mat, channels);
    cv::Mat gray = channels[2] * 0.299 + channels[1] * 0.587 + channels[0] * 0.114;
    return gray;
}

cv::Mat otsu(const cv::Mat& mat)
{
    cv::Mat result = cv::Mat::zeros(mat.rows, mat.cols, CV_8UC1);
    int threshold = getOtsuThreshold(mat);
    for (int i = 0; i < mat.rows; i++) {
        for (int j = 0; j < mat.cols; j++) {
            if (mat.at<uchar>(i,j) > threshold) {
                result.at<uchar>(i,j) = 255;
            }
            else {
                result.at<uchar>(i,j) = 0;
            }
        }
    }
    return result;
}

int getOtsuThreshold(const cv::Mat &mat)
{
    int threshold = 0;
    int nSumPix[256];
    double nProDis[256];
    for (int i = 0; i < 256; i++) {
        nSumPix[i] = 0;
        nProDis[i] = 0;
    }
    for (int i = 0; i < mat.rows; i++) {
        for (int j = 0; j < mat.cols; j++) {
            nSumPix[(int)mat.at<uchar>(i,j)]++;
        }
    }
    for (int i = 0; i < 256; i++) {
        nProDis[i] = (double)nSumPix[i] / (mat.rows * mat.cols);
    }
    double wb, wf;
    double u0_temp, u1_temp, u0, u1;
    double delta_temp;
    double delta_max = 0.0;
    for (int i = 0; i < 256; i++) {
        wb = wf = u0_temp = u1_temp = u0 = u1 = delta_temp = 0;
        for (int j = 0; j < 256; j++) {
            if (j <= i) {
                wb += nProDis[j];
                u0_temp += j * nProDis[j];
            }
            else {
                wf += nProDis[i];
                u1_temp += j * nProDis[j];
            }
        }
        u0 = u0_temp / wb;
        u1 = u1_temp / wf;
        delta_temp = (double)(wb * wf * pow((u0 - u1), 2));
        if (delta_temp > delta_max) {
            delta_max = delta_temp;
            threshold = i;
        }
    }
    return threshold;
}

bool isBinary(const QImage image)
{
    if (!image.isGrayscale()) {
        return false;
    }
    cv::Mat matrix = QImage2cvMat(image);
    for (int i = 0; i < matrix.rows; i++) {
        for (int j = 0; j < matrix.cols; j++) {
            if ((int)matrix.at<uchar>(i,j) != 0 && (int)matrix.at<uchar>(i,j) != 255) {
                return false;
            }
        }
    }
    return true;
}

cv::Mat zoomNearest(const cv::Mat& mat, int zoomRate)
{
    double scale = (double)zoomRate / 100;
    cv::Mat dst = cv::Mat(cv::Size(mat.cols * scale, mat.rows * scale), mat.type(), cv::Scalar::all(0));
    for (int i = 0; i < dst.cols; i++) {
        int sx = cvFloor((double)i / scale);
        sx = std::min(sx, mat.cols - 1);
        for (int j = 0; j < dst.rows; j++) {
            int sy = cvFloor((double)j / scale);
            sy = std::min(sy, mat.rows - 1);
            if (mat.type() == CV_8UC1) {
                dst.at<uchar>(j,i) = mat.at<uchar>(sy,sx);
            }
            else if (mat.type() == CV_8UC4){
                dst.at<cv::Vec4b>(j,i) = mat.at<cv::Vec4b>(sy,sx);
            }
            else {
                dst.at<cv::Vec3b>(j,i) = mat.at<cv::Vec3b>(sy,sx);
            }
        }
    }
    return dst;
}

cv::Mat zoomLinear(const cv::Mat& mat, int zoomRate)
{
    double scale = (double)zoomRate / 100;
    cv::Mat dst = cv::Mat(cv::Size(mat.cols * scale, mat.rows * scale), mat.type(), cv::Scalar::all(0));
    uchar *dataDst = dst.data;
    int stepDst = dst.step;
    uchar *dataSrc = mat.data;
    int stepSrc = mat.step;
    int widthSrc = mat.cols;
    int heightSrc = mat.rows;
    for (int j = 0; j < dst.rows; ++j) {
        float fy = (float)((j + 0.5) / scale - 0.5);
        int sy = cvFloor(fy);
        fy -= sy;
        sy = std::min(sy, heightSrc - 2);
        sy = std::max(0, sy);

        short cbufy[2];
        cbufy[0] = cv::saturate_cast<short>((1.f - fy) * 2048);
        cbufy[1] = 2048 - cbufy[0];

        for (int i = 0; i < dst.cols; ++i) {
            float fx = (float)((i + 0.5) / scale - 0.5);
            int sx = cvFloor(fx);
            fx -= sx;

            if (sx < 0) {
                fx = 0;
                sx = 0;
            }
            if (sx >= widthSrc - 1) {
                fx = 0;
                sx = widthSrc - 2;
            }

            short cbufx[2];
            cbufx[0] = cv::saturate_cast<short>((1.f - fx) * 2048);
            cbufx[1] = 2048 - cbufx[0];

            int channels = mat.channels();
            for (int k = 0; k < channels; k++) {
                *(dataDst+ j*stepDst + channels*i + k) = (
                        *(dataSrc + sy*stepSrc + channels*sx + k) * cbufx[0] * cbufy[0] +
                        *(dataSrc + (sy+1)*stepSrc + channels*sx + k) * cbufx[0] * cbufy[1] +
                        *(dataSrc + sy*stepSrc + channels*(sx+1) + k) * cbufx[1] * cbufy[0] +
                        *(dataSrc + (sy+1)*stepSrc + channels*(sx+1) + k) * cbufx[1] * cbufy[1]
                        ) >> 22;

            }
        }
    }

    return dst;
}

cv::Mat rotateNearest(const cv::Mat &mat, int rotateRate)
{
    float anglePI = rotateRate * CV_PI / 180;
    float absAngle = -abs(CV_PI/2 - (fmod(anglePI + CV_PI, CV_PI))) + CV_PI/2;
    int newCol = mat.cols * cos(absAngle) + mat.rows * sin(absAngle);
    int newRow = mat.rows * cos(absAngle) + mat.cols * sin(absAngle);
    cv::Mat dst = cv::Mat(cv::Size(newCol, newRow), mat.type(), cv::Scalar::all(0));
    int xSm, ySm;
    for (int i = 0; i < dst.rows; i++) {
        for (int j = 0; j < dst.cols; j++) {
            xSm = (int)((i - dst.rows / 2) * cos(anglePI) - (j - dst.cols / 2) * sin(anglePI) + 0.5);
            ySm = (int)((i - dst.rows / 2) * sin(anglePI) + (j - dst.cols / 2) * cos(anglePI) + 0.5);
            xSm += mat.rows / 2;
            ySm += mat.cols / 2;

            if (xSm >= mat.rows || ySm >= mat.cols || xSm <= 0 || ySm <= 0) {
                if (mat.type() == CV_8UC1) {
                    dst.at<uchar>(i,j) = 0;
                }
                else if (mat.type() == CV_8UC4){
                    dst.at<cv::Vec4b>(i,j) = cv::Vec4b(0, 0);
                }
                else {
                    dst.at<cv::Vec3b>(i,j) = cv::Vec3b(0, 0);
                }
            }
            else {
                if (mat.type() == CV_8UC1) {
                    dst.at<uchar>(i,j) = mat.at<uchar>(xSm, ySm);
                }
                else if (mat.type() == CV_8UC4){
                    dst.at<cv::Vec4b>(i,j) = mat.at<cv::Vec4b>(xSm, ySm);
                }
                else {
                    dst.at<cv::Vec3b>(i,j) = mat.at<cv::Vec3b>(xSm, ySm);
                }
            }
        }
    }
    return dst;
}

cv::Mat rotateLinear(const cv::Mat &mat, int rotateRate)
{
    float anglePI = rotateRate * CV_PI / 180;
    float absAngle = -abs(CV_PI/2 - (fmod(anglePI + CV_PI, CV_PI))) + CV_PI/2;
    int newCol = mat.cols * cos(absAngle) + mat.rows * sin(absAngle);
    int newRow = mat.rows * cos(absAngle) + mat.cols * sin(absAngle);
    cv::Mat dst = cv::Mat(cv::Size(newCol, newRow), mat.type(), cv::Scalar::all(0));
    uchar *dataDst = dst.data;
    int stepDst = dst.step;
    uchar *dataSrc = mat.data;
    int stepSrc = mat.step;
    int widthSrc = mat.cols;
    int heightSrc = mat.rows;


    for (int i = 0; i < newRow; i++) {
        for (int j = 0; j < newCol; j++) {
            float sx = (i - newRow / 2) * cos(anglePI) - (j - newCol / 2) * sin(anglePI) + mat.rows / 2;
            int x = cvFloor(sx);
            float u = sx - x;
            float sy = (i - newRow / 2) * sin(anglePI) + (j - newCol / 2) * cos(anglePI) + mat.cols / 2;
            int y = cvFloor(sy);
            float v = sy - y;
            if (x >= heightSrc - 1 && x < heightSrc) {
                x = heightSrc - 2;
                u = 0;
            }
            if (y >= widthSrc - 1 && y < widthSrc) {
                y = widthSrc - 2;
                v = 0;
            }
            int channels = mat.channels();
            if (x >= 0 && x < heightSrc && y >= 0 && y < widthSrc) {
                for (int k = 0; k < channels; k++) {
                    *(dataDst+ i*stepDst + channels*j + k) = (1-u)*(1-v)*(*(dataSrc + x*stepSrc + y*channels+k))
                            + (1-v)*u*(*(dataSrc+(x+1)*(stepSrc)+y*channels+k))
                            + v*(1-u)*(*(dataSrc+x*stepSrc+(y+1)*channels+k))
                            + u*v*(*(dataSrc+(x+1)*stepSrc+(y+1)*channels+k));
                }
            }
        }
    }
    return dst;
}

cv::Mat plus(cv::Mat &ori, const cv::Mat& add, int x, int y)
{
    int xMax = std::min(x + add.cols, ori.cols);
    int yMax = std::min(y + add.rows, ori.rows);
    for (int i = y; i < yMax; i++) {
        for (int j = x; j < xMax; j++) {
            if (ori.type() == CV_8UC4) {
                ori.at<cv::Vec4b>(i,j) = ori.at<cv::Vec4b>(i,j) / 2 + add.at<cv::Vec4b>(i-y, j-x) / 2;
            }
            else if (ori.type() == CV_8UC1) {
                ori.at<uchar>(i,j) = ori.at<uchar>(i,j) / 2 + add.at<uchar>(i-y, j-x) / 2;
            }
            else {
                ori.at<cv::Vec3b>(i,j) = ori.at<cv::Vec3b>(i,j) / 2 + add.at<cv::Vec3b>(i-y, j-x) / 2;
            }
        }
    }
    return ori;
}

cv::Mat minus(cv::Mat &ori, const cv::Mat& add, int x, int y)
{
    int xMax = std::min(x + add.cols, ori.cols);
    int yMax = std::min(y + add.rows, ori.rows);
    for (int i = y; i < yMax; i++) {
        for (int j = x; j < xMax; j++) {
            if (ori.type() == CV_8UC4) {
                ori.at<cv::Vec4b>(i,j)[0] = toRange(ori.at<cv::Vec4b>(i,j)[0] - add.at<cv::Vec4b>(i-y, j-x)[0]);
                ori.at<cv::Vec4b>(i,j)[1] = toRange(ori.at<cv::Vec4b>(i,j)[1] - add.at<cv::Vec4b>(i-y, j-x)[1]);
                ori.at<cv::Vec4b>(i,j)[2] = toRange(ori.at<cv::Vec4b>(i,j)[2] - add.at<cv::Vec4b>(i-y, j-x)[2]);
            }
            else if (ori.type() == CV_8UC1) {
                ori.at<uchar>(i,j) = toRange(ori.at<uchar>(i,j) - add.at<uchar>(i-y, j-x));
            }
            else {
                ori.at<cv::Vec3b>(i,j)[0] = toRange(ori.at<cv::Vec3b>(i,j)[0] - add.at<cv::Vec3b>(i-y, j-x)[0]);
                ori.at<cv::Vec3b>(i,j)[1] = toRange(ori.at<cv::Vec3b>(i,j)[1] - add.at<cv::Vec3b>(i-y, j-x)[1]);
                ori.at<cv::Vec3b>(i,j)[2] = toRange(ori.at<cv::Vec3b>(i,j)[2] - add.at<cv::Vec3b>(i-y, j-x)[2]);
            }
        }
    }
    return ori;
}

cv::Mat multiply(cv::Mat &ori, const cv::Mat& add, int x, int y)
{
    int xMax = std::min(x + add.cols, ori.cols);
    int yMax = std::min(y + add.rows, ori.rows);
    for (int i = y; i < yMax; i++) {
        for (int j = x; j < xMax; j++) {
            if (ori.type() == CV_8UC4) {
                ori.at<cv::Vec4b>(i,j)[0] = (ori.at<cv::Vec4b>(i,j)[0] * add.at<cv::Vec4b>(i-y, j-x)[0]) % 256;
                ori.at<cv::Vec4b>(i,j)[1] = (ori.at<cv::Vec4b>(i,j)[1] * add.at<cv::Vec4b>(i-y, j-x)[1]) % 256;
                ori.at<cv::Vec4b>(i,j)[2] = (ori.at<cv::Vec4b>(i,j)[2] * add.at<cv::Vec4b>(i-y, j-x)[2]) % 256;
            }
            else if (ori.type() == CV_8UC1) {
                ori.at<uchar>(i,j) = toRange(ori.at<uchar>(i,j) - add.at<uchar>(i-y, j-x));
            }
            else {
                ori.at<cv::Vec3b>(i,j)[0] = (ori.at<cv::Vec3b>(i,j)[0] - add.at<cv::Vec3b>(i-y, j-x)[0]) % 256;
                ori.at<cv::Vec3b>(i,j)[1] = (ori.at<cv::Vec3b>(i,j)[1] - add.at<cv::Vec3b>(i-y, j-x)[1]) % 256;
                ori.at<cv::Vec3b>(i,j)[2] = (ori.at<cv::Vec3b>(i,j)[2] - add.at<cv::Vec3b>(i-y, j-x)[2]) % 256;
            }
        }
    }
    return ori;
}

uchar toRange(uchar a)
{
    if (a < 0)
        return 0;
    else if (a > 255)
        return 255;
    else
        return a;
}

int toRangeInt(int a)
{
    if (a < 0)
        return 0;
    else if (a > 255)
        return 255;
    else
        return a;
}

cv::Mat contrastLinear(const cv::Mat& ori, float ls, int lp, float ms, int rp, float rs)
{
    if (ori.type() != CV_8UC1) {
        return ori;
    }
    cv::Mat result = ori.clone();
    for (int i = 0; i < result.rows; i++) {
        for (int j = 0; j < result.cols; j++) {
            if (result.at<uchar>(i,j) <= lp) {
                result.at<uchar>(i,j) = toRangeInt((int)result.at<uchar>(i,j) * ls);
            }
            else if (result.at<uchar>(i,j) > lp && result.at<uchar>(i,j) <= rp) {
                result.at<uchar>(i,j) = toRangeInt((int)result.at<uchar>(i,j) * ms);
            }
            else {
                result.at<uchar>(i,j) = toRangeInt((int)result.at<uchar>(i,j) * rs);
            }
        }
    }
    return result;
}

cv::Mat contrastExp(const cv::Mat& ori, float x)
{
    if (ori.type() != CV_8UC1) {
        return ori;
    }
    cv::Mat result = ori.clone();
    for (int i = 0; i < result.rows; i++) {
        for (int j = 0; j < result.cols; j++) {
            result.at<uchar>(i,j) = toRangeInt((int)pow((double)result.at<uchar>(i,j), x));
        }
    }
    return result;
}

cv::Mat contrastLog(const cv::Mat& ori, float x)
{
    if (ori.type() != CV_8UC1) {
        return ori;
    }
    cv::Mat result = ori.clone();
    for (int i = 0; i < result.rows; i++) {
        for (int j = 0; j < result.cols; j++) {
            double numerator = std::log10f((double)result.at<uchar>(i,j) + 1);
            double denominator = std::log10f((double)1/x + 1);
            result.at<uchar>(i,j) = toRangeInt(numerator / denominator);
        }
    }
    return result;
}

cv::Mat equalization(const cv::Mat& ori)
{
    cv::Mat result = ori.clone();
    std::vector<double> hist(256);
    std::vector<double> dhist(256);
    std::vector<double> Dhist(256);
    int sum = result.rows * result.cols;
    for (int i = 0; i < result.rows; i++) {
        for (int j = 0; j < result.cols; j++) {
            int index = result.at<uchar>(i,j);
            hist[index] += 1;
        }
    }
    for (int i = 0; i < 256; i++) {
        dhist[i] = hist[i] / sum;
        for (int j = 0; j <= i; j++) {
            Dhist[i] = Dhist[i] + dhist[j];
        }
    }
    for (int i = 0; i < result.rows; i++) {
        for (int j = 0; j < result.cols; j++) {
            uchar src = ori.at<uchar>(i,j);
            int mapped = (int)(Dhist[src] * 255);
            result.at<uchar>(i,j) = mapped;
        }
    }
    return result;
}

cv::Mat smoothFilter(const cv::Mat& src, int typeIndex, int sizeIndex)
{
    cv::Mat dst = src.clone();
    if ((typeIndex == 0 || typeIndex == 1) && (sizeIndex == 0 || sizeIndex == 1 || sizeIndex == 2)) {
        std::vector<std::vector<double>> kernel = initKernel(typeIndex, sizeIndex);
        dst = applyKernel(src, kernel);
    }
    else if (typeIndex == 2 && (sizeIndex == 0 || sizeIndex == 1 || sizeIndex == 2)) {
        dst = applyMedianKernel(src, sizeIndex);
    }
    return dst;
}

std::vector<std::vector<double>> initKernel(int typeIndex, int sizeIndex)
{
    std::vector<std::vector<double>> kernel;
    if (typeIndex == 0 && sizeIndex == 0) {
        std::vector<double> line;

        line.push_back(1.0/9.0);
        line.push_back(1.0/9.0);
        line.push_back(1.0/9.0);

        kernel.push_back(line);
        kernel.push_back(line);
        kernel.push_back(line);
    }
    else if (typeIndex == 0 && sizeIndex == 1) {
        std::vector<double> line;

        line.push_back(1.0/16.0);
        line.push_back(1.0/16.0);
        line.push_back(1.0/16.0);
        line.push_back(1.0/16.0);

        kernel.push_back(line);
        kernel.push_back(line);
        kernel.push_back(line);
        kernel.push_back(line);
    }
    else if (typeIndex == 0 && sizeIndex == 2) {
        std::vector<double> line;

        line.push_back(1.0/25.0);
        line.push_back(1.0/25.0);
        line.push_back(1.0/25.0);
        line.push_back(1.0/25.0);
        line.push_back(1.0/25.0);

        kernel.push_back(line);
        kernel.push_back(line);
        kernel.push_back(line);
        kernel.push_back(line);
        kernel.push_back(line);
    }
    else if (typeIndex == 1 && sizeIndex == 0) {
        std::vector<double> line1;
        std::vector<double> line2;

        line1.push_back(1.0/16.0);
        line1.push_back(2.0/16.0);
        line1.push_back(1.0/16.0);

        line2.push_back(2.0/16.0);
        line2.push_back(4.0/16.0);
        line2.push_back(2.0/16.0);

        kernel.push_back(line1);
        kernel.push_back(line2);
        kernel.push_back(line1);
    }
    else if (typeIndex == 1 && sizeIndex == 1) {
        std::vector<double> line1;
        std::vector<double> line2;

        line1.push_back(1.0/56.0);
        line1.push_back(3.0/56.0);
        line1.push_back(3.0/56.0);
        line1.push_back(1.0/56.0);

        line2.push_back(3.0/56.0);
        line2.push_back(7.0/56.0);
        line2.push_back(7.0/56.0);
        line2.push_back(3.0/56.0);

        kernel.push_back(line1);
        kernel.push_back(line2);
        kernel.push_back(line2);
        kernel.push_back(line1);
    }
    else if (typeIndex == 1 && sizeIndex == 2) {
        std::vector<double> line1;
        std::vector<double> line2;
        std::vector<double> line3;

        line1.push_back(1.0/273.0);
        line1.push_back(4.0/273.0);
        line1.push_back(7.0/273.0);
        line1.push_back(4.0/273.0);
        line1.push_back(1.0/273.0);

        line2.push_back(4.0/273.0);
        line2.push_back(16.0/273.0);
        line2.push_back(26.0/273.0);
        line2.push_back(16.0/273.0);
        line2.push_back(4.0/273.0);

        line3.push_back(7.0/273.0);
        line3.push_back(26.0/273.0);
        line3.push_back(41.0/273.0);
        line3.push_back(26.0/273.0);
        line3.push_back(7.0/273.0);

        kernel.push_back(line1);
        kernel.push_back(line2);
        kernel.push_back(line3);
        kernel.push_back(line2);
        kernel.push_back(line1);
    }
    else if (typeIndex == 2) {
        std::vector<double> line1;
        std::vector<double> line2;

        line1.push_back(-1);
        line1.push_back(0);
        line1.push_back(1);

        line2.push_back(-2);
        line2.push_back(0);
        line2.push_back(2);

        kernel.push_back(line1);
        kernel.push_back(line2);
        kernel.push_back(line1);
    }
    else if (typeIndex == 3) {
        std::vector<double> line1;
        std::vector<double> line2;
        std::vector<double> line3;

        line1.push_back(1);
        line1.push_back(2);
        line1.push_back(1);

        line2.push_back(0);
        line2.push_back(0);
        line2.push_back(0);

        line3.push_back(-1);
        line3.push_back(-2);
        line3.push_back(-1);

        kernel.push_back(line1);
        kernel.push_back(line2);
        kernel.push_back(line3);
    }
    else if (typeIndex == 4) {
        std::vector<double> line1;
        std::vector<double> line2;
        std::vector<double> line3;

        line1.push_back(1);
        line1.push_back(1);
        line1.push_back(1);

        line2.push_back(1);
        line2.push_back(-8);
        line2.push_back(1);

        line3.push_back(1);
        line3.push_back(1);
        line3.push_back(1);

        kernel.push_back(line1);
        kernel.push_back(line2);
        kernel.push_back(line3);
    }
    return kernel;
}

cv::Mat applyKernel(const cv::Mat& src, const std::vector<std::vector<double>>& kernel)
{
    int size = kernel.size();
    cv::Mat dst = cv::Mat(cv::Size(src.cols - size, src.rows - size), src.type(), cv::Scalar::all(0));
    for (int i = 0; i < src.rows - size; i++) {
        for (int j = 0; j < src.cols - size; j++) {
            double pixel = 0;
            for (int k = 0; k < size; k++) {
                for (int l = 0; l < size; l++) {
                    pixel += (double)src.at<uchar>(i+k,j+l) * kernel[k][l];
                }
            }
            dst.at<uchar>(i,j) = toRangeInt((int)pixel);
        }
    }
    return dst;
}

cv::Mat applyMedianKernel(const cv::Mat& src, int sizeIndex)
{
    int size = sizeIndex + 3;
    cv::Mat dst = cv::Mat(cv::Size(src.cols - size, src.rows - size), src.type(), cv::Scalar::all(0));
    for (int i = 0; i < src.rows - size; i++) {
        for (int j = 0; j < src.cols - size; j++) {
            std::vector<uchar> pixels;
            for (int k = 0; k < size; k++) {
                for (int l = 0; l < size; l++) {
                    pixels.push_back(src.at<uchar>(i+k,j+l));
                }
            }
            std::sort(pixels.begin(), pixels.end());
            uchar median = size % 2 == 0 ? (pixels[size/2] + pixels[size/2-1]) / 2 : pixels[size/2];
            dst.at<uchar>(i,j) = median;
        }
    }
    return dst;
}

cv::Mat EdgeDetection(const cv::Mat& src, int typeIndex)
{
    cv::Mat dst = src.clone();
    cv::Mat dstX = src.clone();
    cv::Mat dstY = src.clone();
    if (typeIndex == 0) {
        std::vector<std::vector<double>> gradX = initKernel(2, 0);
        std::vector<std::vector<double>> gradY = initKernel(3, 0);
        dstX = applyKernel(src, gradX);
        dstY = applyKernel(src, gradY);
        dst = dstX + dstY;
    }
    else if (typeIndex == 1) {
        std::vector<std::vector<double>> kernel = initKernel(4, 0);
        dst = applyKernel(src, kernel);
    }
    else if (typeIndex == 2) {
        std::vector<std::vector<double>> kernel = initKernel(1, 0);
        dst = applyKernel(src, kernel);
        std::vector<std::vector<double>> gradX = initKernel(2, 0);
        std::vector<std::vector<double>> gradY = initKernel(3, 0);
        dstX = applyKernel(dst, gradX);
        dstY = applyKernel(dst, gradY);
        dst = dstX + dstY;
        cv::Mat grad = dst.clone();
        cv::Mat theta = cv::Mat(cv::Size(dst.cols, dst.rows), CV_32F, cv::Scalar::all(0));
        for (int i = 0; i < theta.rows; i++) {
            for (int j = 0; j < theta.cols; j++) {
                theta.at<double>(i,j) = (std::atan2(dstY.at<uchar>(i,j), dstY.at<uchar>(i,j)) * 180 / CV_PI) + 90;
            }
        }
        dst = noMaxSuppression(dst, theta);
        std::vector<int> hist(1024, 0);
        for (int i = 0; i < dst.rows; i++) {
            for (int j = 0; j < dst.cols; j++) {
                if (dst.at<uchar>(i,j) != 0) {
                    int index = grad.at<uchar>(i,j);
                    hist[index]++;
                }
            }
        }
        int edgeNum = 0;
        int maxMag = 0;
        for (unsigned long i = 1; i < hist.size(); i++) {
            if (hist[i] != 0) {
                maxMag = hist[i];
            }
            edgeNum += hist[i];
        }
        int highThreshold;
        int lowThreshold;
        double rateHigh = 0.7;
        double rateLow = 0.5;
        int highCount = (int)(rateHigh * edgeNum + 0.5);
        int count = 1;
        edgeNum = hist[1];
        while ((edgeNum <= highCount) && (count < maxMag - 1)) {
            count++;
            edgeNum += hist[count];
        }
        highThreshold = count;
        lowThreshold = (int)(highThreshold * rateLow + 0.5);
        for (int i = 0; i < dst.rows; i++) {
            for (int j = 0; j < dst.cols; j++) {
                std::queue<std::vector<int>> que;
                std::vector<int>tmp;
                tmp.push_back(i);
                tmp.push_back(j);
                que.push(tmp);
                while (!que.empty()) {
                    int x = que.front()[1];
                    int y = que.front()[0];
                    que.pop();
                    if (grad.at<uchar>(y,x) >= highThreshold && dst.at<uchar>(y,x) == 0) {
                        int x0 = (x == 0) ? x : x - 1;
                        int x2 = (x == grad.cols - 1) ? x : x + 1;
                        int y0 = (y == 0) ? y : y - 1;
                        int y2 = (y == grad.rows - 1) ? y : y + 1;
                        dst.at<uchar>(y,x) = grad.at<uchar>(y,x);
                        for (int x1 = x0; x1 < x2; x1++) {
                            for (int y1 = y0; y1 < y2; y1++) {
                                if ((x1 != x || y1 != y) && grad.at<uchar>(y1,x1) >= highThreshold && dst.at<uchar>(y1,x1) == 0) {
                                    std::vector<int>tmp;
                                    tmp.push_back(y1);
                                    tmp.push_back(x1);
                                    que.push(tmp);
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return dst;
}

cv::Mat noMaxSuppression(const cv::Mat &src, const cv::Mat &theta)
{
    cv::Mat dst = src.clone();
    for (int i = 1; i < src.rows-1; i++) {
        for (int j = 1; j < src.cols-1; j++) {
            double angle = theta.at<double>(i,j);
            uchar m0 = src.at<uchar>(i,j);
            if ((angle >= 0 && angle < 22.5) || angle >= 157.5) {
                double m1 = src.at<uchar>(i,j-1);
                double m2 = src.at<uchar>(i,j+1);
                if (m0 < m1 || m0 < m2) {
                    dst.at<uchar>(i,j) = 0;
                }
            }
            else if (angle >= 22.5 && angle < 67.5) {
                double m1 = src.at<uchar>(i-1,j+1);
                double m2 = src.at<uchar>(i+1,j-1);
                if (m0 < m1 || m0 < m2) {
                    dst.at<uchar>(i,j) = 0;
                }
            }
            else if (angle >= 67.5 && angle < 112.5) {
                double m1 = src.at<uchar>(i-1,j);
                double m2 = src.at<uchar>(i+1,j);
                if (m0 < m1 || m0 < m2) {
                    dst.at<uchar>(i,j) = 0;
                }
            }
            else if (angle >= 112.5 && angle < 157.5) {
                double m1 = src.at<uchar>(i-1,j-1);
                double m2 = src.at<uchar>(i+1,j+1);
                if (m0 < m1 || m0 < m2) {
                    dst.at<uchar>(i,j) = 0;
                }
            }
        }
    }
    return dst;
}

cv::Mat houghLineTransform(const cv::Mat& src)
{
    int rMax = (int)std::sqrt(src.rows * src.rows + src.cols * src.cols) + 1;
    int thetaMax = 360;
    int threshold = 100;
    cv::Mat trans = cv::Mat(cv::Size(thetaMax, rMax), CV_8UC1, cv::Scalar::all(0));
    cv::Mat dst = src.clone();
    for (int y = 0; y < src.rows; y++) {
        for (int x = 0; x < src.cols; x++) {
            for(int theta = 0; theta < thetaMax; theta++) {
                if (src.at<uchar>(y,x) > threshold) {
                    int tp = (int)(x * sin(theta*CV_PI/180) + y * cos(theta*CV_PI/180));
                    if (tp < 0 || trans.at<uchar>(tp,theta) == 255) continue;
                    trans.at<uchar>(tp,theta) += 1;
                }
            }
        }
    }
    cv::cvtColor(dst, dst, CV_GRAY2BGR);
    for (int rou = 0; rou < rMax; rou++) {
        for (int theta = 0; theta < thetaMax; theta++) {
            if (trans.at<uchar>(rou, theta) > 200) {
                drawLine(dst, rou, theta);
            }
        }
    }
    return dst;
}

cv::Mat drawLine(cv::Mat& src, int rou, int theta)
{
    double k, b;
    int x, y;
    if (theta != 90) {
        b = (double)rou / cos(theta * CV_PI / 180);
        k = -sin(theta * CV_PI / 180) / cos(theta * CV_PI / 180);
        y = 0;
        x = 0;
        if (abs(k) <= 1) {
            for (x = 0; x < src.cols; x++) {
                y = (int)k * x + b;
                if (y >= 0 && y < src.rows) {
                    src.at<cv::Vec3b>(y,x) = cv::Vec3b(0, 255, 0);
                }
            }
        }
        else {
            for (y = 0; y < src.rows; y++) {
                x = (int)(y / k - b / k);
                if (x >= 0 && x < src.cols) {
                    src.at<cv::Vec3b>(y,x) = cv::Vec3b(0, 255, 0);
                }
            }
        }
    }
    else {
        for (y = 0; y < src.rows; y++) {
            src.at<cv::Vec3b>(y,rou) = cv::Vec3b(0, 255, 0);
        }
    }
    return src;
}

cv::Mat houghCircleTransform(const cv::Mat &src)
{
    int r_min = 10;
    int r_max = 110;
    int threshold = 100;
    cv::Mat trans = cv::Mat(cv::Size(src.cols, src.rows), CV_8UC(r_max), cv::Scalar::all(0));
    cv::Mat dst = src.clone();
    for (int y = 0; y < src.rows; y++) {
        for (int x = 0; x < src.cols; x++) {
            for (int r = r_min; r < r_max; r++) {
                for (int angle = 0; angle < 360; angle++) {
                    if (src.at<uchar>(y,x) > threshold) {
                        int a = (int)(x - r * cos(angle*CV_PI/180) + 0.5);
                        int b = (int)(y - r * sin(angle*CV_PI/180) + 0.5);
                        if (a > 0 && a < src.cols && b > 0 && b < src.rows) {
                            if (trans.at<cv::Vec<uchar, 100>>(b,a)[r] < 255) {
                                trans.at<cv::Vec<uchar, 100>>(b,a)[r] += 1;
                            }
                        }
                    }
                }
            }
        }
    }
    cv::cvtColor(dst, dst, CV_GRAY2BGR);
    for (int y = 0; y < trans.rows; y++) {
        for (int x = 0; x < trans.cols; x++) {
            for (int r = r_min; r < r_max; r++) {
                if (trans.at<cv::Vec<uchar, 100>>(y,x)[r] > 200) {
                    drawCircle(dst, x, y, r);
                }
            }
        }
    }
    return dst;
}

cv::Mat drawCircle(cv::Mat& src, int a, int b, int r)
{
    for (int theta = 0; theta < 360; theta++) {
        int x = (a + r * cos(theta*CV_PI/180) + 0.5);
        int y = (b + r * sin(theta*CV_PI/180) + 0.5);
        if (x > 0 && x < src.cols && y > 0 && y < src.rows) {
            src.at<cv::Vec3b>(y,x) = cv::Vec3b(0, 255, 0);
        }
    }
    return src;
}

cv::Mat dilation(const cv::Mat& src, std::vector<std::vector<int>>& kernel)
{
    int size = kernel.size();
    cv::Mat dst = src.clone();
    for (int i = 0; i < src.rows - size; i++) {
        for (int j = 0; j < src.cols - size; j++) {
            int pixel = 255;
            for (int k = 0; k < size; k++) {
                for (int l = 0; l < size; l++) {
                    if (src.at<uchar>(i+k,j+l) == 0 && kernel[k][l] == 1) {
                        pixel = 0;
                        break;
                    }
                }
            }
            dst.at<uchar>(i+size/2,j+size/2) = pixel;
        }
    }
    return dst;
}

cv::Mat erosion(const cv::Mat& src, std::vector<std::vector<int>>& kernel)
{
    int size = kernel.size();
    cv::Mat dst = src.clone();
    for (int i = 0; i < src.rows - size; i++) {
        for (int j = 0; j < src.cols - size; j++) {
            int pixel = 0;
            for (int k = 0; k < size; k++) {
                for (int l = 0; l < size; l++) {
                    if (!(src.at<uchar>(i+k,j+l) == 0 && kernel[k][l] == 1) && kernel[k][l] != 0) {
                        pixel = 255;
                        break;
                    }
                }
            }
            dst.at<uchar>(i+size/2,j+size/2) = pixel;
        }
    }
    return dst;
}

cv::Mat open(const cv::Mat& src, std::vector<std::vector<int>>& kernel)
{
    return dilation(erosion(src, kernel), kernel);
}

cv::Mat close(const cv::Mat& src, std::vector<std::vector<int>>& kernel)
{
    return erosion(dilation(src, kernel), kernel);
}

cv::Mat thinning(const cv::Mat& src, std::vector<std::vector<int>>& kernel)
{
    return andOp(src, inverseMat(hitMissTransform(src, kernel)));
}

cv::Mat thicking(const cv::Mat& src, std::vector<std::vector<int>>& kernel)
{
    return orOp(src, hitMissTransform(src, kernel));
}

cv::Mat hitMissTransform(const cv::Mat& src, std::vector<std::vector<int>>& kernel)
{
    std::vector<std::vector<int>> kernel2 = inverseKernel(kernel);
    cv::Mat src2 = inverseMat(src);
    cv::Mat ero1 = erosion(src, kernel);
    cv::Mat ero2 = erosion(src2, kernel2);
    return andOp(ero1, ero2);
}

std::vector<std::vector<int>> inverseKernel(std::vector<std::vector<int>>& kernel)
{
    std::vector<std::vector<int>> result(kernel);
    for (unsigned long i = 0; i < kernel.size(); i++) {
        for (unsigned long j = 0; j < kernel[i].size(); j++) {
            if (kernel[i][j] == 0) {
                result[i][j] = 1;
            }
            else if (kernel[i][j] == 1) {
                result[i][j] = 0;
            }
        }
    }
    return result;
}

cv::Mat inverseMat(const cv::Mat &src)
{
    cv::Mat result = src.clone();
    for (int i = 0; i < src.rows; i++) {
        for (int j = 0; j < src.cols; j++) {
            if (src.at<uchar>(i,j) == 0) {
                result.at<uchar>(i,j) = 255;
            }
            else {
                result.at<uchar>(i,j) = 0;
            }
        }
    }
    return result;
}

cv::Mat andOp(const cv::Mat& mat1, const cv::Mat& mat2)
{
    cv::Mat result = mat1.clone();
    int Rows = std::min(mat1.rows, mat2.rows);
    int Cols = std::min(mat1.cols, mat2.cols);
    for (int i = 0; i < Rows; i++) {
        for (int j = 0; j < Cols; j++) {
            if (mat1.at<uchar>(i,j) == 0 && mat2.at<uchar>(i,j) == 0) {
                result.at<uchar>(i,j) = 0;
            }
            else {
                result.at<uchar>(i,j) = 255;
            }
        }
    }
    return result;
}

cv::Mat orOp(const cv::Mat& mat1, const cv::Mat& mat2)
{
    cv::Mat result = mat1.clone();
    int Rows = std::min(mat1.rows, mat2.rows);
    int Cols = std::min(mat1.cols, mat2.cols);
    for (int i = 0; i < Rows; i++) {
        for (int j = 0; j < Cols; j++) {
            if (mat1.at<uchar>(i,j) == 0 || mat2.at<uchar>(i,j) == 0) {
                result.at<uchar>(i,j) = 0;
            }
            else {
                result.at<uchar>(i,j) = 255;
            }
        }
    }
    return result;
}

static float Round(float f)
{
    return ( ceil(f)-f > f-floor(f) ) ? floor(f) : ceil(f);
}

int ChessBoardDist(int x1, int y1, int x2, int y2)
{
    return (abs(x1-x2) > abs(y1-y2)) ? abs(x1-x2) : abs(y1-y2);
}

int CityBlockDist(int x1, int y1, int x2, int y2)
{
    return ( abs(x1-x2) + abs(y1-y2) );
}

float EuclideanDist(int x1, int y1, int x2, int y2)
{
    return sqrt( (float)((x1-x2)*(x1-x2) + (y1-y2)*(y1-y2)) );
}

int MyMin(int x, int y)
{
    return (x < y) ? x : y;
}

float MyMin(float x, float y)
{
    return (x < y) ? x : y;
}

cv::Mat euclideanDistanceTransform(const cv::Mat src)
{
    cv::Mat I = src.clone();
    int channels = I.channels();
    int nRows = I.rows * channels;
    int nCols = I.cols;
    int i,j;
    uchar* p;
    uchar* q;
    float fMin = 0.0;
    float fDis = 0.0;
    for( i = 1; i < nRows-1; ++i)
    {
        p = I.ptr<uchar>(i);
        for ( j = 1; j < nCols; ++j)
        {
            q = I.ptr<uchar>(i-1);
            fDis = EuclideanDist(i, j, i-1, j-1);
            fMin = MyMin((float)p[j], fDis+q[j-1] );

            fDis = EuclideanDist(i, j, i-1, j);
            fMin = MyMin( fMin, fDis+q[j] );

            q = I.ptr<uchar>(i);
            fDis = EuclideanDist(i, j, i, j-1);
            fMin = MyMin( fMin, fDis+q[j-1] );

            q = I.ptr<uchar>(i+1);
            fDis = EuclideanDist(i, j, i+1, j-1);
            fMin = MyMin( fMin, fDis+q[j-1] );

            p[j] = (uchar)Round(fMin);
        }
    }
    for( i = nRows-2; i > 0; i-- )
    {
        p = I.ptr<uchar>(i);
        for( j = nCols-1; j >= 0; j-- )
        {

            q = I.ptr<uchar>(i+1);
            fDis = EuclideanDist(i, j, i+1, j);
            fMin = MyMin( (float)p[j], fDis+q[j] );

            fDis = EuclideanDist(i, j, i+1, j+1);
            fMin = MyMin( fMin, fDis+q[j+1] );

            q = I.ptr<uchar>(i);
            fDis = EuclideanDist(i, j, i, j+1);
            fMin = MyMin( fMin, fDis+q[j+1] );

            q = I.ptr<uchar>(i-1);
            fDis = EuclideanDist(i, j, i-1, j+1);
            fMin = MyMin( fMin, fDis+q[j+1] );

            p[j] = (uchar)Round(fMin);
        }
    }
    return I;
}

cv::Mat cityBlockDistanceTransform(const cv::Mat src)
{
    cv::Mat I = src.clone();
    int channels = I.channels();
    int nRows = I.rows * channels;
    int nCols = I.cols;
    int i,j;
    uchar* p;
    uchar* q;
    float fMin = 0.0;
    float fDis = 0.0;
    for( i = 1; i < nRows-1; ++i)
    {
        p = I.ptr<uchar>(i);
        for ( j = 1; j < nCols; ++j)
        {
            q = I.ptr<uchar>(i-1);
            fDis = CityBlockDist(i, j, i-1, j-1);
            fMin = MyMin((float)p[j], fDis+q[j-1] );

            fDis = CityBlockDist(i, j, i-1, j);
            fMin = MyMin( fMin, fDis+q[j] );

            q = I.ptr<uchar>(i);
            fDis = CityBlockDist(i, j, i, j-1);
            fMin = MyMin( fMin, fDis+q[j-1] );

            q = I.ptr<uchar>(i+1);
            fDis = CityBlockDist(i, j, i+1, j-1);
            fMin = MyMin( fMin, fDis+q[j-1] );

            p[j] = (uchar)Round(fMin);
        }
    }
    for( i = nRows-2; i > 0; i-- )
    {
        p = I.ptr<uchar>(i);
        for( j = nCols-1; j >= 0; j-- )
        {

            q = I.ptr<uchar>(i+1);
            fDis = CityBlockDist(i, j, i+1, j);
            fMin = MyMin( (float)p[j], fDis+q[j] );

            fDis = CityBlockDist(i, j, i+1, j+1);
            fMin = MyMin( fMin, fDis+q[j+1] );

            q = I.ptr<uchar>(i);
            fDis = CityBlockDist(i, j, i, j+1);
            fMin = MyMin( fMin, fDis+q[j+1] );

            q = I.ptr<uchar>(i-1);
            fDis = CityBlockDist(i, j, i-1, j+1);
            fMin = MyMin( fMin, fDis+q[j+1] );

            p[j] = (uchar)Round(fMin);
        }
    }
    return I;
}

cv::Mat chessboardDistanceTransform(const cv::Mat src)
{
    cv::Mat I = src.clone();
    int channels = I.channels();
    int nRows = I.rows * channels;
    int nCols = I.cols;
    int i,j;
    uchar* p;
    uchar* q;
    float fMin = 0.0;
    float fDis = 0.0;
    for( i = 1; i < nRows-1; ++i)
    {
        p = I.ptr<uchar>(i);
        for ( j = 1; j < nCols; ++j)
        {
            q = I.ptr<uchar>(i-1);
            fDis = ChessBoardDist(i, j, i-1, j-1);
            fMin = MyMin((float)p[j], fDis+q[j-1] );

            fDis = ChessBoardDist(i, j, i-1, j);
            fMin = MyMin( fMin, fDis+q[j] );

            q = I.ptr<uchar>(i);
            fDis = ChessBoardDist(i, j, i, j-1);
            fMin = MyMin( fMin, fDis+q[j-1] );

            q = I.ptr<uchar>(i+1);
            fDis = ChessBoardDist(i, j, i+1, j-1);
            fMin = MyMin( fMin, fDis+q[j-1] );

            p[j] = (uchar)Round(fMin);
        }
    }
    for( i = nRows-2; i > 0; i-- )
    {
        p = I.ptr<uchar>(i);
        for( j = nCols-1; j >= 0; j-- )
        {

            q = I.ptr<uchar>(i+1);
            fDis = ChessBoardDist(i, j, i+1, j);
            fMin = MyMin( (float)p[j], fDis+q[j] );

            fDis = ChessBoardDist(i, j, i+1, j+1);
            fMin = MyMin( fMin, fDis+q[j+1] );

            q = I.ptr<uchar>(i);
            fDis = ChessBoardDist(i, j, i, j+1);
            fMin = MyMin( fMin, fDis+q[j+1] );

            q = I.ptr<uchar>(i-1);
            fDis = ChessBoardDist(i, j, i-1, j+1);
            fMin = MyMin( fMin, fDis+q[j+1] );

            p[j] = (uchar)Round(fMin);
        }
    }
    return I;
}

bool isWhite(const cv::Mat& src) {
    bool flag = true;
    for (int i = 0; i < src.rows; i++) {
        for (int j = 0; j < src.cols; j++) {
            if (src.at<uchar>(i,j) != 255) {
                flag = false;
                break;
            }
        }
    }
    return flag;
}

bool check(const cv::Mat &input)
{
    int cols = input.cols;
    int rows = input.rows;
    for (int i = 0; i < rows ; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            if (input.at<uchar>(i, j) != 255)
                return false;
        }
    }
    return true;
}

cv::Mat minusOp(cv::Mat& mat1, cv::Mat& mat2)
{
    cv::Mat result = mat1.clone();
    for (int i = 0; i < mat1.rows; i++) {
        for (int j = 0; j < mat1.cols; j++) {
            if (mat1.at<uchar>(i,j) == 0 && mat2.at<uchar>(i,j) == 255) {
                result.at<uchar>(i,j) = 0;
            }
            else {
                result.at<uchar>(i,j) = 255;
            }
        }
    }
    return result;
}

cv::Mat skeletonExtract(const cv::Mat& src, std::vector<std::vector<int>>& kernel)
{
    cv::Mat dst = cv::Mat(src.size(), CV_8UC1, cv::Scalar(255));
    cv::Mat test = src.clone();
    int K = 0;

    while (!check(test) && K < 31) {
        K++;
        test = erosion(test, kernel);
    }

    for (int k = 1; k < K; k++) {
        cv::Mat tmp = src.clone();
        for (int i = 0; i < k; i++) {
            tmp = erosion(tmp, kernel);
        }
        cv::Mat opened = open(tmp, kernel);
        tmp = minusOp(tmp, opened);
        dst = orOp(dst, tmp);
    }
    return dst;
}

cv::Mat geodesticDilation(const cv::Mat& src, const cv::Mat& add, std::vector<std::vector<int>>& kernel)
{
    cv::Mat dst = src.clone();
    for (int i = 0; i < 30; i++) {
        dst = dilation(dst, kernel);
        dst = andOp(dst, add);
    }
    return dst;
}

cv::Mat geodesticErosion(const cv::Mat& src, const cv::Mat& add, std::vector<std::vector<int>>& kernel)
{
    cv::Mat dst = src.clone();
    for (int i = 0; i < 30; i++) {
        dst = erosion(dst, kernel);
        dst = orOp(dst, add);
    }
    return dst;
}

cv::Mat grayDilation(const cv::Mat& src, std::vector<std::vector<int>>& kernel)
{
    int size = kernel.size();
    cv::Mat dst = src.clone();
    for (int i = 0; i < src.rows - size; i++) {
        for (int j = 0; j < src.cols - size; j++) {
            uchar pixel = 0;
            for (int k = 0; k < size; k++) {
                for (int l = 0; l < size; l++) {
                    if (kernel[k][l] == 1 && src.at<uchar>(i+k,j+l) > pixel) {
                        pixel = src.at<uchar>(i+k,j+l);
                    }
                }
            }
            dst.at<uchar>(i+size/2,j+size/2) = pixel;
        }
    }
    return dst;
}

cv::Mat grayErosion(const cv::Mat& src, std::vector<std::vector<int>>& kernel)
{
    int size = kernel.size();
    cv::Mat dst = src.clone();
    for (int i = 0; i < src.rows - size; i++) {
        for (int j = 0; j < src.cols - size; j++) {
            uchar pixel = 255;
            for (int k = 0; k < size; k++) {
                for (int l = 0; l < size; l++) {
                    if (kernel[k][l] == 1 && src.at<uchar>(i+k,j+l) < pixel) {
                        pixel = src.at<uchar>(i+k,j+l);
                    }
                }
            }
            dst.at<uchar>(i+size/2,j+size/2) = pixel;
        }
    }
    return dst;
}

cv::Mat grayOpen(const cv::Mat& src, std::vector<std::vector<int>>& kernel)
{
    return grayDilation(grayErosion(src, kernel), kernel);
}

cv::Mat grayClose(const cv::Mat& src, std::vector<std::vector<int>>& kernel)
{
    return grayErosion(grayDilation(src, kernel), kernel);
}

cv::Mat grayAndOp(const cv::Mat& mat1, const cv::Mat& mat2)
{
    cv::Mat result = mat1.clone();
    int Rows = std::min(mat1.rows, mat2.rows);
    int Cols = std::min(mat1.cols, mat2.cols);
    for (int i = 0; i < Rows; i++) {
        for (int j = 0; j < Cols; j++) {
            if (mat1.at<uchar>(i,j) < mat2.at<uchar>(i,j)) {
                result.at<uchar>(i,j) = mat1.at<uchar>(i,j);
            }
            else {
                result.at<uchar>(i,j) = mat2.at<uchar>(i,j);
            }
        }
    }
    return result;
}

cv::Mat grayOrOp(const cv::Mat& mat1, const cv::Mat& mat2)
{
    cv::Mat result = mat1.clone();
    int Rows = std::min(mat1.rows, mat2.rows);
    int Cols = std::min(mat1.cols, mat2.cols);
    for (int i = 0; i < Rows; i++) {
        for (int j = 0; j < Cols; j++) {
            if (mat1.at<uchar>(i,j) > mat2.at<uchar>(i,j)) {
                result.at<uchar>(i,j) = mat1.at<uchar>(i,j);
            }
            else {
                result.at<uchar>(i,j) = mat2.at<uchar>(i,j);
            }
        }
    }
    return result;
}

cv::Mat grayGeodesticDilation(const cv::Mat& src, const cv::Mat& add, std::vector<std::vector<int>>& kernel)
{
    cv::Mat dst = src.clone();
    for (int i = 0; i < 30; i++) {
        dst = grayDilation(dst, kernel);
        dst = grayAndOp(dst, add);
    }
    return dst;
}

cv::Mat grayGeodesticErosion(const cv::Mat& src, const cv::Mat& add, std::vector<std::vector<int>>& kernel)
{
    cv::Mat dst = src.clone();
    for (int i = 0; i < 30; i++) {
        dst = grayErosion(dst, kernel);
        dst = grayOrOp(dst, add);
    }
    return dst;
}

cv::Mat watershed(const cv::Mat& src)
{
    WatershedAlgorithm myWatershed;
    IplImage image(src);
    IplImage *result = myWatershed.run(&image);
    return cv::cvarrToMat(result);
}
}
