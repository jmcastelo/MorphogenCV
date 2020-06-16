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
#include <opencv2/imgcodecs.hpp>
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
    cv::Mat seedImage;
    cv::Mat outputImage;

    int iteration;

    int imageSize;

    bool circularMask;

    cv::Scalar plainColor;

    int histogramSize;
    cv::Mat blueHistogram;
    cv::Mat greenHistogram;
    cv::Mat redHistogram;

    double histogramMax;
    double colorScaleFactor;

    cv::Scalar bgrSum;
    cv::Vec3b bgrPixel;

    cv::Point selectedPixel;

    cv::Mat pointerCanvas;
    bool pointerCanvasDrawn;
    cv::Point pointer;
    int pointerRadius;
    int pointerThickness;
    cv::Scalar pointerColor;

    int framesPerSecond;
    int frameCount;
    cv::VideoWriter videoWriter;

    void setMask();
    void computeHistogramMax();
    void applyImageOperations();
    void blendImages();
    static void onMouse(int event, int x, int y, int flags, void* userdata);
    void processMouse(int event, int x, int y, int flags);
    void drawPointer(int x, int y);
    void drawPointerCanvas();

public:
    std::vector<std::string> availableImageOperations;

    std::vector<Pipeline*> pipelines;
    Pipeline *outputPipeline;

    bool selectingPixel;
    bool drawingPointer;
    bool persistentDrawing;

    GeneratorCV();
    ~GeneratorCV();

    void destroyAllWindows();

    void drawRandomSeed(bool grayscale);

    void drawSeedImage();
    void loadSeedImage(std::string filename);

    void drawPlainColorSeed();
    void setPlainColor(int blue, int green, int red);

    void iterate();

    void toggleMask(bool apply);

    void computeBGRSum();
    void computeBGRPixel();
    void computeHistogram();
    void computeDFT();
    void showPixelSelectionCursor();
    void showImage();

    void clearPointerCanvas();
    void drawCenteredPointer();
    void setPointerRadius(int radius){ if (radius > 0 && radius < imageSize / 2) pointerRadius = radius;}
    int getPointerRadius(){ return pointerRadius; }
    void setPointerThickness(int thickness){ if (thickness >= -1) pointerThickness = thickness; }
    int getPointerThickness(){ return pointerThickness; }

    void setPointerColor(int red, int green, int blue){ pointerColor = cv::Scalar(blue, green, red); }

    void openVideoWriter(std::string name);
    void writeVideoFrame();
    void closeVideoWriter();

    void setFramesPerSecond(int fps){ framesPerSecond = fps; }
    int getFramesPerSecond(){ return framesPerSecond; }
    int getFrameCount(){ return frameCount; }

    void resetIterationNumer() { iteration = 0; }
    int getIterationNumber(){ return iteration; }

    int getHistogramMax(){ return histogramMax; }

    void setImageSize(int size);
    int getImageSize(){ return imageSize; }

    double getBSum(){ return bgrSum[0] * colorScaleFactor; }
    double getGSum(){ return bgrSum[1] * colorScaleFactor; }
    double getRSum(){ return bgrSum[2] * colorScaleFactor; }

    double getPixelComponent(int colorIndex){ return bgrPixel[colorIndex]; }

    QVector<double> getBlueHistogram();
    QVector<double> getGreenHistogram();
    QVector<double> getRedHistogram();
    QVector<double> getHistogramBins();

    QVector<double> getColorComponents(int colorIndex);

    void swapImageOperations(int pipelineIndex, int operationIndex0, int operationIndex1);
    void removeImageOperation(int pipelineIndex, int operationIndex);
    void insertImageOperation(int pipelineIndex, int newOperationIndex, int currentOperationIndex);

    std::string getImageOperationName(int pipelineIndex, int operationIndex)
    {
        if (pipelineIndex >= 0)
            return pipelines[pipelineIndex]->imageOperations[operationIndex]->getName();
        else
            return outputPipeline->imageOperations[operationIndex]->getName();
    }
    int getImageOperationsSize(int pipelineIndex)
    {
        if (pipelineIndex >= 0)
            return pipelines.empty() ? 0 : pipelines[pipelineIndex]->imageOperations.size();
        else
            return outputPipeline->imageOperations.size();
    }

    void removePipeline(int pipelineIndex);
    void addPipeline();
    void loadPipeline(double blendFactor);
    int getPipelinesSize(){ return pipelines.size(); };

    void setPipelineBlendFactor(int pipelineIndex, double factor);
    double getPipelineBlendFactor(int pipelineIndex){ return pipelines[pipelineIndex]->blendFactor; }
    void equalizePipelineBlendFactors();
};

#endif // GENERATOR_H
