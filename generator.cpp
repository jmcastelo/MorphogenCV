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
        BlendPreviousImages::name,
        BilateralFilter::name,
        GaussianBlur::name,
        Blur::name,
        MedianBlur::name,
        Canny::name,
        ColorQuantization::name,
        ConvertTo::name,
        DeblurFilter::name,
        EqualizeHist::name,
        Filter2D::name,
        GammaCorrection::name,
        InvertColors::name,
        Laplacian::name,
        MixBGRChannels::name,
        MorphologyEx::name,
        Pixelate::name,
        RadialRemap::name,
        Rotation::name,
        Saturate::name,
        Sharpen::name,
        ShiftHue::name,
        SwapChannels::name
    };
}

Pipeline::~Pipeline()
{
    for (auto operation: imageOperations)
        delete operation;
}

void Pipeline::iterate()
{
    for (auto operation: imageOperations)
        if (operation->isEnabled())
            operation->applyOperation(image);
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
    if (!imageOperations.empty())
    {
        std::vector<ImageOperation*>::iterator it = imageOperations.begin();
        imageOperations.erase(it + operationIndex);
    }
}

void Pipeline::insertImageOperation(int newOperationIndex, int currentOperationIndex)
{
    std::vector<ImageOperation*>::iterator it = imageOperations.begin();

    std::string operationName = availableImageOperations[newOperationIndex];

    if (operationName == BilateralFilter::name)
    {
        imageOperations.insert(it + currentOperationIndex + 1, new BilateralFilter(false, 3, 50, 50));
    }
    else if (operationName == BlendPreviousImages::name)
    {
        imageOperations.insert(it + currentOperationIndex + 1, new BlendPreviousImages(false, 0, 0.0));
    }
    else if (operationName == Blur::name)
    {
        imageOperations.insert(it + currentOperationIndex + 1, new Blur(false, 3));
    }
    else if (operationName == Canny::name)
    {
        imageOperations.insert(it + currentOperationIndex + 1, new Canny(false, 100, 300, 3, false));
    }
    else if (operationName == ColorQuantization::name)
    {
        imageOperations.insert(it + currentOperationIndex + 1, new ColorQuantization(false, 255, 179, 255, 255, 0));
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
    else if (operationName == Filter2D::name)
    {
        std::vector<float> kernel = {0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0};
        imageOperations.insert(it + currentOperationIndex + 1, new Filter2D(false, kernel));
    }
    else if (operationName == GammaCorrection::name)
    {
        imageOperations.insert(it + currentOperationIndex + 1, new GammaCorrection(false, 1.0));
    }
    else if (operationName == GaussianBlur::name)
    {
        imageOperations.insert(it + currentOperationIndex + 1, new GaussianBlur(false, 3, 1.0));
    }
    else if (operationName == InvertColors::name)
    {
        imageOperations.insert(it + currentOperationIndex + 1, new InvertColors(false));
    }
    else if (operationName == Laplacian::name)
    {
        imageOperations.insert(it + currentOperationIndex + 1, new Laplacian(false, 3));
    }
    else if (operationName == MedianBlur::name)
    {
        imageOperations.insert(it + currentOperationIndex + 1, new MedianBlur(false, 3));
    }
    else if (operationName == SwapChannels::name)
    {
        imageOperations.insert(it + currentOperationIndex + 1, new SwapChannels(false, 0, 1, 2));
    }
    else if (operationName == MixBGRChannels::name)
    {
        std::vector<float> kernel = {1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0};
        imageOperations.insert(it + currentOperationIndex + 1, new MixBGRChannels(false, kernel));
    }
    else if (operationName == MorphologyEx::name)
    {
        imageOperations.insert(it + currentOperationIndex + 1, new MorphologyEx(false, 3, 1, cv::MORPH_ERODE, cv::MORPH_RECT));
    }
    else if (operationName == Pixelate::name)
    {
        imageOperations.insert(it + currentOperationIndex + 1, new Pixelate(false, 1));
    }
    else if (operationName == RadialRemap::name)
    {
        imageOperations.insert(it + currentOperationIndex + 1, new RadialRemap(false, 0.0, 0, cv::INTER_NEAREST));
    }
    else if (operationName == Rotation::name)
    {
        imageOperations.insert(it + currentOperationIndex + 1, new Rotation(false, 0.0, 1.0, cv::INTER_NEAREST));
    }
    else if (operationName == Saturate::name)
    {
        imageOperations.insert(it + currentOperationIndex + 1, new Saturate(false, 1.0, 0.0));
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

void Pipeline::loadImageOperation(
        std::string operationName,
        bool enabled,
        std::vector<bool> boolParameters,
        std::vector<int> intParameters,
        std::vector<double> doubleParameters,
        std::vector<int> morphTypeParameters,
        std::vector<int> morphShapeParameters,
        std::vector<int> interpolationFlagParameters,
        std::vector<float> kernelElements)
{
    if (operationName == BilateralFilter::name)
    {
        imageOperations.push_back(new BilateralFilter(enabled, intParameters[0], doubleParameters[0], doubleParameters[1]));
    }
    else if (operationName == BlendPreviousImages::name)
    {
        imageOperations.push_back(new BlendPreviousImages(enabled, intParameters[0], doubleParameters[0]));
    }
    else if (operationName == Blur::name)
    {
        imageOperations.push_back(new Blur(enabled, intParameters[0]));
    }
    else if (operationName == Canny::name)
    {
        imageOperations.push_back(new Canny(enabled, doubleParameters[0], doubleParameters[1], intParameters[0], boolParameters[0]));
    }
    else if (operationName == ColorQuantization::name)
    {
        imageOperations.push_back(new ColorQuantization(enabled, intParameters[0], intParameters[1], intParameters[2], intParameters[3], intParameters[4]));
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
    else if (operationName == Filter2D::name)
    {
        imageOperations.push_back(new Filter2D(enabled, kernelElements));
    }
    else if (operationName == GammaCorrection::name)
    {
        imageOperations.push_back(new GammaCorrection(enabled, doubleParameters[0]));
    }
    else if (operationName == GaussianBlur::name)
    {
        imageOperations.push_back(new GaussianBlur(enabled, intParameters[0], doubleParameters[0]));
    }
    else if (operationName == InvertColors::name)
    {
        imageOperations.push_back(new InvertColors(enabled));
    }
    else if (operationName == Laplacian::name)
    {
        imageOperations.push_back(new Laplacian(enabled, intParameters[0]));
    }
    else if (operationName == MedianBlur::name)
    {
        imageOperations.push_back(new MedianBlur(enabled, intParameters[0]));
    }
    else if (operationName == SwapChannels::name)
    {
        imageOperations.push_back(new SwapChannels(enabled, intParameters[0], intParameters[1], intParameters[2]));
    }
    else if (operationName == MixBGRChannels::name)
    {
        imageOperations.push_back(new MixBGRChannels(enabled, kernelElements));
    }
    else if (operationName == MorphologyEx::name)
    {
        cv::MorphTypes morphType = static_cast<cv::MorphTypes>(morphTypeParameters[0]);
        cv::MorphShapes morphShape = static_cast<cv::MorphShapes>(morphShapeParameters[0]);
        imageOperations.push_back(new MorphologyEx(enabled, intParameters[0], intParameters[1], morphType, morphShape));
    }
    else if (operationName == Pixelate::name)
    {
        imageOperations.push_back(new Pixelate(enabled, intParameters[0]));
    }
    else if (operationName == RadialRemap::name)
    {
        cv::InterpolationFlags flag = static_cast<cv::InterpolationFlags>(interpolationFlagParameters[0]);
        imageOperations.push_back(new RadialRemap(enabled, doubleParameters[0], intParameters[0], flag));
    }
    else if (operationName == Rotation::name)
    {
        cv::InterpolationFlags flag = static_cast<cv::InterpolationFlags>(interpolationFlagParameters[0]);
        imageOperations.push_back(new Rotation(enabled, doubleParameters[0], doubleParameters[1], flag));
    }
    else if (operationName == Saturate::name)
    {
        imageOperations.push_back(new Saturate(enabled, doubleParameters[0], doubleParameters[1]));
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
        BlendPreviousImages::name,
        BilateralFilter::name,
        GaussianBlur::name,
        Blur::name,
        MedianBlur::name,
        Canny::name,
        ColorQuantization::name,
        ConvertTo::name,
        DeblurFilter::name,
        EqualizeHist::name,
        Filter2D::name,
        GammaCorrection::name,
        InvertColors::name,
        Laplacian::name,
        MixBGRChannels::name,
        MorphologyEx::name,
        Pixelate::name,
        RadialRemap::name,
        Rotation::name,
        Saturate::name,
        Sharpen::name,
        ShiftHue::name,
        SwapChannels::name
    };

    imageSize = 700;

    colorScaleFactor = 1.0 / (imageSize * imageSize * 255);

    histogramSize = 256;

    iteration = 0;

    selectingPixel = false;

    drawingPointer = false;
    pointerCanvasDrawn = false;
    persistentDrawing = false;
    pointerRadius = 5;
    pointerThickness = -1;
    pointerColor = cv::Scalar(255, 255, 255);

    setPlainColor(0, 0, 0);

    framesPerSecond = 30;

    cv::namedWindow("Frame");
    cv::setMouseCallback("Frame", onMouse, this);

    circularMask = false;

    setMask();
    computeHistogramMax();
    drawRandomSeed(false);
    clearPointerCanvas();

    outputPipeline = new Pipeline(cv::Mat::zeros(imageSize, imageSize, CV_8UC3));

    selectedPixel = cv::Point(imageSize / 2, imageSize / 2);
}

GeneratorCV::~GeneratorCV()
{
    for (auto pipeline: pipelines)
        delete pipeline;

    pipelines.clear();

    delete outputPipeline;
}

void GeneratorCV::destroyAllWindows()
{
    closeVideoWriter();
    cv::destroyAllWindows();
}

void GeneratorCV::setMask()
{
    mask = cv::Mat::zeros(imageSize, imageSize, CV_8U);

    if (circularMask)
    {
        cv::Point center = cv::Point(imageSize / 2, imageSize / 2);
        cv::circle(mask, center, imageSize / 2 - 2, cv::Scalar(255, 255, 255), -1, cv::FILLED);
    }
    else
        cv::rectangle(mask, cv::Point(0, 0), cv::Point(imageSize, imageSize), cv::Scalar(255, 255, 255), -1);
}

void GeneratorCV::toggleMask(bool apply)
{
    circularMask = apply;
    setMask();
    computeHistogramMax();
}

void GeneratorCV::computeHistogramMax()
{
    cv::Mat full = cv::Mat::ones(imageSize, imageSize, CV_8U);
    cv::Mat masked;
    full.copyTo(masked, mask);
    int pixelCount = cv::countNonZero(masked);
    histogramMax = pixelCount * 5.0 / 100.0;
}

void GeneratorCV::drawRandomSeed(bool grayscale)
{
    cv::Mat randomSeedImage = cv::Mat(imageSize, imageSize, CV_8UC3);
    cv::randu(randomSeedImage, cv::Scalar::all(0), cv::Scalar::all(255));

    if (grayscale)
    {
        cv::cvtColor(randomSeedImage, randomSeedImage, cv::COLOR_BGR2GRAY);
        cv::cvtColor(randomSeedImage, randomSeedImage, cv::COLOR_GRAY2BGR);
    }

    cv::Mat maskedSeed = cv::Mat::zeros(imageSize, imageSize, CV_8UC3);
    randomSeedImage.copyTo(maskedSeed, mask);

    for (auto pipeline: pipelines)
        pipeline->image = maskedSeed.clone();

    outputImage = maskedSeed.clone();

    cv::imshow("Frame", maskedSeed);
}

void GeneratorCV::loadSeedImage(std::string filename)
{
    seedImage = cv::imread(filename);

    if (seedImage.size() != cv::Size(imageSize, imageSize))
        cv::resize(seedImage, seedImage, cv::Size(imageSize, imageSize));

    cv::Mat maskedSeed = cv::Mat::zeros(imageSize, imageSize, CV_8UC3);
    seedImage.copyTo(maskedSeed, mask);
    seedImage = maskedSeed.clone();
}

void GeneratorCV::drawSeedImage()
{
    if (!seedImage.empty())
    {
        for (auto pipeline: pipelines)
            pipeline->image = seedImage.clone();

        outputImage = seedImage.clone();

        cv::imshow("Frame", seedImage);
    }
}

void GeneratorCV::setPlainColor(int blue, int green, int red)
{
    plainColor = cv::Scalar(blue, green, red);
}

void GeneratorCV::drawPlainColorSeed()
{
    cv::Mat plainColorSeed(imageSize, imageSize, CV_8UC3, plainColor);

    cv::Mat maskedSeed = cv::Mat::zeros(imageSize, imageSize, CV_8UC3);
    plainColorSeed.copyTo(maskedSeed, mask);

    for (auto pipeline: pipelines)
        pipeline->image = maskedSeed.clone();

    outputImage = maskedSeed.clone();

    cv::imshow("Frame", maskedSeed);
}

void GeneratorCV::blendImages()
{
    cv::Mat blendImage = cv::Mat::zeros(imageSize, imageSize, CV_8UC3);

    for (auto pipeline: pipelines)
        cv::addWeighted(blendImage, 1.0, pipeline->image, pipeline->blendFactor, 0.0, blendImage);

    outputPipeline->image = blendImage.clone();
}

void GeneratorCV::iterate()
{
    for (auto &pipeline: pipelines)
        pipeline->iterate();

    if (!pipelines.empty())
        blendImages();
    else
        outputPipeline->image = outputImage.clone();

    outputPipeline->iterate();

    outputImage = cv::Mat::zeros(imageSize, imageSize, CV_8UC3);

    outputPipeline->image.copyTo(outputImage, mask);

    if (pointerCanvasDrawn)
        drawPointerCanvas();

    for (auto &pipeline: pipelines)
        pipeline->image = outputImage.clone();

    iteration++;
}

void GeneratorCV::computeBGRSum()
{
    bgrSum = cv::sum(outputImage);
}

void GeneratorCV::computeBGRPixel()
{
    bgrPixel = outputImage.at<cv::Vec3b>(selectedPixel);
}

void GeneratorCV::computeHistogram()
{
    float range[] = {0, 256};
    const float *histogramRange[] = {range};

    cv::Mat bgrChannels[3];
    cv::split(outputImage, bgrChannels);

    cv::calcHist(&bgrChannels[0], 1, 0, mask, blueHistogram, 1, &histogramSize, histogramRange, true, false);
    cv::calcHist(&bgrChannels[1], 1, 0, mask, greenHistogram, 1, &histogramSize, histogramRange, true, false);
    cv::calcHist(&bgrChannels[2], 1, 0, mask, redHistogram, 1, &histogramSize, histogramRange, true, false);
}

void GeneratorCV::computeDFT()
{
    cv::Mat outputImageGray;
    cv::cvtColor(outputImage, outputImageGray, cv::COLOR_BGR2GRAY);

    cv::Mat padded;
    int m = cv::getOptimalDFTSize(outputImage.rows);
    int n = cv::getOptimalDFTSize(outputImage.cols);
    cv::copyMakeBorder(outputImageGray, padded, 0, m - outputImageGray.rows, 0, n - outputImageGray.cols, cv::BORDER_CONSTANT, cv::Scalar::all(0));

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

    cv::imshow("DFT Spectrum Magnitude", magOutImage);
}

void GeneratorCV::showPixelSelectionCursor()
{
    cv::Mat layer = cv::Mat::zeros(imageSize, imageSize, CV_8UC3);
    cv::line(layer, cv::Point(0, selectedPixel.y), cv::Point(imageSize, selectedPixel.y), cv::Scalar(255, 255, 255));
    cv::line(layer, cv::Point(selectedPixel.x, 0), cv::Point(selectedPixel.x, imageSize), cv::Scalar(255, 255, 255));
    cv::addWeighted(outputImage, 0.5, layer, 0.5, 0.0, layer);
    cv::imshow("Frame", layer);
}

void GeneratorCV::showImage()
{
    cv::imshow("Frame", outputImage);
}

void GeneratorCV::openVideoWriter(std::string name)
{
    videoWriter.open(name, cv::VideoWriter::fourcc('P', 'I', 'M', '1'), framesPerSecond, cv::Size(imageSize, imageSize), true);
    frameCount = 0;
}

void GeneratorCV::writeVideoFrame()
{
    if (videoWriter.isOpened())
    {
        videoWriter.write(outputImage);
        frameCount++;
    }
}

void GeneratorCV::closeVideoWriter()
{
    if (videoWriter.isOpened())
        videoWriter.release();
}

void GeneratorCV::onMouse(int event, int x, int y, int flags, void *userdata)
{
    GeneratorCV *mw = reinterpret_cast<GeneratorCV*>(userdata);
    mw->processMouse(event, x, y, flags);
}

void GeneratorCV::processMouse(int event, int x, int y, int flags)
{
    if (event == cv::EVENT_LBUTTONDOWN || (event == cv::EVENT_MOUSEMOVE && flags == cv::EVENT_FLAG_LBUTTON))
    {
        if (selectingPixel)
            selectedPixel = cv::Point(x, y);

        if (drawingPointer && !selectingPixel)
            drawPointer(x, y);
    }
    else if (event == cv::EVENT_LBUTTONUP && drawingPointer && !selectingPixel && !persistentDrawing)
    {
        clearPointerCanvas();
    }
}

void GeneratorCV::drawPointer(int x, int y)
{
    if (!persistentDrawing)
        pointerCanvas = cv::Mat::zeros(imageSize, imageSize, CV_8UC3);

    cv::circle(pointerCanvas, cv::Point(x, y), pointerRadius, pointerColor, pointerThickness, cv::FILLED);

    cv::Mat dst = cv::Mat::zeros(imageSize, imageSize, CV_8UC3);
    pointerCanvas.copyTo(dst, mask);
    pointerCanvas = dst.clone();

    pointerCanvasDrawn = true;

    drawPointerCanvas();
    cv::imshow("Frame", outputImage);
}

void GeneratorCV::drawCenteredPointer()
{
    drawPointer(imageSize / 2, imageSize / 2);
}

void GeneratorCV::drawPointerCanvas()
{
    cv::Mat pointerCanvasGray;
    cv::cvtColor(pointerCanvas, pointerCanvasGray, cv::COLOR_BGR2GRAY);
    cv::Mat drawingMask;
    cv::threshold(pointerCanvasGray, drawingMask, 1, 255, cv::THRESH_BINARY);
    cv::Mat maskInverse;
    cv::bitwise_not(drawingMask, maskInverse);

    cv::Mat outputImageMasked;
    cv::bitwise_and(outputImage, outputImage, outputImageMasked, maskInverse);
    cv::Mat pointerCanvasMasked;
    cv::bitwise_and(pointerCanvas, pointerCanvas, pointerCanvasMasked, drawingMask);

    cv::add(outputImageMasked, pointerCanvasMasked, outputImage);
}

void GeneratorCV::clearPointerCanvas()
{
    pointerCanvas = cv::Mat::zeros(imageSize, imageSize, CV_8UC3);

    pointerCanvasDrawn = false;
}

void GeneratorCV::setImageSize(int size)
{
    imageSize = size;

    colorScaleFactor = 1.0 / (imageSize * imageSize * 255);

    for (auto &pipeline: pipelines)
        cv::resize(pipeline->image, pipeline->image, cv::Size(imageSize, imageSize));

    cv::resize(outputImage, outputImage, cv::Size(imageSize, imageSize));

    if (!seedImage.empty())
        cv::resize(seedImage, seedImage, cv::Size(imageSize, imageSize));

    cv::resize(pointerCanvas, pointerCanvas, cv::Size(imageSize, imageSize));

    setMask();
    computeHistogramMax();

    cv::imshow("Frame", outputImage);

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
    for (it = outputImage.begin<cv::Vec3b>(), end = outputImage.end<cv::Vec3b>(); it != end; ++it)
        components.push_back((*it)[colorIndex]);

    return components;
}

void GeneratorCV::swapImageOperations(int pipelineIndex, int operationIndex0, int operationIndex1)
{
    if (pipelineIndex >= 0)
        pipelines[pipelineIndex]->swapImageOperations(operationIndex0, operationIndex1);
    else
        outputPipeline->swapImageOperations(operationIndex0, operationIndex1);
}

void GeneratorCV::removeImageOperation(int pipelineIndex, int operationIndex)
{
    if (pipelineIndex >= 0)
        pipelines[pipelineIndex]->removeImageOperation(operationIndex);
    else
        outputPipeline->removeImageOperation(operationIndex);
}

void GeneratorCV::insertImageOperation(int pipelineIndex, int newOperationIndex, int currentOperationIndex)
{
    if (pipelineIndex >= 0)
        pipelines[pipelineIndex]->insertImageOperation(newOperationIndex, currentOperationIndex);
    else
        outputPipeline->insertImageOperation(newOperationIndex, currentOperationIndex);
}

void GeneratorCV::removePipeline(int pipelineIndex)
{
    if (pipelineIndex >= 0 && !pipelines.empty())
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
    pipelines.push_back(new Pipeline(outputImage));
    if (pipelines.size() == 1)
        pipelines.back()->blendFactor = 1.0;
    else
        pipelines.back()->blendFactor = 0.0;
}

void GeneratorCV::loadPipeline(double blendFactor)
{
    cv::Mat mat;
    outputImage.copyTo(mat, mask);
    pipelines.push_back(new Pipeline(mat));
    pipelines.back()->blendFactor = blendFactor;
}

void GeneratorCV::setPipelineBlendFactor(int pipelineIndex, double factor)
{
    // Keep within range

    if (factor < 0.0)
        factor = 0.0;
    if (factor > 1.0)
        factor = 1.0;

    if (pipelines.size() == 1)
        factor = 1.0;

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
