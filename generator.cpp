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

#include "generator.h"

void Pipeline::iterate()
{
    for (auto operation: imageOperations)
        if (operation->isEnabled())
            image = operation->applyOperation(image);
}

Pipeline::~Pipeline()
{
    for (auto &operation: imageOperations)
    {
        delete operation;
    }
}

void Pipeline::swapImageOperations(int operationIndex0, int operationIndex1)
{
    ImageOperation *operation = imageOperations[operationIndex0];
    std::vector<ImageOperation*>::iterator it = imageOperations.begin();
    imageOperations.erase(it + operationIndex0);
    imageOperations.insert(it + operationIndex1, operation);
}

void Pipeline::removeImageOperation(int operationIndex)
{
    std::vector<ImageOperation*>::iterator it = imageOperations.begin();
    imageOperations.erase(it + operationIndex);
}

void Pipeline::insertImageOperation(int newOperationIndex, int currentOperationIndex)
{
    std::vector<ImageOperation*>::iterator it = imageOperations.begin();

    if (newOperationIndex == 0)
    {
        imageOperations.insert(it + currentOperationIndex + 1, new Canny(false, 100, 300, 3, false));
    }
    else if (newOperationIndex == 1)
    {
        imageOperations.insert(it + currentOperationIndex + 1, new ConvertTo(false, 1.0, 0.0));
    }
    else if (newOperationIndex == 2)
    {
        imageOperations.insert(it + currentOperationIndex + 1, new EqualizeHist(false));
    }
    else if (newOperationIndex == 3)
    {
        imageOperations.insert(it + currentOperationIndex + 1, new GaussianBlur(false, 3, 0.0, 0.0));
    }
    else if (newOperationIndex == 4)
    {
        imageOperations.insert(it + currentOperationIndex + 1, new Laplacian(false, 3, 1.0, 0.0));
    }
    else if (newOperationIndex == 5)
    {
        imageOperations.insert(it + currentOperationIndex + 1, new MixChannels(false, 0, 1, 2));
    }
    else if (newOperationIndex == 6)
    {
        imageOperations.insert(it + currentOperationIndex + 1, new MorphologyEx(false, 3, 1, cv::MORPH_ERODE, cv::MORPH_RECT));
    }
    else if (newOperationIndex == 7)
    {
        imageOperations.insert(it + currentOperationIndex + 1, new Rotation(false, 0.0, 1.0, cv::INTER_NEAREST));
    }
    else if (newOperationIndex == 8)
    {
        imageOperations.insert(it + currentOperationIndex + 1, new Sharpen(false, 1.0, 5.0, 1.0));
    }
    else if (newOperationIndex == 9)
    {
        imageOperations.insert(it + currentOperationIndex + 1, new ShiftHue(false, 0));
    }
}

GeneratorCV::GeneratorCV()
{
    availableImageOperations = {
        Canny::name,
        ConvertTo::name,
        EqualizeHist::name,
        GaussianBlur::name,
        Laplacian::name,
        MixChannels::name,
        MorphologyEx::name,
        Rotation::name,
        Sharpen::name,
        ShiftHue::name
    };

    imageSize = 700;

    cv::Mat img = cv::Mat::zeros(imageSize, imageSize, CV_8UC3);

    pipelines.push_back(new Pipeline(img));
    pipelines.push_back(new Pipeline(img));

    pipelines[0]->blendFactor = 0.5;
    pipelines[1]->blendFactor = 0.5;

    colorScaleFactor = 1.0 / (imageSize * imageSize * 255);

    histogramSize = 256;

    iteration = 0;

    selectingPixel = false;

    setMask();
    computeHistogramMax();
    drawSeed(true);
    initImageOperations();

    cv::namedWindow("Out image", cv::WINDOW_AUTOSIZE);
    cv::setMouseCallback("Out image", onMouse, this);
    selectedPixel = cv::Point(imageSize / 2, imageSize / 2);
}

GeneratorCV::~GeneratorCV()
{
    cv::destroyWindow("Out image");
}

void GeneratorCV::setMask()
{
    mask = cv::Mat::zeros(imageSize, imageSize, CV_8U);
    cv::Point center = cv::Point(imageSize / 2, imageSize / 2);
    cv::circle(mask, center, imageSize / 2, cv::Scalar(255, 255, 255), -1, cv::FILLED);
}

void GeneratorCV::computeHistogramMax()
{
    cv::Mat full = cv::Mat::ones(imageSize, imageSize, CV_8U);
    cv::Mat masked;
    full.copyTo(masked, mask);
    int pixelCount = cv::countNonZero(masked);
    histogramMax = pixelCount * 5.0 / 100.0;
}

void GeneratorCV::drawSeed(bool grayscale)
{
    cv::Mat seedImage = cv::Mat(imageSize, imageSize, CV_8UC3);
    cv::randu(seedImage, cv::Scalar::all(0), cv::Scalar::all(255));

    if (grayscale)
    {
        cv::cvtColor(seedImage, seedImage, cv::COLOR_BGR2GRAY);
        cv::cvtColor(seedImage, seedImage, cv::COLOR_GRAY2BGR);
    }

    cv::Mat maskedSeed = cv::Mat::zeros(imageSize, imageSize, CV_8UC3);
    seedImage.copyTo(maskedSeed, mask);

    for (auto &pipeline: pipelines)
        pipeline->image = maskedSeed.clone();

    cv::imshow("Out image", maskedSeed);
}

void GeneratorCV::initImageOperations()
{
    pipelines[0]->imageOperations.clear();
    pipelines[0]->imageOperations.push_back(new MorphologyEx(true, 3, 1, cv::MORPH_DILATE, cv::MORPH_ELLIPSE));
    pipelines[0]->imageOperations.push_back(new Rotation(true, 36.0, 1.005, cv::INTER_LINEAR));

    pipelines[1]->imageOperations.clear();
    pipelines[1]->imageOperations.push_back(new ConvertTo(true, 0.85, 0.0));
    pipelines[1]->imageOperations.push_back(new Sharpen(true, 1.0, 5.0, 1.0));
}

void GeneratorCV::blendImages()
{
    cv::Mat blendImage = cv::Mat::zeros(imageSize, imageSize, CV_8UC3);

    for (auto pipeline: pipelines)
        cv::addWeighted(blendImage, 1.0, pipeline->image, pipeline->blendFactor, 0.0, blendImage);

    blendImage.copyTo(outImage, mask);
}

void GeneratorCV::iterate()
{
    for (auto pipeline: pipelines)
        pipeline->iterate();

    blendImages();

    for (auto &pipeline: pipelines)
        pipeline->image = outImage.clone();

    iteration++;
}

void GeneratorCV::computeBGRSum()
{
    bgrSum = cv::sum(outImage);
}

void GeneratorCV::computeBGRPixel()
{
    bgrPixel = outImage.at<cv::Vec3b>(selectedPixel);
}

void GeneratorCV::computeHistogram()
{
    float range[] = {0, 256};
    const float *histogramRange[] = {range};

    cv::Mat bgrChannels[3];
    cv::split(outImage, bgrChannels);

    cv::calcHist(&bgrChannels[0], 1, 0, mask, blueHistogram, 1, &histogramSize, histogramRange, true, false);
    cv::calcHist(&bgrChannels[1], 1, 0, mask, greenHistogram, 1, &histogramSize, histogramRange, true, false);
    cv::calcHist(&bgrChannels[2], 1, 0, mask, redHistogram, 1, &histogramSize, histogramRange, true, false);
}

void GeneratorCV::showPixelSelectionCursor()
{
    cv::Mat layer = cv::Mat::zeros(imageSize, imageSize, CV_8UC3);
    cv::line(layer, cv::Point(0, selectedPixel.y), cv::Point(imageSize, selectedPixel.y), cv::Scalar(255, 255, 255));
    cv::line(layer, cv::Point(selectedPixel.x, 0), cv::Point(selectedPixel.x, imageSize), cv::Scalar(255, 255, 255));
    cv::addWeighted(outImage, 0.5, layer, 0.5, 0.0, layer);
    cv::imshow("Out image", layer);
}

void GeneratorCV::showImage()
{
    cv::imshow("Out image", outImage);
}

void GeneratorCV::onMouse(int event, int x, int y, int, void *userdata)
{
    GeneratorCV *mw = reinterpret_cast<GeneratorCV*>(userdata);

    if (event == cv::EVENT_LBUTTONDOWN)
    {
        mw->selectPixel(x, y);
    }
}

void GeneratorCV::selectPixel(int x, int y)
{
    if (selectingPixel)
    {
        selectedPixel = cv::Point(x, y);
    }
}

void GeneratorCV::writeImage(std::string filename)
{
    std::vector<int> params = {cv::IMWRITE_JPEG_QUALITY, 100};
    cv::imwrite(filename, outImage, params);
}

void GeneratorCV::setImageSize(int size)
{
    imageSize = size;

    colorScaleFactor = 1.0 / (imageSize * imageSize * 255);

    for (auto &pipeline: pipelines)
    {
        cv::Mat dst;
        cv::resize(pipeline->image, dst, cv::Size(imageSize, imageSize));
        pipeline->image = dst.clone();
    }

    setMask();
    computeHistogramMax();

    cv::imshow("Out image", pipelines[0]->image);

    selectedPixel = cv::Point(imageSize / 2, imageSize / 2);
}

QVector<double> GeneratorCV::getBlueHistogram()
{
    QVector<double> histogram(histogramSize);

    for (int i = 0; i < histogramSize; i++)
        histogram[i] = blueHistogram.at<float>(i);

    return histogram;
}

QVector<double> GeneratorCV::getGreenHistogram()
{
    QVector<double> histogram(histogramSize);

    for (int i = 0; i < histogramSize; i++)
        histogram[i] = greenHistogram.at<float>(i);

    return histogram;
}

QVector<double> GeneratorCV::getRedHistogram()
{
    QVector<double> histogram(histogramSize);

    for (int i = 0; i < histogramSize; i++)
        histogram[i] = redHistogram.at<float>(i);

    return histogram;
}

QVector<double> GeneratorCV::getHistogramBins()
{
    QVector<double> bins(histogramSize);

    for (int i = 0; i < histogramSize; i++)
        bins[i] = i;

    return bins;
}

QVector<double> GeneratorCV::getColorComponents(int colorIndex)
{
    QVector<double> components;

    cv::MatIterator_<cv::Vec3b> it, end;
    for (it = outImage.begin<cv::Vec3b>(), end = outImage.end<cv::Vec3b>(); it != end; ++it)
        components.push_back((*it)[colorIndex]);

    return components;
}

void GeneratorCV::swapImageOperations(int imageIndex, int operationIndex0, int operationIndex1)
{
    pipelines[imageIndex]->swapImageOperations(operationIndex0, operationIndex1);
}

void GeneratorCV::removeImageOperation(int pipelineIndex, int operationIndex)
{
    pipelines[pipelineIndex]->removeImageOperation(operationIndex);
}

void GeneratorCV::insertImageOperation(int pipelineIndex, int newOperationIndex, int currentOperationIndex)
{
    pipelines[pipelineIndex]->insertImageOperation(newOperationIndex, currentOperationIndex);
}

void GeneratorCV::removePipeline(int pipelineIndex)
{
    if (!pipelines.empty())
    {
        pipelines.erase(pipelines.begin() + pipelineIndex);

        // Recompute blend factors (their sum = 1)

        double sumBlendFactors = 0.0;

        for (auto pipeline: pipelines)
            sumBlendFactors += pipeline->blendFactor;

        if (sumBlendFactors == 0.0)
        {
            for (auto &pipeline: pipelines)
                pipeline->blendFactor = 1.0 / pipelines.size();
        }
        else
        {
            for (auto &pipeline: pipelines)
                pipeline->blendFactor /= sumBlendFactors;
        }
    }
}

void GeneratorCV::addPipeline()
{
    pipelines.push_back(new Pipeline(outImage));
    pipelines.back()->blendFactor = 0.0;
}

void GeneratorCV::setPipelineBlendFactor(int pipelineIndex, double factor)
{
    double sumBlendFactors = 0.0;

    for (int i = 0; i < static_cast<int>(pipelines.size()); i++)
        if (i != pipelineIndex)
            sumBlendFactors += pipelines[i]->blendFactor;

    if (sumBlendFactors == 0.0)
    {
        for (int i = 0; i < static_cast<int>(pipelines.size()); i++)
            if (i != pipelineIndex)
                pipelines[i]->blendFactor = 1.0e-6;

        sumBlendFactors = 0.0;

        for (int i = 0; i < static_cast<int>(pipelines.size()); i++)
            if (i != pipelineIndex)
                sumBlendFactors += pipelines[i]->blendFactor;
    }

    // Scaling the blend factors like this ensures that its sum is unity

    double scale = (1.0 - factor) / sumBlendFactors;

    for (int i = 0; i < static_cast<int>(pipelines.size()); i++)
        if (i != pipelineIndex)
            pipelines[i]->blendFactor *= scale;

    pipelines[pipelineIndex]->blendFactor = factor;
}

void GeneratorCV::equalizePipelineBlendFactors()
{
    for (auto &pipeline: pipelines)
        pipeline->blendFactor = 1.0 / pipelines.size();
}
