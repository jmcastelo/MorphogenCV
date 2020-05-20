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

// Bilateral filter

std::string BilateralFilter::name = "Blur: bilateral";

BilateralFilter::BilateralFilter(bool on, int d, double sc, double ss): ImageOperation(on)
{
    diameter = new IntParameter("Diameter", d, 0, 50);
    sigmaColor = new DoubleParameter("Sigma color", sc, 0.0, 300.0);
    sigmaSpace = new DoubleParameter("Sigma space", ss, 0.0, 300.0);
}

cv::Mat BilateralFilter::applyOperation(const cv::Mat &src)
{
    cv::Mat dst;
    cv::bilateralFilter(src, dst, diameter->value, sigmaColor->value, sigmaSpace->value, cv::BORDER_ISOLATED);
    return dst;
}

// Blur

std::string Blur::name = "Blur: homogeneous";

Blur::Blur(bool on, int size): ImageOperation(on)
{
    ksize = new IntParameter("Kernel size", size, 1, 100);
}

cv::Mat Blur::applyOperation(const cv::Mat &src)
{
    cv::Mat dst;
    cv::blur(src, dst, cv::Size(ksize->value, ksize->value), cv::Point(-1, -1), cv::BORDER_ISOLATED);
    return dst;
}

// Canny

std::string Canny::name = "Canny";

Canny::Canny(bool on, double th1, double th2, int size, bool g): ImageOperation(on)
{
    threshold1 = new DoubleParameter("Threshold 1", th1, 0.0, 300.0);
    threshold2 = new DoubleParameter("Threshold 2", th2, 0.0, 300.0);
    apertureSize = new IntParameter("Aperture size", size, 3, 7);
    L2gradient = new BoolParameter("L2 gradient", g);
}

cv::Mat Canny::applyOperation(const cv::Mat &src)
{
    cv::Mat detectedEdges;
    cv::Canny(src, detectedEdges, threshold1->value, threshold2->value, apertureSize->value, L2gradient->value);
    cv::Mat dst = cv::Mat(src.rows, src.cols, src.type(), cv::Scalar(0));
    src.copyTo(dst, detectedEdges);
    return dst;
}

// Convert to

std::string ConvertTo::name = "Contrast and brightness";

ConvertTo::ConvertTo(bool on, double a, double b): ImageOperation(on)
{
    alpha = new DoubleParameter("Gain", a, -100.0, 100.0);
    beta = new DoubleParameter("Bias", b, -100.0, 100.0);
}

cv::Mat ConvertTo::applyOperation(const cv::Mat &src)
{
    cv::Mat dst;
    src.convertTo(dst, -1, alpha->value, beta->value);
    return dst;
}

// Deblur filter

std::string DeblurFilter::name = "Deblur filter";

DeblurFilter::DeblurFilter(bool on, double r, double snr): ImageOperation(on)
{
    radius = new DoubleParameter("Radius", r, 0.0, 100.0);
    signalToNoiseRatio = new DoubleParameter("Signal to noise ratio", snr, 1.0, 10000.0);
}

void DeblurFilter::computePSF(cv::Mat &outputImg, cv::Size filterSize)
{
    cv::Mat h(filterSize, CV_32F, cv::Scalar(0));
    cv::Point point(filterSize.width / 2, filterSize.height / 2);
    cv::circle(h, point, radius->value, 255, -1, 8);
    cv::Scalar summa = sum(h);
    outputImg = h / summa[0];
}

void DeblurFilter::computeWnrFilter(const cv::Mat &input_h_PSF, cv::Mat &output_G, double nsr)
{
    cv::Mat h_PSF_shifted;
    fftShift(input_h_PSF, h_PSF_shifted);
    cv::Mat planes[2] = { cv::Mat_<float>(h_PSF_shifted.clone()), cv::Mat::zeros(h_PSF_shifted.size(), CV_32F) };
    cv::Mat complexI;
    cv::merge(planes, 2, complexI);
    cv::dft(complexI, complexI);
    cv::split(complexI, planes);
    cv::Mat denom;
    cv::pow(abs(planes[0]), 2, denom);
    denom += nsr;
    cv::divide(planes[0], denom, output_G);
}

void DeblurFilter::fftShift(const cv::Mat &inputImg, cv::Mat &outputImg)
{
    outputImg = inputImg.clone();
    int cx = outputImg.cols / 2;
    int cy = outputImg.rows / 2;
    cv::Mat q0(outputImg, cv::Rect(0, 0, cx, cy));
    cv::Mat q1(outputImg, cv::Rect(cx, 0, cx, cy));
    cv::Mat q2(outputImg, cv::Rect(0, cy, cx, cy));
    cv::Mat q3(outputImg, cv::Rect(cx, cy, cx, cy));
    cv::Mat tmp;
    q0.copyTo(tmp);
    q3.copyTo(q0);
    tmp.copyTo(q3);
    q1.copyTo(tmp);
    q2.copyTo(q1);
    tmp.copyTo(q2);
}

void DeblurFilter::filter2DFreq(const cv::Mat &inputImg, cv::Mat &outputImg, const cv::Mat &H)
{
    cv::Mat planes[2] = { cv::Mat_<float>(inputImg.clone()), cv::Mat::zeros(inputImg.size(), CV_32F) };
    cv::Mat complexI;
    cv::merge(planes, 2, complexI);
    cv::dft(complexI, complexI, cv::DFT_SCALE);
    cv::Mat planesH[2] = { cv::Mat_<float>(H.clone()), cv::Mat::zeros(H.size(), CV_32F) };
    cv::Mat complexH;
    cv::merge(planesH, 2, complexH);
    cv::Mat complexIH;
    cv::mulSpectrums(complexI, complexH, complexIH, 0);
    cv::idft(complexIH, complexIH);
    cv::split(complexIH, planes);
    outputImg = planes[0];
}

cv::Mat DeblurFilter::applyOperation(const cv::Mat &src)
{
    cv::Rect roi = cv::Rect(0, 0, src.cols & -2, src.rows & -2);

    cv::Mat Hw, h;
    computePSF(h, roi.size());
    computeWnrFilter(h, Hw, 1.0 / signalToNoiseRatio->value);

    cv::Mat srcChannels[3];
    cv::split(src, srcChannels);

    cv::Mat dstChannels[3];

    filter2DFreq(srcChannels[0](roi), dstChannels[0], Hw);
    filter2DFreq(srcChannels[1](roi), dstChannels[1], Hw);
    filter2DFreq(srcChannels[2](roi), dstChannels[2], Hw);

    cv::Mat dst;
    cv::merge(dstChannels, 3, dst);
    dst.convertTo(dst, CV_8UC3);
    cv::normalize(dst, dst, 0, 255, cv::NORM_MINMAX);
    return dst;
}

// Equalize histogram

std::string EqualizeHist::name = "Equalize histogram";

EqualizeHist::EqualizeHist(bool on): ImageOperation(on){}

cv::Mat EqualizeHist::applyOperation(const cv::Mat &src)
{
    cv::Mat dst;
    cv::cvtColor(src, dst, cv::COLOR_BGR2YCrCb);

    std::vector<cv::Mat> channels;
    cv::split(dst, channels);

    cv::equalizeHist(channels[0], channels[0]);

    cv::merge(channels, dst);

    cv::cvtColor(dst, dst, cv::COLOR_YCrCb2BGR);

    return dst;
}

// Gamma correction

std::string GammaCorrection::name = "Gamma correction";

GammaCorrection::GammaCorrection(bool on, double g): ImageOperation(on)
{
    gamma = new DoubleParameter("Gamma", g, 0.0, 100.0);
}

cv::Mat GammaCorrection::applyOperation(const cv::Mat &src)
{
    cv::Mat lookUpTable = cv::Mat(1, 256, CV_8U);
    uchar *p = lookUpTable.ptr();
    for (int i = 0; i < 256; i++)
        p[i] = cv::saturate_cast<uchar>(cv::pow(i / 255.0, gamma->value) * 255.0);

    cv::Mat dst = src.clone();
    cv::LUT(src, lookUpTable, dst);
    return dst;
}

// Gaussian blur

std::string GaussianBlur::name = "Blur: Gaussian";

GaussianBlur::GaussianBlur(bool on, int k, double s): ImageOperation(on)
{
    ksize = new IntParameter("Kernel size", k, 0, 25);
    sigma = new DoubleParameter("Sigma", s, 0.001, 1.0);
}

cv::Mat GaussianBlur::applyOperation(const cv::Mat &src)
{
    int size = 2 * ksize->value - 1;
    if (size == -1) size = 0;

    cv::Mat dst;
    cv::GaussianBlur(src, dst, cv::Size(size, size), sigma->value, sigma->value, cv::BORDER_ISOLATED);
    return dst;
}

// Laplacian

std::string Laplacian::name = "Laplacian";

Laplacian::Laplacian(bool on, int k, double s, double d): ImageOperation(on)
{
    ksize = new IntParameter("Kernel size", k, 0, 2);
    scale = new DoubleParameter("Scale", s, -100.0, 100.0);
    delta = new DoubleParameter("Delta", d, -100.0, 100.0);
}

cv::Mat Laplacian::applyOperation(const cv::Mat &src)
{
    int size = 2 * ksize->value + 1;
    cv::Mat tmp;
    cv::Laplacian(src, tmp, CV_16SC3, size, scale->value, delta->value, cv::BORDER_ISOLATED);
    cv::Mat dst;
    cv::convertScaleAbs(tmp, dst);
    return dst;
}

// Median blur

std::string MedianBlur::name = "Blur: median";

MedianBlur::MedianBlur(bool on, int size): ImageOperation(on)
{
    ksize = new IntParameter("Kernel size", size, 1, 25);
}

cv::Mat MedianBlur::applyOperation(const cv::Mat &src)
{
    int size = 2 * ksize->value + 1;
    cv::Mat dst;
    cv::medianBlur(src, dst, size);
    return dst;
}

// Mix channels

std::string MixChannels::name = "Mix channels";

MixChannels::MixChannels(bool on, int b, int g, int r): ImageOperation(on)
{
    std::vector<std::string> valueNames = {"Blue", "Green", "Red"};
    std::vector<int> values = {0, 1, 2};

    blue = new OptionsParameter<int>("Blue", valueNames, values, b);
    green = new OptionsParameter<int>("Green", valueNames, values, g);
    red = new OptionsParameter<int>("Red", valueNames, values, r);
}

cv::Mat MixChannels::applyOperation(const cv::Mat &src)
{
    int fromTo[] = {0, blue->value, 1, green->value, 2, red->value};
    cv::Mat dst(src.rows, src.cols, src.type());
    cv::mixChannels(&src, 1, &dst, 1, fromTo, 3);
    return dst;
}

// Morphological operations

std::string MorphologyEx::name = "Morphology operation";

MorphologyEx::MorphologyEx(bool on, int k, int its, cv::MorphTypes type, cv::MorphShapes shape): ImageOperation(on)
{
    ksize = new IntParameter("Kernel size", k, 1, 25);
    iterations = new IntParameter("Iterations", its, 1, 100);

    std::vector<std::string> typeValueNames = {"Erode", "Dilate", "Opening", "Closing", "Gradient", "Top hat", "Black hat"};
    std::vector<cv::MorphTypes> typeValues = {cv::MORPH_ERODE, cv::MORPH_DILATE, cv::MORPH_OPEN, cv::MORPH_CLOSE, cv::MORPH_GRADIENT, cv::MORPH_TOPHAT, cv::MORPH_BLACKHAT};

    morphType = new OptionsParameter<cv::MorphTypes>("Type", typeValueNames, typeValues, type);

    std::vector<std::string> shapeValueNames = {"Rectangle", "Cross", "Ellipse"};
    std::vector<cv::MorphShapes> shapeValues = {cv::MORPH_RECT, cv::MORPH_CROSS, cv::MORPH_ELLIPSE};

    morphShape = new OptionsParameter<cv::MorphShapes>("Shape", shapeValueNames, shapeValues, shape);
}

cv::Mat MorphologyEx::applyOperation(const cv::Mat &src)
{
    cv::Mat element = cv::getStructuringElement(morphShape->value, cv::Size(ksize->value, ksize->value));
    cv::Mat dst;
    cv::morphologyEx(src, dst, morphType->value, element, cv::Point(-1, -1), iterations->value, cv::BORDER_ISOLATED);
    return dst;
}

// Radial remap

std::string RadialRemap::name = "Radial remap";

RadialRemap::RadialRemap(bool on, double a, cv::InterpolationFlags f): ImageOperation(on)
{
    amplitude = new DoubleParameter("Amplitude", a, -100.0, 100.0);

    std::vector<std::string> valueNames = {"Nearest neighbor", "Bilinear", "Bicubic", "Area", "Lanczos 8x8"};
    std::vector<cv::InterpolationFlags> values = {cv::INTER_NEAREST, cv::INTER_LINEAR, cv::INTER_CUBIC, cv::INTER_AREA, cv::INTER_LANCZOS4};

    flag = new OptionsParameter<cv::InterpolationFlags>("Interpolation", valueNames, values, f);

    oldAmplitude = amplitude->value;

    updateMappingMatrices();
}

void RadialRemap::updateMappingMatrices()
{
    mapX = cv::Mat(size, CV_32FC1);
    mapY = cv::Mat(size, CV_32FC1);

    float rMax = 0.5 * mapX.cols;
    float centerX = 0.5 * mapX.cols;
    float centerY = 0.5 * mapX.cols;

    float pi = 3.14159265359;

    for (int i = 0; i < mapX.rows; i++)
    {
        for (int j = 0; j < mapX.cols; j++)
        {
            float x = j - centerX;
            float y = i - centerY;
            float r = sqrtf(x * x + y * y);
            float f = 0.0;
            //if (r != 0.0) f = 0.5* amplitude * (1.0 + cosf(pi * r / rMax)) / r;
            //if (r != 0.0) f = amplitude * (1.0 - r / rMax) / r;
            //f = amplitude / rMax;
            if (r != 0.0) f = amplitude->value * sinf(2.0 * pi * 7 * r / rMax) / r;
            mapX.at<float>(i, j) = centerX + x * (1.0 + f);
            mapY.at<float>(i, j) = centerY + y * (1.0 + f);
        }
    }
}

cv::Mat RadialRemap::applyOperation(const cv::Mat &src)
{
    if (src.size() != size)
    {
        size = src.size();
        updateMappingMatrices();
    }
    if (amplitude->value != oldAmplitude)
    {
        oldAmplitude = amplitude->value;
        updateMappingMatrices();
    }
    cv::Mat dst;
    cv::remap(src, dst, mapX, mapY, flag->value, cv::BORDER_TRANSPARENT);
    return dst;
}

// Rotation

std::string Rotation::name = "Rotation";

Rotation::Rotation(bool on, double a, double s, cv::InterpolationFlags f): ImageOperation(on)
{
    angle = new DoubleParameter("Angle", a, -360.0, 360.0);
    scale = new DoubleParameter("Scale", s, 0.0, 2.0);

    std::vector<std::string> valueNames = {"Nearest neighbor", "Bilinear", "Bicubic", "Area", "Lanczos 8x8"};
    std::vector<cv::InterpolationFlags> values = {cv::INTER_NEAREST, cv::INTER_LINEAR, cv::INTER_CUBIC, cv::INTER_AREA, cv::INTER_LANCZOS4};

    flag = new OptionsParameter<cv::InterpolationFlags>("Interpolation", valueNames, values, f);
}

cv::Mat Rotation::applyOperation(const cv::Mat &src)
{
    cv::Point center = cv::Point(src.cols / 2, src.rows / 2);
    cv::Mat rotationMat = cv::getRotationMatrix2D(center, angle->value, scale->value);
    cv::Mat dst;
    cv::warpAffine(src, dst, rotationMat, src.size(), flag->value);
    return dst;
}

// Sharpen

std::string Sharpen::name = "Sharpen";

Sharpen::Sharpen(bool on, double s, double t, double a): ImageOperation(on)
{
    sigma = new DoubleParameter("Sigma", s, 0.0, 10.0);
    threshold = new DoubleParameter("Threshold", t, 0.0, 100.0);
    amount = new DoubleParameter("Amount", a, 0.0, 10.0);
}

cv::Mat Sharpen::applyOperation(const cv::Mat &src)
{
    cv::Mat blurred;
    cv::GaussianBlur(src, blurred, cv::Size(), sigma->value, sigma->value, cv::BORDER_ISOLATED);
    cv::Mat lowContrastMask = abs(src - blurred) < threshold->value;
    cv::Mat dst = src * (1 + amount->value) + blurred * (-amount->value);
    src.copyTo(dst, lowContrastMask);
    return dst;
}

std::string ShiftHue::name = "Shift hue";

ShiftHue::ShiftHue(bool on, int d): ImageOperation(on)
{
    delta = new IntParameter("Delta", d, -180, 180);
}

cv::Mat ShiftHue::applyOperation(const cv::Mat &src)
{
    cv::Mat hsv;
    cv::cvtColor(src, hsv, cv::COLOR_BGR2HSV);

    hsv.forEach<cv::Vec3b>([&](cv::Vec3b &pixel, const int*){ pixel[0] = (pixel[0] + delta->value) % 180; });

    cv::Mat dst;
    cv::cvtColor(hsv, dst, cv::COLOR_HSV2BGR);

    return dst;
}
