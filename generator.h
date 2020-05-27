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
#include <opencv2/highgui.hpp>
#include <opencv2/videoio.hpp>
#include <QVector>

class Pipeline
{
    std::vector<std::string> availableImageOperations;

public:
    cv::Mat image;
    std::vector<ImageOperation*> imageOperations;

    double blendFactor;

    Pipeline(cv::Mat img);
    ~Pipeline();

    void iterate();

    void swapImageOperations(int operationIndex0, int operationIndex1);
    void removeImageOperation(int operationIndex);
    void insertImageOperation(int newOperationIndex, int currentOperationIndex);
    void loadImageOperation(
            std::string operationName,
            bool enabled,
            std::vector<bool> boolParameters,
            std::vector<int> intParameters,
            std::vector<double> doubleParameters,
            std::vector<int> morphTypeParameters,
            std::vector<int> morphShapeParameters,
            std::vector<int> interpolationFlagParameters,
            std::vector<float> kernelElements);
};

class GeneratorCV
{
    cv::Mat mask;
    cv::Mat outImage;
    std::vector<cv::Mat> previousFrames;

    int iteration;

    int imageSize;

    int previousFramesSize;
    double previousFramesBlendFactor;

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

    int framesPerSecond;
    int frameCount;
    cv::VideoWriter videoWriter;

    void setMask();
    void computeHistogramMax();
    void applyImageOperations();
    void blendImages();
    void blendPreviousImages();
    static void onMouse(int event, int x, int y, int, void* userdata);
    void selectPixel(int x, int y);

public:
    std::vector<std::string> availableImageOperations;

    std::vector<Pipeline*> pipelines;

    bool selectingPixel;

    GeneratorCV();
    ~GeneratorCV();

    void drawSeed(bool grayscale);

    void iterate();

    void computeBGRSum();
    void computeBGRPixel();
    void computeHistogram();
    void computeDFT();
    void showPixelSelectionCursor();
    void showImage();

    void openVideoWriter(std::string name);
    void writeVideoFrame();
    void closeVideoWriter();

    void setPreviousFramesSize(int n);
    int getPreviousFramesSize(){ return previousFramesSize; }

    void setPreviousFramesBlendFactor(double factor){ if (factor >= 0.0) previousFramesBlendFactor = factor; }
    double getPreviousFramesBlendFactor(){ return previousFramesBlendFactor; }

    void setFramesPerSecond(int fps){ framesPerSecond = fps; }
    int getFramesPerSecond(){ return framesPerSecond; }
    int getFrameCount(){ return frameCount; }

    int getIterationNumber(){ return iteration; };

    int getHistogramMax(){ return histogramMax; };

    void setImageSize(int size);
    int getImageSize(){ return imageSize; };

    double getBSum(){ return bgrSum[0] * colorScaleFactor; };
    double getGSum(){ return bgrSum[1] * colorScaleFactor; };
    double getRSum(){ return bgrSum[2] * colorScaleFactor; };

    double getPixelComponent(int colorIndex){ return bgrPixel[colorIndex]; };

    QVector<double> getBlueHistogram();
    QVector<double> getGreenHistogram();
    QVector<double> getRedHistogram();
    QVector<double> getHistogramBins();

    QVector<double> getColorComponents(int colorIndex);

    void swapImageOperations(int pipelineIndex, int operationIndex0, int operationIndex1);
    void removeImageOperation(int pipelineIndex, int operationIndex);
    void insertImageOperation(int pipelineIndex, int newOperationIndex, int currentOperationIndex);

    std::string getImageOperationName(int pipelineIndex, int operationIndex){ return pipelines[pipelineIndex]->imageOperations[operationIndex]->getName(); };
    int getImageOperationsSize(int pipelineIndex){ return pipelines.empty() ? 0 : pipelines[pipelineIndex]->imageOperations.size(); };

    void removePipeline(int pipelineIndex);
    void addPipeline();
    void loadPipeline(double blendFactor);
    int getPipelinesSize(){ return pipelines.size(); };

    void setPipelineBlendFactor(int pipelineIndex, double factor);
    double getPipelineBlendFactor(int pipelineIndex){ return pipelines[pipelineIndex]->blendFactor; };
    void equalizePipelineBlendFactors();
};

#endif // GENERATOR_H
