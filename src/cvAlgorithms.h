#ifndef ALGORITHMS_H
#define ALGORITHMS_H

#include <QImage>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "WaterShedAlgorithm.h"

namespace Algo {
    QImage cvMat2QImage(const cv::Mat& mat);
    cv::Mat QImage2cvMat(QImage image);
    void split(const cv::Mat& src, std::vector<cv::Mat> &dst);
    cv::Mat toGray(const cv::Mat& mat);
    cv::Mat otsu(const cv::Mat& mat);
    int getOtsuThreshold(const cv::Mat& mat);
    bool isBinary(const QImage image);
    cv::Mat zoomNearest(const cv::Mat& mat, int zoomRate);
    cv::Mat zoomLinear(const cv::Mat& mat, int zoomRate);
    cv::Mat rotateNearest(const cv::Mat& mat, int rotateRate);
    cv::Mat rotateLinear(const cv::Mat& mat, int rotateRate);
    cv::Mat plus(cv::Mat& ori, const cv::Mat& add, int x, int y);
    cv::Mat minus(cv::Mat& ori, const cv::Mat& add, int x, int y);
    cv::Mat multiply(cv::Mat& ori, const cv::Mat& add, int x, int y);
    uchar toRange(uchar a);
    int toRangeInt(int a);
    cv::Mat contrastLinear(const cv::Mat& ori, float ls, int lp, float ms, int rp, float rs);
    cv::Mat contrastExp(const cv::Mat& ori, float x);
    cv::Mat contrastLog(const cv::Mat& ori, float x);
    cv::Mat equalization(const cv::Mat& ori);
    cv::Mat smoothFilter(const cv::Mat& src, int typeIndex, int sizeIndex);
    std::vector<std::vector<double>> initKernel(int typeIndex, int sizeIndex);
    cv::Mat applyKernel(const cv::Mat& src, const std::vector<std::vector<double>>& kernel);
    cv::Mat applyMedianKernel(const cv::Mat& src, int sizeIndex);
    cv::Mat EdgeDetection(const cv::Mat& src, int typeIndex);
    cv::Mat noMaxSuppression(const cv::Mat& src, const cv::Mat& theta);
    cv::Mat houghLineTransform(const cv::Mat& src);
    cv::Mat drawLine(cv::Mat& src, int rou, int theta);
    cv::Mat houghCircleTransform(const cv::Mat& src);
    cv::Mat drawCircle(cv::Mat& src, int x, int y, int r);
    cv::Mat dilation(const cv::Mat& src, std::vector<std::vector<int>>& kernel);
    cv::Mat erosion(const cv::Mat& src, std::vector<std::vector<int>>& kernel);
    cv::Mat open(const cv::Mat& src, std::vector<std::vector<int>>& kernel);
    cv::Mat close(const cv::Mat& src, std::vector<std::vector<int>>& kernel);
    cv::Mat thinning(const cv::Mat& src, std::vector<std::vector<int>>& kernel);
    cv::Mat thicking(const cv::Mat& src, std::vector<std::vector<int>>& kernel);
    std::vector<std::vector<int>> inverseKernel(std::vector<std::vector<int>>& kernel);
    cv::Mat inverseMat(const cv::Mat& src);
    cv::Mat hitMissTransform(const cv::Mat& src, std::vector<std::vector<int>>& kernel);
    cv::Mat andOp(const cv::Mat& mat1, const cv::Mat& mat2);
    cv::Mat orOp(const cv::Mat& mat1, const cv::Mat& mat2);
    cv::Mat euclideanDistanceTransform(const cv::Mat src);
    cv::Mat cityBlockDistanceTransform(const cv::Mat src);
    cv::Mat chessboardDistanceTransform(const cv::Mat src);
    cv::Mat skeletonExtract(const cv::Mat& src, std::vector<std::vector<int>>& kernel);
    cv::Mat geodesticDilation(const cv::Mat& src, const cv::Mat& add, std::vector<std::vector<int>>& kernel);
    cv::Mat geodesticErosion(const cv::Mat& src, const cv::Mat& add, std::vector<std::vector<int>>& kernel);
    cv::Mat grayDilation(const cv::Mat& src, std::vector<std::vector<int>>& kernel);
    cv::Mat grayErosion(const cv::Mat& src, std::vector<std::vector<int>>& kernel);
    cv::Mat grayOpen(const cv::Mat& src, std::vector<std::vector<int>>& kernel);
    cv::Mat grayClose(const cv::Mat& src, std::vector<std::vector<int>>& kernel);
    cv::Mat grayGeodesticDilation(const cv::Mat& src, const cv::Mat& add, std::vector<std::vector<int>>& kernel);
    cv::Mat grayGeodesticErosion(const cv::Mat& src, const cv::Mat& add, std::vector<std::vector<int>>& kernel);
    cv::Mat watershed(const cv::Mat& src);
}

#endif // ALGORITHMS_H
