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

GeneratorCV::GeneratorCV()
{
    availableImageOperations = {
        Canny::name,
        ConvertTo::name,
        GaussianBlur::name,
        Laplacian::name,
        MixChannels::name,
        MorphologyEx::name,
        Rotation::name,
        Sharpen::name
    };

    cv::Mat img;
    images = {img, img};

    std::vector<ImageOperation*> ops;
    imageOperations = {ops, ops};

    imageSize = 700;
    colorScaleFactor = 1.0 / (imageSize * imageSize * 255);
    iteration = 0;

    selectingPixel = false;

    blendFactor = 0.5;

    setMask();
    initSystem(true);

    cv::namedWindow("image", cv::WINDOW_AUTOSIZE);
    cv::setMouseCallback("image", onMouse, this);
    selectedPixel = cv::Point(imageSize / 2, imageSize / 2);
}

GeneratorCV::~GeneratorCV()
{
    cv::destroyWindow("image");
}

void GeneratorCV::setMask()
{
    mask = cv::Mat::zeros(imageSize, imageSize, CV_8U);
    cv::Point center = cv::Point(imageSize / 2, imageSize / 2);
    cv::circle(mask, center, imageSize / 2, cv::Scalar(255, 255, 255), -1);
}

void GeneratorCV::initSystem(bool grayscale)
{
    cv::Mat seedImage = cv::Mat(imageSize, imageSize, CV_8UC3);
    cv::randu(seedImage, cv::Scalar::all(0), cv::Scalar::all(255));

    if (grayscale)
    {
        cv::cvtColor(seedImage, seedImage, cv::COLOR_BGR2GRAY);
        cv::cvtColor(seedImage, seedImage, cv::COLOR_GRAY2BGR);
    }

    for (size_t i = 0; i < images.size(); i++)
    {
        seedImage.copyTo(images[i], mask);
    }

    cv::imshow("image", images[0]);
}

void GeneratorCV::applyImageOperations()
{
    for (size_t i = 0; i < images.size(); i++)
    {
        cv::Mat src = images[i];

        for (auto operation: imageOperations[i])
        {
            cv::Mat dst = operation->applyOperation(src);
            dst.copyTo(src, mask);
        }

        images[i] = src.clone();
    }
}

void GeneratorCV::iterate()
{
    applyImageOperations();

    cv::addWeighted(images[0], blendFactor, images[1], 1.0 - blendFactor, 0.0, blendedImage);

    for (size_t i = 0; i < images.size(); i++)
    {
        blendedImage.copyTo(images[i], mask);
    }

    iteration++;
}

void GeneratorCV::computeBGRSum()
{
    bgrSum = cv::sum(blendedImage);
}

void GeneratorCV::computeBGRPixel()
{
    bgrPixel = blendedImage.at<cv::Vec3b>(selectedPixel);
}

void GeneratorCV::showPixelSelectionCursor()
{
    cv::Mat layer = cv::Mat::zeros(imageSize, imageSize, CV_8UC3);
    cv::line(layer, cv::Point(0, selectedPixel.y), cv::Point(imageSize, selectedPixel.y), cv::Scalar(255, 255, 255));
    cv::line(layer, cv::Point(selectedPixel.x, 0), cv::Point(selectedPixel.x, imageSize), cv::Scalar(255, 255, 255));
    cv::addWeighted(blendedImage, 0.5, layer, 0.5, 0.0, layer);
    cv::imshow("image", layer);
}

void GeneratorCV::showImage()
{
    cv::imshow("image", blendedImage);
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
    cv::imwrite(filename, images[0], params);
}

int GeneratorCV::getIterationNumber()
{
    return iteration;
}

void GeneratorCV::setImageSize(int size)
{
    imageSize = size;

    colorScaleFactor = 1.0 / (imageSize * imageSize * 255);

    for (auto &img: images)
    {
        cv::Mat dst;
        cv::resize(img, dst, cv::Size(imageSize, imageSize));
        img = dst.clone();
    }

    setMask();

    cv::imshow("image", images[0]);

    selectedPixel = cv::Point(imageSize / 2, imageSize / 2);
}

void GeneratorCV::swapImageOperations(int imageIndex, int operationIndex0, int operationIndex1)
{
    ImageOperation *operation = imageOperations[imageIndex][operationIndex0];
    std::vector<ImageOperation*>::iterator it = imageOperations[imageIndex].begin();
    imageOperations[imageIndex].erase(it + operationIndex0);
    imageOperations[imageIndex].insert(it + operationIndex1, operation);
}

void GeneratorCV::removeImageOperation(int imageIndex, int operationIndex)
{
    std::vector<ImageOperation*>::iterator it = imageOperations[imageIndex].begin();
    imageOperations[imageIndex].erase(it + operationIndex);
}

void GeneratorCV::insertImageOperation(int imageIndex, int newOperationIndex, int currentOperationIndex)
{
    std::vector<ImageOperation*>::iterator it = imageOperations[imageIndex].begin();

    if (newOperationIndex == 0)
    {
        imageOperations[imageIndex].insert(it + currentOperationIndex + 1, new Canny(100, 300, 3, false));
    }
    else if (newOperationIndex == 1)
    {
        imageOperations[imageIndex].insert(it + currentOperationIndex + 1, new ConvertTo(1.0, 0.0));
    }
    else if (newOperationIndex == 2)
    {
        imageOperations[imageIndex].insert(it + currentOperationIndex + 1, new GaussianBlur(3, 0.0, 0.0));
    }
    else if (newOperationIndex == 3)
    {
        imageOperations[imageIndex].insert(it + currentOperationIndex + 1, new Laplacian(3, 1.0, 0.0));
    }
    else if (newOperationIndex == 4)
    {
        imageOperations[imageIndex].insert(it + currentOperationIndex + 1, new MixChannels(0, 1, 2));
    }
    else if (newOperationIndex == 5)
    {
        imageOperations[imageIndex].insert(it + currentOperationIndex + 1, new MorphologyEx(3, 1, cv::MORPH_ERODE, cv::MORPH_RECT));
    }
    else if (newOperationIndex == 6)
    {
        imageOperations[imageIndex].insert(it + currentOperationIndex + 1, new Rotation(0.0, 1.0, cv::INTER_NEAREST));
    }
    else if (newOperationIndex == 7)
    {
        imageOperations[imageIndex].insert(it + currentOperationIndex + 1, new Sharpen(1.0, 5.0, 1.0));
    }
}
