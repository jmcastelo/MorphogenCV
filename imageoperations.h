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

#include "parameter.h"
#include <opencv2/imgproc.hpp>
#include <opencv2/photo.hpp>
#include <vector>
#include <string>
#include <cmath>

// Base image operation class

class ImageOperation
{
public:
    bool enabled;
    bool isEnabled(){ return enabled; }

    virtual std::string getName() = 0;

    virtual std::vector<BoolParameter*> getBoolParameters(){ std::vector<BoolParameter*> parameters; return parameters; };
    virtual std::vector<IntParameter*> getIntParameters(){ std::vector<IntParameter*> parameters; return parameters; };
    virtual std::vector<DoubleParameter*> getDoubleParameters(){ std::vector<DoubleParameter*> parameters; return parameters; };
    virtual std::vector<OptionsParameter<int>*> getOptionsIntParameters(){ std::vector<OptionsParameter<int>*> parameters; return parameters; }
    virtual std::vector<OptionsParameter<cv::MorphTypes>*> getMorphTypeParameters(){ std::vector<OptionsParameter<cv::MorphTypes>*> parameters; return parameters; };
    virtual std::vector<OptionsParameter<cv::MorphShapes>*> getMorphShapeParameters(){ std::vector<OptionsParameter<cv::MorphShapes>*> parameters; return parameters; };
    virtual std::vector<OptionsParameter<cv::InterpolationFlags>*> getInterpolationFlagParameters(){ std::vector<OptionsParameter<cv::InterpolationFlags>*> parameters; return parameters; };
    virtual KernelParameter* getKernelParameter(){ return nullptr; }

    void adjustMinMax(double value, double minValue, double maxValue, double &min, double &max)
    {
        if (value < minValue) min = value;
        else min = minValue;

        if (value > maxValue) max = value;
        else max = maxValue;
    }

    virtual void applyOperation(cv::Mat &src) = 0;

    ImageOperation(bool on): enabled(on){};
    virtual ~ImageOperation(){};
};

// Bilateral filter

class BilateralFilter: public ImageOperation
{
    IntParameter *diameter;
    DoubleParameter *sigmaColor, *sigmaSpace;

public:
    static std::string name;

    BilateralFilter(bool on, int d, double sc, double ss);
    ~BilateralFilter()
    {
        delete diameter;
        delete sigmaColor;
        delete sigmaSpace;
    }

    std::string getName(){ return name; };

    std::vector<IntParameter*> getIntParameters(){ std::vector<IntParameter*> parameters = {diameter}; return parameters; };
    std::vector<DoubleParameter*> getDoubleParameters(){ std::vector<DoubleParameter*> parameters = {sigmaColor, sigmaSpace}; return parameters; };

    void applyOperation(cv::Mat &src);
};

// Blur

class Blur: public ImageOperation
{
    IntParameter *ksize;

public:
    static std::string name;

    Blur(bool on, int size);
    ~Blur(){ delete ksize; }

    std::string getName(){ return name; };

    std::vector<IntParameter*> getIntParameters(){ std::vector<IntParameter*> parameters = {ksize}; return parameters; };

    void applyOperation(cv::Mat &src);
};

// Canny

class Canny: public ImageOperation
{
    DoubleParameter *threshold1, *threshold2;
    IntParameter *apertureSize;
    BoolParameter *L2gradient;

public:
    static std::string name;

    Canny(bool on, double th1, double th2, int size, bool g);
    ~Canny()
    {
        delete threshold1;
        delete threshold2;
        delete apertureSize;
        delete L2gradient;
    }

    std::string getName(){ return name; };

    std::vector<BoolParameter*> getBoolParameters(){ std::vector<BoolParameter*> parameters = {L2gradient}; return parameters; };
    std::vector<IntParameter*> getIntParameters(){ std::vector<IntParameter*> parameters = {apertureSize}; return parameters; };
    std::vector<DoubleParameter*> getDoubleParameters(){ std::vector<DoubleParameter*> parameters = {threshold1, threshold2}; return parameters; };

    void applyOperation(cv::Mat &src);
};

// Convert to

class ConvertTo: public ImageOperation
{
    DoubleParameter *alpha, *beta;

public:
    static std::string name;

    ConvertTo(bool on, double a, double b);
    ~ConvertTo()
    {
        delete alpha;
        delete beta;
    }

    std::string getName(){ return name; };

    std::vector<DoubleParameter*> getDoubleParameters(){ std::vector<DoubleParameter*> parameters = {alpha, beta}; return parameters; };

    void applyOperation(cv::Mat &src);
};

// Deblur filter

class DeblurFilter: public ImageOperation
{
    DoubleParameter *radius, *signalToNoiseRatio;

public:
    static std::string name;

    DeblurFilter(bool on, double r, double snr);
    ~DeblurFilter()
    {
        delete radius;
        delete signalToNoiseRatio;
    }

    std::string getName(){ return name; };

    std::vector<DoubleParameter*> getDoubleParameters(){ std::vector<DoubleParameter*> parameters = {radius, signalToNoiseRatio}; return parameters; };

    void computePSF(cv::Mat &outputImg, cv::Size filterSize);
    void computeWnrFilter(const cv::Mat &input_h_PSF, cv::Mat &output_G, double nsr);
    void fftShift(const cv::Mat &inputImg, cv::Mat &outputImg);
    void filter2DFreq(const cv::Mat &inputImg, cv::Mat &outputImg, const cv::Mat &H);
    void applyOperation(cv::Mat &src);
};

// Equalize histogram

class EqualizeHist: public ImageOperation
{
public:
    static std::string name;

    EqualizeHist(bool on);

    std::string getName(){ return name; };

    void applyOperation(cv::Mat &src);
};

// Filter 2D

class Filter2D: public ImageOperation
{
    KernelParameter *kernel;

public:
    static std::string name;

    cv::Mat kernelMat;

    Filter2D(bool on, std::vector<float> v);
    ~Filter2D(){ delete kernel; }

    std::string getName(){ return name; }

    KernelParameter* getKernelParameter(){ return kernel; }

    void updateKernelMat();
    void applyOperation(cv::Mat &src);
};

// Gamma correction

class GammaCorrection: public ImageOperation
{
    DoubleParameter *gamma;

public:
    static std::string name;

    GammaCorrection(bool on, double g);
    ~GammaCorrection(){ delete gamma; }

    std::string getName(){ return name; };

    std::vector<DoubleParameter*> getDoubleParameters(){ std::vector<DoubleParameter*> parameters = {gamma}; return parameters; };

    void applyOperation(cv::Mat &src);
};

// Gaussian blur

class GaussianBlur: public ImageOperation
{
    IntParameter *ksize;
    DoubleParameter *sigma;

public:
    static std::string name;

    GaussianBlur(bool on, int k, double s);
    ~GaussianBlur()
    {
        delete ksize;
        delete sigma;
    }

    std::string getName(){ return name; };

    std::vector<IntParameter*> getIntParameters(){ std::vector<IntParameter*> parameters = {ksize}; return parameters; };
    std::vector<DoubleParameter*> getDoubleParameters(){ std::vector<DoubleParameter*> parameters = {sigma}; return parameters; };

    void applyOperation(cv::Mat &src);
};

// Invert colors

class InvertColors: public ImageOperation
{
public:
    static std::string name;

    InvertColors(bool on);

    std::string getName(){ return name; };
    void applyOperation(cv::Mat &src);
};

// Laplacian

class Laplacian: public ImageOperation
{
    IntParameter *ksize;

public:
    static std::string name;

    Laplacian(bool on, int k);
    ~Laplacian()
    {
        delete ksize;
    }

    std::string getName(){ return name; };

    std::vector<IntParameter*> getIntParameters(){ std::vector<IntParameter*> parameters = {ksize}; return parameters; };

    void applyOperation(cv::Mat &src);
};

// Median blur

class MedianBlur: public ImageOperation
{
    IntParameter *ksize;

public:
    static std::string name;

    MedianBlur(bool on, int size);
    ~MedianBlur(){ delete ksize; }

    std::string getName(){ return name; };

    std::vector<IntParameter*> getIntParameters(){ std::vector<IntParameter*> parameters = {ksize}; return parameters; };

    void applyOperation(cv::Mat &src);
};

// Mix channels

class MixChannels: public ImageOperation
{
    OptionsParameter<int> *blue, *green, *red;

public:
    static std::string name;

    MixChannels(bool on, int b, int g, int r);
    ~MixChannels()
    {
        delete blue;
        delete green;
        delete red;
    }

    std::string getName(){ return name; };

    std::vector<OptionsParameter<int>*> getOptionsIntParameters(){ std::vector<OptionsParameter<int>*> parameters = {blue, green, red}; return parameters; };

    void applyOperation(cv::Mat &src);
};

// Morphological transformations

class MorphologyEx: public ImageOperation
{
    IntParameter *ksize, *iterations;
    OptionsParameter<cv::MorphTypes> *morphType;
    OptionsParameter<cv::MorphShapes> *morphShape;

public:
    static std::string name;

    MorphologyEx(bool on, int k, int its, cv::MorphTypes t, cv::MorphShapes s);
    ~MorphologyEx()
    {
        delete ksize;
        delete morphType;
        delete morphShape;
    }

    std::string getName(){ return name; };

    std::vector<IntParameter*> getIntParameters(){ std::vector<IntParameter*> parameters = {ksize, iterations}; return parameters; };
    std::vector<OptionsParameter<cv::MorphTypes>*> getMorphTypeParameters(){ std::vector<OptionsParameter<cv::MorphTypes>*> parameters = {morphType}; return parameters; };
    std::vector<OptionsParameter<cv::MorphShapes>*> getMorphShapeParameters(){ std::vector<OptionsParameter<cv::MorphShapes>*> parameters = {morphShape}; return parameters; };

    void applyOperation(cv::Mat &src);
};

// Pixelate

class Pixelate: public ImageOperation
{
    IntParameter *pixelSize;

public:
    static std::string name;

    Pixelate(bool on, int size);
    ~Pixelate(){ delete pixelSize; }

    std::string getName(){ return name; };

    std::vector<IntParameter*> getIntParameters(){ std::vector<IntParameter*> parameters = {pixelSize}; return parameters; };

    void applyOperation(cv::Mat &src);
};

// Radial remap

class RadialRemap: public ImageOperation
{
    DoubleParameter *amplitude;
    OptionsParameter<int> *radialFunction;
    OptionsParameter<cv::InterpolationFlags> *flag;
    double oldAmplitude;
    int oldRadialFunction;

    cv::Size size;
    cv::Mat mapX, mapY;
    void updateMappingMatrices();

public:
    static std::string name;

    RadialRemap(bool on, double a, int rf, cv::InterpolationFlags f);
    ~RadialRemap()
    {
        delete amplitude;
        delete flag;
    }

    std::string getName(){ return name; };

    std::vector<DoubleParameter*> getDoubleParameters(){ std::vector<DoubleParameter*> parameters = {amplitude}; return parameters; };
    std::vector<OptionsParameter<int>*> getOptionsIntParameters(){ std::vector<OptionsParameter<int>*> parameters = {radialFunction}; return parameters; };
    std::vector<OptionsParameter<cv::InterpolationFlags>*> getInterpolationFlagParameters(){ std::vector<OptionsParameter<cv::InterpolationFlags>*> parameters = {flag}; return parameters; };

    void applyOperation(cv::Mat &src);
};

// Rotation

class Rotation: public ImageOperation
{
    DoubleParameter *angle, *scale;
    OptionsParameter<cv::InterpolationFlags> *flag;

public:
    static std::string name;

    Rotation(bool on, double a, double s, cv::InterpolationFlags f);
    ~Rotation()
    {
        delete angle;
        delete scale;
        delete flag;
    }

    std::string getName(){ return name; };

    std::vector<DoubleParameter*> getDoubleParameters(){ std::vector<DoubleParameter*> parameters = {angle, scale}; return parameters; };
    std::vector<OptionsParameter<cv::InterpolationFlags>*> getInterpolationFlagParameters(){ std::vector<OptionsParameter<cv::InterpolationFlags>*> parameters = {flag}; return parameters; };

    void applyOperation(cv::Mat &src);
};

// Saturate

class Saturate: public ImageOperation
{
    DoubleParameter *gain, *bias;

public:
    static std::string name;

    Saturate(bool on, double g, double b);
    ~Saturate()
    {
        delete gain;
        delete bias;
    }

    std::string getName(){ return name; };

    std::vector<DoubleParameter*> getDoubleParameters(){ std::vector<DoubleParameter*> parameters = {gain, bias}; return parameters; };

    void applyOperation(cv::Mat &src);
};

// Sharpen

class Sharpen: public ImageOperation
{
    DoubleParameter *sigma, *threshold, *amount;

public:
    static std::string name;

    Sharpen(bool on, double s, double t, double a);
    ~Sharpen()
    {
        delete sigma;
        delete threshold;
        delete amount;
    }

    std::string getName(){ return name; };

    std::vector<DoubleParameter*> getDoubleParameters(){ std::vector<DoubleParameter*> parameters = {sigma, threshold, amount}; return parameters; };

    void applyOperation(cv::Mat &src);
};

// Shift hue

class ShiftHue: public ImageOperation
{
    IntParameter *delta;

public:
    static std::string name;

    ShiftHue(bool on, int d);
    ~ShiftHue(){ delete delta; }

    std::string getName(){ return name; };

    std::vector<IntParameter*> getIntParameters(){ std::vector<IntParameter*> parameters = {delta}; return parameters; };

    void applyOperation(cv::Mat &src);
};

#endif // IMAGEOPERATIONS_H
