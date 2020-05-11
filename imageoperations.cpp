#include "imageoperations.h"

cv::Mat ConvertTo::applyOperation(cv::Mat src)
{
    cv::Mat dst;
    src.convertTo(dst, -1, alpha, beta);
    return dst;
}

cv::Mat GaussianBlur::applyOperation(cv::Mat src)
{
    cv::Mat dst;
    cv::GaussianBlur(src, dst, cv::Size(ksize, ksize), sigmaX, sigmaY);
    return dst;
}

cv::Mat Laplacian::applyOperation(cv::Mat src)
{
    cv::Mat tmp;
    cv::Laplacian(src, tmp, CV_16S, ksize, scale, delta);
    cv::Mat dst;
    cv::convertScaleAbs(tmp, dst);
    return dst;
}

cv::Mat Rotation::applyOperation(cv::Mat src)
{
    cv::Point center = cv::Point(src.cols / 2, src.rows / 2);
    cv::Mat rotationMat = cv::getRotationMatrix2D(center, angle, scale);
    cv::Mat dst;
    cv::warpAffine(src, dst, rotationMat, src.size());
    return dst;
}
