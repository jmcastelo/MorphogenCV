#ifndef IMAGEOPERATIONS_H
#define IMAGEOPERATIONS_H

#include <opencv2/imgproc.hpp>

class ImageOperation
{
public:
    virtual cv::Mat applyOperation(cv::Mat src) = 0;
};

class ConvertTo: public ImageOperation
{
public:
    double alpha, beta;
    ConvertTo(double a, double b): alpha(a), beta(b){}
    cv::Mat applyOperation(cv::Mat src);
};

class GaussianBlur: public ImageOperation
{
public:
    int ksize;
    double sigmaX, sigmaY;
    GaussianBlur(int k, double sx, double sy): ksize(k), sigmaX(sx), sigmaY(sy){}
    cv::Mat applyOperation(cv::Mat src);
};

class Laplacian: public ImageOperation
{
public:
    int ksize;
    double scale, delta;
    Laplacian(int k, double s, double d): ksize(k), scale(s), delta(d){}
    cv::Mat applyOperation(cv::Mat src);
};

class Rotation: public ImageOperation
{
public:
    double angle, scale;
    int flags;
    Rotation(double a, double s, int f): angle(a), scale(s), flags(f){}
    cv::Mat applyOperation(cv::Mat src);
};

#endif // IMAGEOPERATIONS_H
