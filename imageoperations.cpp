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

ImageOperation::~ImageOperation(){}

// Canny

QString Canny::name = "Canny";

Canny::Canny(double th1, double th2, int s, bool g): threshold1(th1), threshold2(th2), apertureSize(s), L2gradient(g)
{
    QLabel *threshold1Label = new QLabel("Threshold 1");
    QLabel *threshold2Label = new QLabel("Threshold 2");
    QLabel *apertureSizeLabel = new QLabel("Aperture size");

    threshold1LineEdit = new CustomLineEdit();
    threshold2LineEdit = new CustomLineEdit();
    apertureSizeLineEdit = new CustomLineEdit();

    QDoubleValidator *threshold1Validator = new QDoubleValidator(0.0, 300.0, 10, threshold1LineEdit);
    threshold1Validator->setLocale(QLocale::English);
    threshold1LineEdit->setValidator(threshold1Validator);
    threshold1LineEdit->setText(QString::number(threshold1));

    QDoubleValidator *threshold2Validator = new QDoubleValidator(0.0, 300.0, 10, threshold2LineEdit);
    threshold2Validator->setLocale(QLocale::English);
    threshold2LineEdit->setValidator(threshold2Validator);
    threshold2LineEdit->setText(QString::number(threshold2));

    QIntValidator *apertureSizeValidator = new QIntValidator(3, 7, apertureSizeLineEdit);
    apertureSizeLineEdit->setValidator(apertureSizeValidator);
    apertureSizeLineEdit->setText(QString::number(apertureSize));

    L2gradientCheckBox = new QCheckBox("L2 gradient");
    L2gradientCheckBox->setChecked(false);

    QVBoxLayout *vBoxLayout = new QVBoxLayout;
    vBoxLayout->addWidget(threshold1Label);
    vBoxLayout->addWidget(threshold1LineEdit);
    vBoxLayout->addWidget(threshold2Label);
    vBoxLayout->addWidget(threshold2LineEdit);
    vBoxLayout->addWidget(apertureSizeLabel);
    vBoxLayout->addWidget(apertureSizeLineEdit);
    vBoxLayout->addWidget(L2gradientCheckBox);

    mainWidget = new QWidget(this);
    mainWidget->setLayout(vBoxLayout);

    connect(apertureSizeLineEdit, &CustomLineEdit::returnPressed, [=](){
        int size = apertureSizeLineEdit->text().toInt();
        if (size > 0 && size % 2 == 0) size--;
        apertureSize = size;
        apertureSizeLineEdit->setText(QString::number(size));
    });
    connect(threshold1LineEdit, &CustomLineEdit::returnPressed, [=](){ threshold1 = threshold1LineEdit->text().toDouble(); });
    connect(threshold2LineEdit, &CustomLineEdit::returnPressed, [=](){ threshold2 = threshold2LineEdit->text().toDouble(); });
    connect(apertureSizeLineEdit, &CustomLineEdit::focusOut, [=](){ apertureSizeLineEdit->setText(QString::number(apertureSize)); });
    connect(threshold1LineEdit, &CustomLineEdit::focusOut, [=](){ threshold1LineEdit->setText(QString::number(threshold1)); });
    connect(threshold2LineEdit, &CustomLineEdit::focusOut, [=](){ threshold2LineEdit->setText(QString::number(threshold2)); });
    connect(L2gradientCheckBox, &QCheckBox::stateChanged, [=](int state){ L2gradient = (state == Qt::Checked); });
}

Canny::~Canny()
{
    threshold1LineEdit->disconnect();
    threshold2LineEdit->disconnect();
    apertureSizeLineEdit->disconnect();
    L2gradientCheckBox->disconnect();

    QLayoutItem *child;
    while ((child = mainWidget->layout()->takeAt(0)) != 0)
    {
        delete child;
    }

    delete mainWidget->layout();
    delete mainWidget;
}

QWidget* Canny::getParametersWidget()
{
    return mainWidget;
}

cv::Mat Canny::applyOperation(cv::Mat src)
{
    cv::Mat dst;
    cv::Canny(src, dst, threshold1, threshold2, apertureSize, L2gradient);
    return dst;
}

// Convert to

QString ConvertTo::name = "Convert to";

ConvertTo::ConvertTo(double a, double b): alpha(a), beta(b)
{
    QLabel *alphaLabel = new QLabel("Alpha");
    QLabel *betaLabel = new QLabel("Beta");

    alphaLineEdit = new CustomLineEdit();
    betaLineEdit = new CustomLineEdit();

    QDoubleValidator *alphaValidator = new QDoubleValidator(-100.0, 100.0, 10, alphaLineEdit);
    alphaValidator->setLocale(QLocale::English);
    alphaLineEdit->setValidator(alphaValidator);
    alphaLineEdit->setText(QString::number(alpha));

    QDoubleValidator *betaValidator = new QDoubleValidator(-100.0, 100.0, 10, betaLineEdit);
    betaValidator->setLocale(QLocale::English);
    betaLineEdit->setValidator(betaValidator);
    betaLineEdit->setText(QString::number(beta));

    QVBoxLayout *vBoxLayout = new QVBoxLayout;
    vBoxLayout->addWidget(alphaLabel);
    vBoxLayout->addWidget(alphaLineEdit);
    vBoxLayout->addWidget(betaLabel);
    vBoxLayout->addWidget(betaLineEdit);

    mainWidget = new QWidget(this);
    mainWidget->setLayout(vBoxLayout);

    connect(alphaLineEdit, &CustomLineEdit::returnPressed, [=](){ alpha = alphaLineEdit->text().toDouble(); });
    connect(betaLineEdit, &CustomLineEdit::returnPressed, [=](){ beta = betaLineEdit->text().toDouble(); });
    connect(alphaLineEdit, &CustomLineEdit::focusOut, [=](){ alphaLineEdit->setText(QString::number(alpha)); });
    connect(betaLineEdit, &CustomLineEdit::focusOut, [=](){ betaLineEdit->setText(QString::number(beta)); });
}

ConvertTo::~ConvertTo()
{
    alphaLineEdit->disconnect();
    betaLineEdit->disconnect();

    QLayoutItem *child;
    while ((child = mainWidget->layout()->takeAt(0)) != 0)
    {
        delete child;
    }

    delete mainWidget->layout();
    delete mainWidget;
}

QWidget* ConvertTo::getParametersWidget()
{
    return mainWidget;
}

cv::Mat ConvertTo::applyOperation(cv::Mat src)
{
    cv::Mat dst;
    src.convertTo(dst, -1, alpha, beta);
    return dst;
}

// Dilate

QString Dilate::name = "Dilate";

Dilate::Dilate(int k): ksize(k)
{
    QLabel *ksizeLabel = new QLabel("Kernel size");

    ksizeLineEdit = new CustomLineEdit();

    QIntValidator *ksizeValidator = new QIntValidator(1, 25, ksizeLineEdit);
    ksizeLineEdit->setValidator(ksizeValidator);
    ksizeLineEdit->setText(QString::number(ksize));

    QVBoxLayout *vBoxLayout = new QVBoxLayout;
    vBoxLayout->addWidget(ksizeLabel);
    vBoxLayout->addWidget(ksizeLineEdit);

    mainWidget = new QWidget(this);
    mainWidget->setLayout(vBoxLayout);

    connect(ksizeLineEdit, &CustomLineEdit::returnPressed, [=](){
        int size = ksizeLineEdit->text().toInt();
        if (size > 0 && size % 2 == 0) size--;
        ksize = size;
        ksizeLineEdit->setText(QString::number(size));
    });
    connect(ksizeLineEdit, &CustomLineEdit::focusOut, [=](){ ksizeLineEdit->setText(QString::number(ksize)); });
}

Dilate::~Dilate()
{
    ksizeLineEdit->disconnect();

    QLayoutItem *child;
    while ((child = mainWidget->layout()->takeAt(0)) != 0)
    {
        delete child;
    }

    delete mainWidget->layout();
    delete mainWidget;
}

QWidget* Dilate::getParametersWidget()
{
    return mainWidget;
}

cv::Mat Dilate::applyOperation(cv::Mat src)
{
    cv::Mat element = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(ksize, ksize));
    cv::Mat dst;
    cv::dilate(src, dst, element);
    return dst;
}

// Erode

QString Erode::name = "Erode";

Erode::Erode(int k): ksize(k)
{
    QLabel *ksizeLabel = new QLabel("Kernel size");

    ksizeLineEdit = new CustomLineEdit();

    QIntValidator *ksizeValidator = new QIntValidator(1, 25, ksizeLineEdit);
    ksizeLineEdit->setValidator(ksizeValidator);
    ksizeLineEdit->setText(QString::number(ksize));

    QVBoxLayout *vBoxLayout = new QVBoxLayout;
    vBoxLayout->addWidget(ksizeLabel);
    vBoxLayout->addWidget(ksizeLineEdit);

    mainWidget = new QWidget(this);
    mainWidget->setLayout(vBoxLayout);

    connect(ksizeLineEdit, &CustomLineEdit::returnPressed, [=](){
        int size = ksizeLineEdit->text().toInt();
        if (size > 0 && size % 2 == 0) size--;
        ksize = size;
        ksizeLineEdit->setText(QString::number(size));
    });
    connect(ksizeLineEdit, &CustomLineEdit::focusOut, [=](){ ksizeLineEdit->setText(QString::number(ksize)); });
}

Erode::~Erode()
{
    ksizeLineEdit->disconnect();

    QLayoutItem *child;
    while ((child = mainWidget->layout()->takeAt(0)) != 0)
    {
        delete child;
    }

    delete mainWidget->layout();
    delete mainWidget;
}

QWidget* Erode::getParametersWidget()
{
    return mainWidget;
}

cv::Mat Erode::applyOperation(cv::Mat src)
{
    cv::Mat element = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(ksize, ksize));
    cv::Mat dst;
    cv::erode(src, dst, element);
    return dst;
}

// Gaussian blut

QString GaussianBlur::name = "Gaussian blur";

GaussianBlur::GaussianBlur(int k, double sx, double sy): ksize(k), sigmaX(sx), sigmaY(sy)
{
    QLabel *ksizeLabel = new QLabel("Kernel size");
    QLabel *sigmaXLabel = new QLabel("Sigma X");
    QLabel *sigmaYLabel = new QLabel("Sigma Y");

    ksizeLineEdit = new CustomLineEdit();
    sigmaXLineEdit = new CustomLineEdit();
    sigmaYLineEdit = new CustomLineEdit();

    QIntValidator *ksizeValidator = new QIntValidator(0, 25, ksizeLineEdit);
    ksizeLineEdit->setValidator(ksizeValidator);
    ksizeLineEdit->setText(QString::number(ksize));

    QDoubleValidator *sigmaXValidator = new QDoubleValidator(0.0, 10.0, 10, sigmaXLineEdit);
    sigmaXValidator->setLocale(QLocale::English);
    sigmaXLineEdit->setValidator(sigmaXValidator);
    sigmaXLineEdit->setText(QString::number(sigmaX));

    QDoubleValidator *sigmaYValidator = new QDoubleValidator(0.0, 10.0, 10, sigmaYLineEdit);
    sigmaYValidator->setLocale(QLocale::English);
    sigmaYLineEdit->setValidator(sigmaYValidator);
    sigmaYLineEdit->setText(QString::number(sigmaY));

    QVBoxLayout *vBoxLayout = new QVBoxLayout;
    vBoxLayout->addWidget(ksizeLabel);
    vBoxLayout->addWidget(ksizeLineEdit);
    vBoxLayout->addWidget(sigmaXLabel);
    vBoxLayout->addWidget(sigmaXLineEdit);
    vBoxLayout->addWidget(sigmaYLabel);
    vBoxLayout->addWidget(sigmaYLineEdit);

    mainWidget = new QWidget(this);
    mainWidget->setLayout(vBoxLayout);

    connect(ksizeLineEdit, &CustomLineEdit::returnPressed, [=](){
        int size = ksizeLineEdit->text().toInt();
        if (size > 0 && size % 2 == 0) size--;
        ksize = size;
        ksizeLineEdit->setText(QString::number(size));
    });
    connect(sigmaXLineEdit, &CustomLineEdit::returnPressed, [=](){ sigmaX = sigmaXLineEdit->text().toDouble(); });
    connect(sigmaYLineEdit, &CustomLineEdit::returnPressed, [=](){ sigmaY = sigmaYLineEdit->text().toDouble(); });
    connect(ksizeLineEdit, &CustomLineEdit::focusOut, [=](){ ksizeLineEdit->setText(QString::number(ksize)); });
    connect(sigmaXLineEdit, &CustomLineEdit::focusOut, [=](){ sigmaXLineEdit->setText(QString::number(sigmaX)); });
    connect(sigmaYLineEdit, &CustomLineEdit::focusOut, [=](){ sigmaYLineEdit->setText(QString::number(sigmaY)); });
}

GaussianBlur::~GaussianBlur()
{
    ksizeLineEdit->disconnect();
    sigmaXLineEdit->disconnect();
    sigmaYLineEdit->disconnect();

    QLayoutItem *child;
    while ((child = mainWidget->layout()->takeAt(0)) != 0)
    {
        delete child;
    }

    delete mainWidget->layout();
    delete mainWidget;
}

QWidget* GaussianBlur::getParametersWidget()
{
    return mainWidget;
}

cv::Mat GaussianBlur::applyOperation(cv::Mat src)
{
    cv::Mat dst;
    cv::GaussianBlur(src, dst, cv::Size(ksize, ksize), sigmaX, sigmaY, cv::BORDER_DEFAULT);
    return dst;
}

// Laplacian

QString Laplacian::name = "Laplacian";

Laplacian::Laplacian(int k, double s, double d): ksize(k), scale(s), delta(d)
{
    QLabel *ksizeLabel = new QLabel("Kernel size");
    QLabel *scaleLabel = new QLabel("Scale");
    QLabel *deltaLabel = new QLabel("Delta");

    ksizeLineEdit = new CustomLineEdit();
    scaleLineEdit = new CustomLineEdit();
    deltaLineEdit = new CustomLineEdit();

    QIntValidator *ksizeValidator = new QIntValidator(0, 25, ksizeLineEdit);
    ksizeLineEdit->setValidator(ksizeValidator);
    ksizeLineEdit->setText(QString::number(ksize));

    QDoubleValidator *scaleValidator = new QDoubleValidator(-100.0, 100.0, 10, scaleLineEdit);
    scaleValidator->setLocale(QLocale::English);
    scaleLineEdit->setValidator(scaleValidator);
    scaleLineEdit->setText(QString::number(scale));

    QDoubleValidator *deltaValidator = new QDoubleValidator(-100.0, 100.0, 10, deltaLineEdit);
    deltaValidator->setLocale(QLocale::English);
    deltaLineEdit->setValidator(deltaValidator);
    deltaLineEdit->setText(QString::number(delta));

    QVBoxLayout *vBoxLayout = new QVBoxLayout;
    vBoxLayout->addWidget(ksizeLabel);
    vBoxLayout->addWidget(ksizeLineEdit);
    vBoxLayout->addWidget(scaleLabel);
    vBoxLayout->addWidget(scaleLineEdit);
    vBoxLayout->addWidget(deltaLabel);
    vBoxLayout->addWidget(deltaLineEdit);

    mainWidget = new QWidget(this);
    mainWidget->setLayout(vBoxLayout);

    connect(ksizeLineEdit, &CustomLineEdit::returnPressed, [=](){
        int size = ksizeLineEdit->text().toInt();
        if (size > 0 && size % 2 == 0) size--;
        ksize = size;
        ksizeLineEdit->setText(QString::number(size));
    });
    connect(scaleLineEdit, &CustomLineEdit::returnPressed, [=](){ scale = scaleLineEdit->text().toDouble(); });
    connect(deltaLineEdit, &CustomLineEdit::returnPressed, [=](){ delta = deltaLineEdit->text().toDouble(); });
    connect(ksizeLineEdit, &CustomLineEdit::focusOut, [=](){ ksizeLineEdit->setText(QString::number(ksize)); });
    connect(scaleLineEdit, &CustomLineEdit::focusOut, [=](){ scaleLineEdit->setText(QString::number(scale)); });
    connect(deltaLineEdit, &CustomLineEdit::focusOut, [=](){ deltaLineEdit->setText(QString::number(delta)); });
}

Laplacian::~Laplacian()
{
    ksizeLineEdit->disconnect();
    scaleLineEdit->disconnect();
    deltaLineEdit->disconnect();

    QLayoutItem *child;
    while ((child = mainWidget->layout()->takeAt(0)) != 0)
    {
        delete child;
    }

    delete mainWidget->layout();
    delete mainWidget;
}

QWidget* Laplacian::getParametersWidget()
{
    return mainWidget;
}

cv::Mat Laplacian::applyOperation(cv::Mat src)
{
    cv::Mat tmp;
    cv::Laplacian(src, tmp, CV_16S, ksize, scale, delta, cv::BORDER_DEFAULT);
    cv::Mat dst;
    cv::convertScaleAbs(tmp, dst);
    return dst;
}

// Morphological operations

QString MorphologyEx::name = "Morphology operation";

MorphologyEx::MorphologyEx(int k): ksize(k)
{
    QLabel *ksizeLabel = new QLabel("Kernel size");

    ksizeLineEdit = new CustomLineEdit();

    QIntValidator *ksizeValidator = new QIntValidator(1, 25, ksizeLineEdit);
    ksizeLineEdit->setValidator(ksizeValidator);
    ksizeLineEdit->setText(QString::number(ksize));

    QVBoxLayout *vBoxLayout = new QVBoxLayout;
    vBoxLayout->addWidget(ksizeLabel);
    vBoxLayout->addWidget(ksizeLineEdit);

    mainWidget = new QWidget(this);
    mainWidget->setLayout(vBoxLayout);

    connect(ksizeLineEdit, &CustomLineEdit::returnPressed, [=](){
        int size = ksizeLineEdit->text().toInt();
        if (size > 0 && size % 2 == 0) size--;
        ksize = size;
        ksizeLineEdit->setText(QString::number(size));
    });
    connect(ksizeLineEdit, &CustomLineEdit::focusOut, [=](){ ksizeLineEdit->setText(QString::number(ksize)); });
}

MorphologyEx::~MorphologyEx()
{
    ksizeLineEdit->disconnect();

    QLayoutItem *child;
    while ((child = mainWidget->layout()->takeAt(0)) != 0)
    {
        delete child;
    }

    delete mainWidget->layout();
    delete mainWidget;
}

QWidget* MorphologyEx::getParametersWidget()
{
    return mainWidget;
}

cv::Mat MorphologyEx::applyOperation(cv::Mat src)
{
    cv::Mat element = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(ksize, ksize));
    cv::Mat dst;
    cv::morphologyEx(src, dst, cv::MORPH_GRADIENT, element);
    return dst;
}

// Rotation

QString Rotation::name = "Rotation";

Rotation::Rotation(double a, double s, cv::InterpolationFlags f): angle(a), scale(s), flag(f)
{
    QLabel *angleLabel = new QLabel("Angle");
    QLabel *scaleLabel = new QLabel("Scale");
    QLabel *flagLabel = new QLabel("Interpolation");

    angleLineEdit = new CustomLineEdit();
    scaleLineEdit = new CustomLineEdit();

    QDoubleValidator *angleValidator = new QDoubleValidator(-360.0, 360.0, 10, angleLineEdit);
    angleValidator->setLocale(QLocale::English);
    angleLineEdit->setValidator(angleValidator);
    angleLineEdit->setText(QString::number(angle));

    QDoubleValidator *scaleValidator = new QDoubleValidator(0.0, 100.0, 10, scaleLineEdit);
    scaleValidator->setLocale(QLocale::English);
    scaleLineEdit->setValidator(scaleValidator);
    scaleLineEdit->setText(QString::number(scale));

    flagCombobox = new QComboBox;
    flagCombobox->addItem("Nearest neighbor");
    flagCombobox->addItem("Bilinear");
    flagCombobox->addItem("Bicubic");
    flagCombobox->addItem("Area");
    flagCombobox->addItem("Lanczos 8x8");
    //flagCombobox->addItem("Bit exact biliniar");
    flagCombobox->setCurrentIndex(0);

    flags[0] = cv::INTER_NEAREST;
    flags[1] = cv::INTER_LINEAR;
    flags[2] = cv::INTER_CUBIC;
    flags[3] = cv::INTER_AREA;
    flags[4] = cv::INTER_LANCZOS4;
    //flags[5] = cv::INTER_LINEAR_EXACT;

    QVBoxLayout *vBoxLayout = new QVBoxLayout;
    vBoxLayout->addWidget(angleLabel);
    vBoxLayout->addWidget(angleLineEdit);
    vBoxLayout->addWidget(scaleLabel);
    vBoxLayout->addWidget(scaleLineEdit);
    vBoxLayout->addWidget(flagLabel);
    vBoxLayout->addWidget(flagCombobox);

    mainWidget = new QWidget(this);
    mainWidget->setLayout(vBoxLayout);

    connect(angleLineEdit, &CustomLineEdit::returnPressed, [=](){ angle = angleLineEdit->text().toDouble(); });
    connect(scaleLineEdit, &CustomLineEdit::returnPressed, [=](){ scale = scaleLineEdit->text().toDouble(); });
    connect(angleLineEdit, &CustomLineEdit::focusOut, [=](){ angleLineEdit->setText(QString::number(angle)); });
    connect(scaleLineEdit, &CustomLineEdit::focusOut, [=](){ scaleLineEdit->setText(QString::number(scale)); });
    connect(flagCombobox, QOverload<int>::of(&QComboBox::activated), [=](int flagIndex){ flag = flags[flagIndex]; });
}

Rotation::~Rotation()
{
    angleLineEdit->disconnect();
    scaleLineEdit->disconnect();
    flagCombobox->disconnect();

    QLayoutItem *child;
    while ((child = mainWidget->layout()->takeAt(0)) != 0)
    {
        delete child;
    }

    delete mainWidget->layout();
    delete mainWidget;
}

QWidget* Rotation::getParametersWidget()
{
    return mainWidget;
}

cv::Mat Rotation::applyOperation(cv::Mat src)
{
    cv::Point center = cv::Point(src.cols / 2, src.rows / 2);
    cv::Mat rotationMat = cv::getRotationMatrix2D(center, angle, scale);
    cv::Mat dst;
    cv::warpAffine(src, dst, rotationMat, src.size(), flag, cv::BORDER_TRANSPARENT);
    return dst;
}
