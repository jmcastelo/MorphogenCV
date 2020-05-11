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
    cv::GaussianBlur(src, dst, cv::Size(ksize, ksize), sigmaX, sigmaY, cv::BORDER_DEFAULT);
    return dst;
}

cv::Mat Laplacian::applyOperation(cv::Mat src)
{
    cv::Mat tmp;
    cv::Laplacian(src, tmp, CV_16S, ksize, scale, delta, cv::BORDER_DEFAULT);
    cv::Mat dst;
    cv::convertScaleAbs(tmp, dst);
    return dst;
}

cv::Mat Rotation::applyOperation(cv::Mat src)
{
    cv::Point center = cv::Point(src.cols / 2, src.rows / 2);
    cv::Mat rotationMat = cv::getRotationMatrix2D(center, angle, scale);
    cv::Mat dst;
    cv::warpAffine(src, dst, rotationMat, src.size(), flags, cv::BORDER_TRANSPARENT);
    return dst;
}
