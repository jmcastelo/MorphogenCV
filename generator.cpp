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

Pipeline::Pipeline(cv::Mat img): image(img)
{
    // Duplicate of GeneratorCV's
    availableImageOperations = {
        BilateralFilter::name,
        GaussianBlur::name,
        Blur::name,
        MedianBlur::name,
        Canny::name,
        ConvertTo::name,
        DeblurFilter::name,
        EqualizeHist::name,
        GammaCorrection::name,
        Laplacian::name,
        MixChannels::name,
        MorphologyEx::name,
        RadialRemap::name,
        Rotation::name,
        Sharpen::name,
        ShiftHue::name
    };
}

Pipeline::~Pipeline()
{
    for (auto operation: imageOperations)
    {
        delete operation;
    }
}

void Pipeline::iterate()
{
    for (auto operation: imageOperations)
        if (operation->isEnabled())
            image = operation->applyOperation(image);
}

void Pipeline::swapImageOperations(int operationIndex0, int operationIndex1)
{
    ImageOperation *operation = imageOperations[operationIndex0];

    std::vector<ImageOperation*>::iterator it = imageOperations.begin();

    imageOperations.erase(it + operationIndex0);
    if (operationIndex0 > operationIndex1) imageOperations.insert(it + operationIndex1, operation);
    else imageOperations.insert(it + operationIndex1 - 1, operation);
}

void Pipeline::removeImageOperation(int operationIndex)
{
    std::vector<ImageOperation*>::iterator it = imageOperations.begin();
    imageOperations.erase(it + operationIndex);
}

void Pipeline::insertImageOperation(int newOperationIndex, int currentOperationIndex)
{
    std::vector<ImageOperation*>::iterator it = imageOperations.begin();

    std::string operationName = availableImageOperations[newOperationIndex];

    if (operationName == BilateralFilter::name)
    {
        imageOperations.insert(it + currentOperationIndex + 1, new BilateralFilter(false, 3, 50, 50));
    }
    else if (operationName == Blur::name)
    {
        imageOperations.insert(it + currentOperationIndex + 1, new Blur(false, 3));
    }
    else if (operationName == Canny::name)
    {
        imageOperations.insert(it + currentOperationIndex + 1, new Canny(false, 100, 300, 3, false));
    }
    else if (operationName == ConvertTo::name)
    {
        imageOperations.insert(it + currentOperationIndex + 1, new ConvertTo(false, 1.0, 0.0));
    }
    else if (operationName == DeblurFilter::name)
    {
        imageOperations.insert(it + currentOperationIndex + 1, new DeblurFilter(false, 1.0, 1000.0));
    }
    else if (operationName == EqualizeHist::name)
    {
        imageOperations.insert(it + currentOperationIndex + 1, new EqualizeHist(false));
    }
    else if (operationName == GammaCorrection::name)
    {
        imageOperations.insert(it + currentOperationIndex + 1, new GammaCorrection(false, 1.0));
    }
    else if (operationName == GaussianBlur::name)
    {
        imageOperations.insert(it + currentOperationIndex + 1, new GaussianBlur(false, 3, 1.0));
    }
    else if (operationName == Laplacian::name)
    {
        imageOperations.insert(it + currentOperationIndex + 1, new Laplacian(false, 3, 1.0, 0.0));
    }
    else if (operationName == MedianBlur::name)
    {
        imageOperations.insert(it + currentOperationIndex + 1, new MedianBlur(false, 3));
    }
    else if (operationName == MixChannels::name)
    {
        imageOperations.insert(it + currentOperationIndex + 1, new MixChannels(false, 0, 1, 2));
    }
    else if (operationName == MorphologyEx::name)
    {
        imageOperations.insert(it + currentOperationIndex + 1, new MorphologyEx(false, 3, 1, cv::MORPH_ERODE, cv::MORPH_RECT));
    }
    else if (operationName == Rotation::name)
    {
        imageOperations.insert(it + currentOperationIndex + 1, new Rotation(false, 0.0, 1.0, cv::INTER_NEAREST));
    }
    else if (operationName == RadialRemap::name)
    {
        imageOperations.insert(it + currentOperationIndex + 1, new RadialRemap(false, 0.0, cv::INTER_NEAREST));
    }
    else if (operationName == Sharpen::name)
    {
        imageOperations.insert(it + currentOperationIndex + 1, new Sharpen(false, 1.0, 5.0, 1.0));
    }
    else if (operationName == ShiftHue::name)
    {
        imageOperations.insert(it + currentOperationIndex + 1, new ShiftHue(false, 0));
    }
}

void Pipeline::loadImageOperation(std::string operationName, bool enabled, std::vector<bool> boolParameters, std::vector<int> intParameters, std::vector<double> doubleParameters, std::vector<int> morphTypeParameters, std::vector<int> morphShapeParameters, std::vector<int> interpolationFlagParameters)
{
    if (operationName == BilateralFilter::name)
    {
        imageOperations.push_back(new BilateralFilter(enabled, intParameters[0], doubleParameters[0], doubleParameters[1]));
    }
    else if (operationName == Blur::name)
    {
        imageOperations.push_back(new Blur(enabled, intParameters[0]));
    }
    else if (operationName == Canny::name)
    {
        imageOperations.push_back(new Canny(enabled, doubleParameters[0], doubleParameters[1], intParameters[0], boolParameters[0]));
    }
    else if (operationName == ConvertTo::name)
    {
        imageOperations.push_back(new ConvertTo(enabled, doubleParameters[0], doubleParameters[1]));
    }
    else if (operationName == DeblurFilter::name)
    {
        imageOperations.push_back(new DeblurFilter(enabled, doubleParameters[0], doubleParameters[1]));
    }
    else if (operationName == EqualizeHist::name)
    {
        imageOperations.push_back(new EqualizeHist(enabled));
    }
    else if (operationName == GammaCorrection::name)
    {
        imageOperations.push_back(new GammaCorrection(enabled, doubleParameters[0]));
    }
    else if (operationName == GaussianBlur::name)
    {
        imageOperations.push_back(new GaussianBlur(enabled, intParameters[0], doubleParameters[0]));
    }
    else if (operationName == Laplacian::name)
    {
        imageOperations.push_back(new Laplacian(enabled, intParameters[0], doubleParameters[0], doubleParameters[1]));
    }
    else if (operationName == MedianBlur::name)
    {
        imageOperations.push_back(new MedianBlur(enabled, intParameters[0]));
    }
    else if (operationName == MixChannels::name)
    {
        imageOperations.push_back(new MixChannels(enabled, intParameters[0], intParameters[1], intParameters[2]));
    }
    else if (operationName == MorphologyEx::name)
    {
        cv::MorphTypes morphType = static_cast<cv::MorphTypes>(morphTypeParameters[0]);
        cv::MorphShapes morphShape = static_cast<cv::MorphShapes>(morphShapeParameters[0]);
        imageOperations.push_back(new MorphologyEx(enabled, intParameters[0], intParameters[1], morphType, morphShape));
    }
    else if (operationName == RadialRemap::name)
    {
        cv::InterpolationFlags flag = static_cast<cv::InterpolationFlags>(interpolationFlagParameters[0]);
        imageOperations.push_back(new RadialRemap(enabled, doubleParameters[0], flag));
    }
    else if (operationName == Rotation::name)
    {
        cv::InterpolationFlags flag = static_cast<cv::InterpolationFlags>(interpolationFlagParameters[0]);
        imageOperations.push_back(new Rotation(enabled, doubleParameters[0], doubleParameters[1], flag));
    }
    else if (operationName == Sharpen::name)
    {
        imageOperations.push_back(new Sharpen(enabled, doubleParameters[0], doubleParameters[1], doubleParameters[2]));
    }
    else if (operationName == ShiftHue::name)
    {
        imageOperations.push_back(new ShiftHue(enabled, intParameters[0]));
    }
}

GeneratorCV::GeneratorCV()
{
    // Duplicate of Pipeline's
    availableImageOperations = {
        BilateralFilter::name,
        GaussianBlur::name,
        Blur::name,
        MedianBlur::name,
        Canny::name,
        ConvertTo::name,
        DeblurFilter::name,
        EqualizeHist::name,
        GammaCorrection::name,
        Laplacian::name,
        MixChannels::name,
        MorphologyEx::name,
        RadialRemap::name,
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
    cv::destroyAllWindows();
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

    outImage = maskedSeed.clone();

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

void GeneratorCV::computeDFT()
{
    cv::Mat outImageGray;
    cv::cvtColor(outImage, outImageGray, cv::COLOR_BGR2GRAY);

    cv::Mat padded;
    int m = cv::getOptimalDFTSize(outImage.rows);
    int n = cv::getOptimalDFTSize(outImage.cols);
    cv::copyMakeBorder(outImageGray, padded, 0, m - outImageGray.rows, 0, n - outImageGray.cols, cv::BORDER_CONSTANT, cv::Scalar::all(0));

    cv::Mat planes[] = {cv::Mat_<float>(padded), cv::Mat::zeros(padded.size(), CV_32F)};
    cv::Mat complexOutImage;
    cv::merge(planes, 2, complexOutImage);

    cv::dft(complexOutImage, complexOutImage);

    cv::split(complexOutImage, planes);
    cv::magnitude(planes[0], planes[1], planes[0]);
    cv::Mat magOutImage = planes[0];

    magOutImage += cv::Scalar::all(1);
    cv::log(magOutImage, magOutImage);

    magOutImage = magOutImage(cv::Rect(0, 0, magOutImage.cols & -2, magOutImage.rows & -2));

    int cx = magOutImage.cols / 2;
    int cy = magOutImage.rows / 2;

    cv::Mat q0(magOutImage, cv::Rect(0, 0, cx, cy));
    cv::Mat q1(magOutImage, cv::Rect(cx, 0, cx, cy));
    cv::Mat q2(magOutImage, cv::Rect(0 ,cy, cx, cy));
    cv::Mat q3(magOutImage, cv::Rect(cx, cy, cx, cy));

    cv::Mat tmp;

    q0.copyTo(tmp);
    q3.copyTo(q0);
    tmp.copyTo(q3);

    q1.copyTo(tmp);
    q2.copyTo(q1);
    tmp.copyTo(q2);

    cv::normalize(magOutImage, magOutImage, 0, 1, cv::NORM_MINMAX);

    cv::imshow("DFT Spectrum magnitude", magOutImage);
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

void GeneratorCV::loadPipeline(double blendFactor)
{
    pipelines.push_back(new Pipeline(outImage));
    pipelines.back()->blendFactor = blendFactor;
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
