// Copyright 2020 José María Castelo Ares

// This file is part of MorphogenCV.

// MorphogenCV is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// MorphogenCV is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with MorphogenCV.  If not, see <https://www.gnu.org/licenses/>.

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
