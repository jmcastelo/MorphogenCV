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
    diameter = new IntParameter("Diameter", d, 0, 50, false);
    sigmaColor = new DoubleParameter("Sigma color", sc, 0.0, 300.0, 0.0, 1.0e6);
    sigmaSpace = new DoubleParameter("Sigma space", ss, 0.0, 300.0, 0.0, 1.0e6);
}

void BilateralFilter::applyOperation(cv::Mat &src)
{
    cv::Mat dst(src.rows, src.cols, src.type());
    cv::bilateralFilter(src, dst, diameter->value, sigmaColor->value, sigmaSpace->value, cv::BORDER_ISOLATED);
    src = dst.clone();
}

// Blur

std::string Blur::name = "Blur: homogeneous";

Blur::Blur(bool on, int size): ImageOperation(on)
{
    ksize = new IntParameter("Kernel size", size, 1, 51, true);
}

void Blur::applyOperation(cv::Mat &src)
{
    cv::blur(src, src, cv::Size(ksize->value, ksize->value), cv::Point(-1, -1), cv::BORDER_ISOLATED);
}

// Canny

std::string Canny::name = "Canny";

Canny::Canny(bool on, double th1, double th2, int size, bool g): ImageOperation(on)
{
    threshold1 = new DoubleParameter("Threshold 1", th1, 0.0, 300.0, 0.0, 1.0e6);
    threshold2 = new DoubleParameter("Threshold 2", th2, 0.0, 300.0, 0.0, 1.0e6);
    apertureSize = new IntParameter("Aperture size", size, 3, 7, true);
    L2gradient = new BoolParameter("L2 gradient", g);
}

void Canny::applyOperation(cv::Mat &src)
{
    cv::Mat channels[3];
    cv::split(src, channels);

    cv::Mat dst[3];

    for (int i = 0; i < 3; i++)
    {
        cv::Mat detectedEdges;
        cv::Canny(channels[i], detectedEdges, threshold1->value, threshold2->value, apertureSize->value, L2gradient->value);
        dst[i].create(channels[i].size(), channels[i].type());
        dst[i] = cv::Scalar::all(0);
        channels[i].copyTo(dst[i], detectedEdges);
    }

    cv::merge(channels, 3, src);
}

// Convert to

std::string ConvertTo::name = "Contrast/brightness";

ConvertTo::ConvertTo(bool on, double a, double b): ImageOperation(on)
{
    alpha = new DoubleParameter("Gain", a, 0.0, 10.0, -1.0e6, 1.0e6);
    beta = new DoubleParameter("Bias", b, -255.0, 255.0, -1.0e6, 1.0e6);
}

void ConvertTo::applyOperation(cv::Mat &src)
{
    src.convertTo(src, -1, alpha->value, beta->value);
}

// Deblur filter

std::string DeblurFilter::name = "Deblur filter";

DeblurFilter::DeblurFilter(bool on, double r, double snr): ImageOperation(on)
{
    radius = new DoubleParameter("Radius", r, 0.0, 100.0, 0.0, 1000000.0);
    signalToNoiseRatio = new DoubleParameter("Signal to noise ratio", snr, 0.001, 10000.0, 1.0e-6, 1.0e6);
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

void DeblurFilter::applyOperation(cv::Mat &src)
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
    cv::normalize(dst, src, 0, 255, cv::NORM_MINMAX);
}

// Equalize histogram

std::string EqualizeHist::name = "Equalize histogram";

EqualizeHist::EqualizeHist(bool on): ImageOperation(on){}

void EqualizeHist::applyOperation(cv::Mat &src)
{
    cv::cvtColor(src, src, cv::COLOR_BGR2YCrCb);

    std::vector<cv::Mat> channels;
    cv::split(src, channels);

    cv::equalizeHist(channels[0], channels[0]);

    cv::merge(channels, src);

    cv::cvtColor(src, src, cv::COLOR_YCrCb2BGR);

}

// Filter 2D

std::string Filter2D::name = "Filter 2D";

Filter2D::Filter2D(bool on, std::vector<float> v): ImageOperation(on)
{
    kernel = new KernelParameter("Kernel", v, -100.0, 100.0);

    updateKernelMat();
}

void Filter2D::updateKernelMat()
{
    kernelMat = cv::Mat(kernel->values).reshape(0, 3);
    kernelMat.convertTo(kernelMat, CV_32F);
}

void Filter2D::applyOperation(cv::Mat &src)
{
    cv::filter2D(src, src, -1, kernelMat, cv::Point(-1, -1), 0.0, cv::BORDER_ISOLATED);
}

// Gamma correction

std::string GammaCorrection::name = "Gamma correction";

GammaCorrection::GammaCorrection(bool on, double g): ImageOperation(on)
{
    gamma = new DoubleParameter("Gamma", g, 0.0, 10.0, 0.0, 1.0e6);
}

void GammaCorrection::applyOperation(cv::Mat &src)
{
    cv::Mat lookUpTable = cv::Mat(1, 256, CV_8U);
    uchar *p = lookUpTable.ptr();
    for (int i = 0; i < 256; i++)
        p[i] = cv::saturate_cast<uchar>(cv::pow(i / 255.0, gamma->value) * 255.0);

    cv::LUT(src, lookUpTable, src);
}

// Gaussian blur

std::string GaussianBlur::name = "Blur: Gaussian";

GaussianBlur::GaussianBlur(bool on, int k, double s): ImageOperation(on)
{
    ksize = new IntParameter("Kernel size", k, 0, 51, true);
    sigma = new DoubleParameter("Sigma", s, 0.001, 50.0, 1.0e-6, 1.0e6);
}

void GaussianBlur::applyOperation(cv::Mat &src)
{
    cv::GaussianBlur(src, src, cv::Size(ksize->value, ksize->value), sigma->value, sigma->value, cv::BORDER_ISOLATED);
}

// Invert colors

std::string InvertColors::name = "Invert colors";

InvertColors::InvertColors(bool on): ImageOperation(on){}

void InvertColors::applyOperation(cv::Mat &src)
{
    cv::bitwise_not(src, src);
}

// Laplacian

std::string Laplacian::name = "Laplacian";

Laplacian::Laplacian(bool on, int k): ImageOperation(on)
{
    ksize = new IntParameter("Kernel size", k, 1, 51, true);
}

void Laplacian::applyOperation(cv::Mat &src)
{
    cv::Mat channels[3];
    cv::split(src, channels);

    cv::Mat lap[3];
    cv::Laplacian(channels[0], lap[0], CV_16S, ksize->value, 1.0, 0.0, cv::BORDER_ISOLATED);
    cv::Laplacian(channels[1], lap[1], CV_16S, ksize->value, 1.0, 0.0, cv::BORDER_ISOLATED);
    cv::Laplacian(channels[2], lap[2], CV_16S, ksize->value, 1.0, 0.0, cv::BORDER_ISOLATED);

    cv::merge(lap, 3, src);

    cv::convertScaleAbs(src, src);
}

// Median blur

std::string MedianBlur::name = "Blur: median";

MedianBlur::MedianBlur(bool on, int size): ImageOperation(on)
{
    ksize = new IntParameter("Kernel size", size, 3, 51, true);
}

void MedianBlur::applyOperation(cv::Mat &src)
{
    cv::medianBlur(src, src, ksize->value);
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

void MixChannels::applyOperation(cv::Mat &src)
{
    int fromTo[] = {0, blue->value, 1, green->value, 2, red->value};
    cv::mixChannels(&src, 1, &src, 1, fromTo, 3);
}

// Morphological operations

std::string MorphologyEx::name = "Morphology operation";

MorphologyEx::MorphologyEx(bool on, int k, int its, cv::MorphTypes type, cv::MorphShapes shape): ImageOperation(on)
{
    ksize = new IntParameter("Kernel size", k, 1, 51, true);
    iterations = new IntParameter("Iterations", its, 1, 100, false);

    std::vector<std::string> typeValueNames = {"Erode", "Dilate", "Opening", "Closing", "Gradient", "Top hat", "Black hat"};
    std::vector<cv::MorphTypes> typeValues = {cv::MORPH_ERODE, cv::MORPH_DILATE, cv::MORPH_OPEN, cv::MORPH_CLOSE, cv::MORPH_GRADIENT, cv::MORPH_TOPHAT, cv::MORPH_BLACKHAT};

    morphType = new OptionsParameter<cv::MorphTypes>("Type", typeValueNames, typeValues, type);

    std::vector<std::string> shapeValueNames = {"Rectangle", "Cross", "Ellipse"};
    std::vector<cv::MorphShapes> shapeValues = {cv::MORPH_RECT, cv::MORPH_CROSS, cv::MORPH_ELLIPSE};

    morphShape = new OptionsParameter<cv::MorphShapes>("Shape", shapeValueNames, shapeValues, shape);
}

void MorphologyEx::applyOperation(cv::Mat &src)
{
    cv::Mat element = cv::getStructuringElement(morphShape->value, cv::Size(ksize->value, ksize->value));
    cv::morphologyEx(src, src, morphType->value, element, cv::Point(-1, -1), iterations->value, cv::BORDER_ISOLATED);
}

// Pixelate

std::string Pixelate::name = "Pixelate";

Pixelate::Pixelate(bool on, int size): ImageOperation(on)
{
    pixelSize = new IntParameter("Pixel size", size, 1, 50, false);
}

void Pixelate::applyOperation(cv::Mat &src)
{
    cv::Mat dst(src.rows, src.cols, src.type());

    cv::Rect square;

    for (int row = 0; row < src.rows; row += pixelSize->value)
    {
        square.y = row;
        square.height = row + pixelSize->value < src.rows ? pixelSize->value : src.rows - row;

        for (int col = 0; col < src.cols; col += pixelSize->value)
        {
            square.x = col;
            square.width = col + pixelSize->value < src.cols ? pixelSize->value : src.cols - col;

            cv::Scalar meanColor = cv::mean(cv::Mat(src, square));
            cv::rectangle(dst, square, meanColor, cv::FILLED);
        }
    }

    src = dst.clone();
}

// Radial remap

std::string RadialRemap::name = "Radial remap";

RadialRemap::RadialRemap(bool on, double a, int rf, cv::InterpolationFlags f): ImageOperation(on)
{
    amplitude = new DoubleParameter("Amplitude", a, -5.0, 5.0, -1.0e6, 1.0e6);

    std::vector<std::string> radialFunctionValueNames = {"Linear inc.", "Linear dec.", "Cosine inc.", "Cosine dec."};
    std::vector<int> radialFuncionValues = {0, 1, 2, 3};

    radialFunction = new OptionsParameter<int>("Function", radialFunctionValueNames, radialFuncionValues, rf);

    std::vector<std::string> flagValueNames = {"Nearest neighbor", "Bilinear", "Bicubic", "Area", "Lanczos 8x8"};
    std::vector<cv::InterpolationFlags> flagValues = {cv::INTER_NEAREST, cv::INTER_LINEAR, cv::INTER_CUBIC, cv::INTER_AREA, cv::INTER_LANCZOS4};

    flag = new OptionsParameter<cv::InterpolationFlags>("Interpolation", flagValueNames, flagValues, f);

    oldAmplitude = amplitude->value;
    oldRadialFunction = radialFunction->value;

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

    if (radialFunction->value == 0) // Linear inc.
    {
        float f = amplitude->value / rMax;

        for (int i = 0; i < mapX.rows; i++)
        {
            for (int j = 0; j < mapX.cols; j++)
            {
                float x = j - centerX;
                float y = i - centerY;
                mapX.at<float>(i, j) = centerX + x * (1.0 + f);
                mapY.at<float>(i, j) = centerY + y * (1.0 + f);
            }
        }
    }
    else if (radialFunction->value == 1) // Linear dec.
    {
        for (int i = 0; i < mapX.rows; i++)
        {
            for (int j = 0; j < mapX.cols; j++)
            {
                float x = j - centerX;
                float y = i - centerY;
                float r = sqrtf(x * x + y * y);
                float f = 0.0;
                if (r != 0.0) f = amplitude->value * (1.0 / r - 1.0 / rMax);
                mapX.at<float>(i, j) = centerX + x * (1.0 + f);
                mapY.at<float>(i, j) = centerY + y * (1.0 + f);
            }
        }
    }
    else if (radialFunction->value == 2) // Cosine inc.
    {
        for (int i = 0; i < mapX.rows; i++)
        {
            for (int j = 0; j < mapX.cols; j++)
            {
                float x = j - centerX;
                float y = i - centerY;
                float r = sqrtf(x * x + y * y);
                float f = 0.0;
                if (r != 0.0) f = 0.5* amplitude->value * (1.0 - cosf(pi * r / rMax)) / r;
                mapX.at<float>(i, j) = centerX + x * (1.0 + f);
                mapY.at<float>(i, j) = centerY + y * (1.0 + f);
            }
        }
    }
    else if (radialFunction->value == 3) // Cosine dec.
    {
        for (int i = 0; i < mapX.rows; i++)
        {
            for (int j = 0; j < mapX.cols; j++)
            {
                float x = j - centerX;
                float y = i - centerY;
                float r = sqrtf(x * x + y * y);
                float f = 0.0;
                if (r != 0.0) f = 0.5* amplitude->value * (1.0 + cosf(pi * r / rMax)) / r;
                mapX.at<float>(i, j) = centerX + x * (1.0 + f);
                mapY.at<float>(i, j) = centerY + y * (1.0 + f);
            }
        }
    }
}

void RadialRemap::applyOperation(cv::Mat &src)
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
    if (radialFunction->value != oldRadialFunction)
    {
        oldRadialFunction = radialFunction->value;
        updateMappingMatrices();
    }
    cv::remap(src, src, mapX, mapY, flag->value, cv::BORDER_TRANSPARENT);
}

// Rotation

std::string Rotation::name = "Rotation/scaling";

Rotation::Rotation(bool on, double a, double s, cv::InterpolationFlags f): ImageOperation(on)
{
    angle = new DoubleParameter("Angle", a, -360.0, 360.0, -1.0e6, 1.0e6);
    scale = new DoubleParameter("Scale", s, 0.0, 10.0, 0.0, 1.0e6);

    std::vector<std::string> valueNames = {"Nearest neighbor", "Bilinear", "Bicubic", "Area", "Lanczos 8x8"};
    std::vector<cv::InterpolationFlags> values = {cv::INTER_NEAREST, cv::INTER_LINEAR, cv::INTER_CUBIC, cv::INTER_AREA, cv::INTER_LANCZOS4};

    flag = new OptionsParameter<cv::InterpolationFlags>("Interpolation", valueNames, values, f);
}

void Rotation::applyOperation(cv::Mat &src)
{
    cv::Point center = cv::Point(src.cols / 2, src.rows / 2);
    cv::Mat rotationMat = cv::getRotationMatrix2D(center, angle->value, scale->value);
    cv::warpAffine(src, src, rotationMat, src.size(), flag->value);
}

// Sharpen

std::string Sharpen::name = "Sharpen";

Sharpen::Sharpen(bool on, double s, double t, double a): ImageOperation(on)
{
    sigma = new DoubleParameter("Sigma", s, 0.001, 10.0, 1.0e-6, 1.0e6);
    threshold = new DoubleParameter("Threshold", t, 0.0, 100.0, 0.0, 1.0e6);
    amount = new DoubleParameter("Amount", a, 0.0, 10.0, -1.0e6, 1.0e6);
}

void Sharpen::applyOperation(cv::Mat &src)
{
    cv::Mat blurred;
    cv::GaussianBlur(src, blurred, cv::Size(), sigma->value, sigma->value, cv::BORDER_ISOLATED);
    cv::Mat lowContrastMask = abs(src - blurred) < threshold->value;
    src = src * (1 + amount->value) + blurred * (-amount->value);
    src.copyTo(src, lowContrastMask);
}

// Shift hue

std::string ShiftHue::name = "Shift hue";

ShiftHue::ShiftHue(bool on, int d): ImageOperation(on)
{
    delta = new IntParameter("Delta", d, -180, 180, false);
}

void ShiftHue::applyOperation(cv::Mat &src)
{
    cv::Mat hsv;
    cv::cvtColor(src, hsv, cv::COLOR_BGR2HSV);

    hsv.forEach<cv::Vec3b>([&](cv::Vec3b &pixel, const int*){ pixel[0] = (pixel[0] + delta->value) % 180; });

    cv::cvtColor(hsv, src, cv::COLOR_HSV2BGR);
}
