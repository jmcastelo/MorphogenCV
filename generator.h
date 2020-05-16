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

#ifndef GENERATOR_H
#define GENERATOR_H

#include "imageoperations.h"
#include <vector>
#include <string>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <QVector>
#include <QString>
#include <QWidget>

class GeneratorCV
{
    cv::Mat mask;

    std::vector<cv::Mat> images;
    std::vector<std::vector<ImageOperation*>> imageOperations;

    cv::Mat outImage;

    int iteration;

    int imageSize;

    int histogramSize;
    cv::Mat blueHistogram;
    cv::Mat greenHistogram;
    cv::Mat redHistogram;

    double histogramMax;
    double colorScaleFactor;
    double blendFactor;

    cv::Scalar bgrSum;
    cv::Vec3b bgrPixel;

    cv::Point selectedPixel;

    void setMask();
    void computeHistogramMax();
    void initImageOperations();
    void applyImageOperations();
    static void onMouse(int event, int x, int y, int, void* userdata);
    void selectPixel(int x, int y);

public:
    std::vector<QString> availableImageOperations;

    bool selectingPixel;

    GeneratorCV();
    ~GeneratorCV();

    void drawSeed(bool grayscale);

    void iterate();
    void computeBGRSum();
    void computeBGRPixel();
    void computeHistogram();
    void showPixelSelectionCursor();
    void showImage();

    void writeImage(std::string filename);

    int getIterationNumber(){ return iteration; };

    int getHistogramMax(){ return histogramMax; };

    void setImageSize(int size);
    int getImageSize(){ return imageSize; };

    void setBlendFactor(double factor){ blendFactor = factor; };
    double getBlendFactor() { return blendFactor; };

    double getBSum(){ return bgrSum[0] * colorScaleFactor; };
    double getGSum(){ return bgrSum[1] * colorScaleFactor; };
    double getRSum(){ return bgrSum[2] * colorScaleFactor; };

    double getPixelComponent(int colorIndex){ return bgrPixel[colorIndex]; };

    QVector<double> getBlueHistogram();
    QVector<double> getGreenHistogram();
    QVector<double> getRedHistogram();
    QVector<double> getHistogramBins();

    QVector<double> getColorComponents(int colorIndex);

    void swapImageOperations(int imageIndex, int operationIndex0, int operationIndex1);
    void removeImageOperation(int imageIndex, int operationIndex);
    void insertImageOperation(int imageIndex, int newOperationIndex, int currentOperationIndex);
    QString getImageOperationName(int imageIndex, int operationIndex){ return imageOperations[imageIndex][operationIndex]->getName(); };
    int getImageOperationsSize(int imageIndex){ return imageOperations[imageIndex].size(); };
    QWidget* getImageOperationParametersWidget(int imageIndex, int operationIndex){ return imageOperations[imageIndex][operationIndex]->getParametersWidget(); };
};

#endif // GENERATOR_H
