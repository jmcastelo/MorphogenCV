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

    threshold1LineEdit->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
    threshold2LineEdit->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
    apertureSizeLineEdit->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);

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
    L2gradientCheckBox->setChecked(L2gradient);

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

cv::Mat Canny::applyOperation(const cv::Mat &src)
{
    cv::Mat detectedEdges;
    cv::Canny(src, detectedEdges, threshold1, threshold2, apertureSize, L2gradient);
    cv::Mat dst = cv::Mat(src.rows, src.cols, src.type(), cv::Scalar(0));
    src.copyTo(dst, detectedEdges);
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

    alphaLineEdit->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
    betaLineEdit->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);

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

cv::Mat ConvertTo::applyOperation(const cv::Mat &src)
{
    cv::Mat dst;
    src.convertTo(dst, -1, alpha, beta);
    return dst;
}

// Equalize histogram

QString EqualizeHist::name = "Equalize histogram";

QWidget* EqualizeHist::getParametersWidget()
{
    return mainWidget;
}

cv::Mat EqualizeHist::applyOperation(const cv::Mat &src)
{
    cv::Mat dst;
    cv::cvtColor(src, dst, cv::COLOR_BGR2YCrCb);

    std::vector<cv::Mat> channels;
    cv::split(dst, channels);

    cv::equalizeHist(channels[0], channels[0]);

    cv::merge(channels, dst);

    cv::cvtColor(dst, dst, cv::COLOR_YCrCb2BGR);

    return dst;
}

// Gaussian blur

QString GaussianBlur::name = "Gaussian blur";

GaussianBlur::GaussianBlur(int k, double sx, double sy): ksize(k), sigmaX(sx), sigmaY(sy)
{
    QLabel *ksizeLabel = new QLabel("Kernel size");
    QLabel *sigmaXLabel = new QLabel("Sigma X");
    QLabel *sigmaYLabel = new QLabel("Sigma Y");

    ksizeLineEdit = new CustomLineEdit();
    sigmaXLineEdit = new CustomLineEdit();
    sigmaYLineEdit = new CustomLineEdit();

    ksizeLineEdit->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
    sigmaXLineEdit->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
    sigmaYLineEdit->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);

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

    connect(ksizeLineEdit, &CustomLineEdit::returnPressed, [=]()
    {
        int size = ksizeLineEdit->text().toInt();
        if (size > 0 && size % 2 == 0) size--;
        if (size == 0 && (sigmaX == 0.0 || sigmaY == 0.0))
        {
            ksizeLineEdit->setText(QString::number(ksize));
        }
        else
        {
            ksize = size;
        }
    });
    connect(sigmaXLineEdit, &CustomLineEdit::returnPressed, [=]()
    {
        double sigma = sigmaXLineEdit->text().toDouble();
        if (ksize == 0 && sigma == 0.0)
        {
            sigmaXLineEdit->setText(QString::number(sigmaX));
        }
        else
        {
            sigmaX = sigma;
        }
    });
    connect(sigmaYLineEdit, &CustomLineEdit::returnPressed, [=]()
    {
        double sigma = sigmaYLineEdit->text().toDouble();
        if (ksize == 0 && sigma == 0.0)
        {
            sigmaYLineEdit->setText(QString::number(sigmaY));
        }
        else
        {
            sigmaY = sigma;
        }
    });
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

cv::Mat GaussianBlur::applyOperation(const cv::Mat &src)
{
    cv::Mat dst;
    cv::GaussianBlur(src, dst, cv::Size(ksize, ksize), sigmaX, sigmaY, cv::BORDER_ISOLATED);
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

    ksizeLineEdit->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
    scaleLineEdit->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
    deltaLineEdit->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);

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

cv::Mat Laplacian::applyOperation(const cv::Mat &src)
{
    cv::Mat tmp;
    cv::Laplacian(src, tmp, CV_16SC3, ksize, scale, delta, cv::BORDER_ISOLATED);
    cv::Mat dst;
    cv::convertScaleAbs(tmp, dst);
    return dst;
}

// Mix channels

QString MixChannels::name = "Mix channels";

MixChannels::MixChannels(int b, int g, int r): blue(b), green(g), red(r)
{
    QLabel *blueLabel = new QLabel("Blue");
    QLabel *greenLabel = new QLabel("Green");
    QLabel *redLabel = new QLabel("Red");

    blueComboBox = new QComboBox;
    blueComboBox->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
    blueComboBox->addItem("Blue");
    blueComboBox->addItem("Green");
    blueComboBox->addItem("Red");
    blueComboBox->setCurrentIndex(blue);

    greenComboBox = new QComboBox;
    greenComboBox->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
    greenComboBox->addItem("Blue");
    greenComboBox->addItem("Green");
    greenComboBox->addItem("Red");
    greenComboBox->setCurrentIndex(green);

    redComboBox = new QComboBox;
    redComboBox->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
    redComboBox->addItem("Blue");
    redComboBox->addItem("Green");
    redComboBox->addItem("Red");
    redComboBox->setCurrentIndex(red);

    QVBoxLayout *vBoxLayout = new QVBoxLayout;
    vBoxLayout->addWidget(blueLabel);
    vBoxLayout->addWidget(blueComboBox);
    vBoxLayout->addWidget(greenLabel);
    vBoxLayout->addWidget(greenComboBox);
    vBoxLayout->addWidget(redLabel);
    vBoxLayout->addWidget(redComboBox);

    mainWidget = new QWidget(this);
    mainWidget->setLayout(vBoxLayout);

    fromTo[0] = 0;
    fromTo[1] = blue;
    fromTo[2] = 1;
    fromTo[3] = green;
    fromTo[4] = 2;
    fromTo[5] = red;

    connect(blueComboBox, QOverload<int>::of(&QComboBox::activated), [=](int channelIndex){ fromTo[1] = channelIndex; });
    connect(greenComboBox, QOverload<int>::of(&QComboBox::activated), [=](int channelIndex){ fromTo[3] = channelIndex; });
    connect(redComboBox, QOverload<int>::of(&QComboBox::activated), [=](int channelIndex){ fromTo[5] = channelIndex; });
}

MixChannels::~MixChannels()
{
    blueComboBox->disconnect();
    greenComboBox->disconnect();
    redComboBox->disconnect();

    QLayoutItem *child;
    while ((child = mainWidget->layout()->takeAt(0)) != 0)
    {
        delete child;
    }

    delete mainWidget->layout();
    delete mainWidget;
}

QWidget* MixChannels::getParametersWidget()
{
    return mainWidget;
}

cv::Mat MixChannels::applyOperation(const cv::Mat &src)
{
    cv::Mat dst(src.rows, src.cols, src.type());
    cv::mixChannels(&src, 1, &dst, 1, fromTo, 3);
    return dst;
}

// Morphological operations

QString MorphologyEx::name = "Morphology operation";

MorphologyEx::MorphologyEx(int k, int its, cv::MorphTypes t, cv::MorphShapes s): ksize(k), iterations(its), morphType(t), morphShape(s)
{
    QLabel *morphTypeLabel = new QLabel("Type");
    QLabel *ksizeLabel = new QLabel("Kernel size");
    QLabel *morphShapeLabel = new QLabel("Shape");
    QLabel *iterationsLabel = new QLabel("Iterations");

    morphTypes[0] = cv::MORPH_ERODE;
    morphTypes[1] = cv::MORPH_DILATE;
    morphTypes[2] = cv::MORPH_OPEN;
    morphTypes[3] = cv::MORPH_CLOSE;
    morphTypes[4] = cv::MORPH_GRADIENT;
    morphTypes[5] = cv::MORPH_TOPHAT;
    morphTypes[6] = cv::MORPH_BLACKHAT;

    int morphTypeComboBoxIndex = 0;

    for (int i = 0; i < 7; i++)
    {
        if (morphType == morphTypes[i])
        {
            morphTypeComboBoxIndex = i;
        }
    }

    morphTypeComboBox = new QComboBox;
    morphTypeComboBox->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
    morphTypeComboBox->addItem("Erode");
    morphTypeComboBox->addItem("Dilate");
    morphTypeComboBox->addItem("Opening");
    morphTypeComboBox->addItem("Closing");
    morphTypeComboBox->addItem("Gradient");
    morphTypeComboBox->addItem("Top hat");
    morphTypeComboBox->addItem("Black hat");
    morphTypeComboBox->setCurrentIndex(morphTypeComboBoxIndex);

    ksizeLineEdit = new CustomLineEdit();
    ksizeLineEdit->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);

    QIntValidator *ksizeValidator = new QIntValidator(1, 25, ksizeLineEdit);
    ksizeLineEdit->setValidator(ksizeValidator);
    ksizeLineEdit->setText(QString::number(ksize));

    morphShapes[0] = cv::MORPH_RECT;
    morphShapes[1] = cv::MORPH_CROSS;
    morphShapes[2] = cv::MORPH_ELLIPSE;

    int morphShapeComboBoxIndex = 0;

    for (int i = 0; i < 3; i++)
    {
        if (morphShape == morphShapes[i])
        {
            morphShapeComboBoxIndex = i;
        }
    }

    morphShapeComboBox = new QComboBox;
    morphShapeComboBox->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
    morphShapeComboBox->addItem("Rectangle");
    morphShapeComboBox->addItem("Cross");
    morphShapeComboBox->addItem("Ellipse");
    morphShapeComboBox->setCurrentIndex(morphShapeComboBoxIndex);

    iterationsLineEdit = new CustomLineEdit();
    iterationsLineEdit->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);

    QIntValidator *iterationsValidator = new QIntValidator(1, 100, ksizeLineEdit);
    iterationsLineEdit->setValidator(iterationsValidator);
    iterationsLineEdit->setText(QString::number(iterations));

    QVBoxLayout *vBoxLayout = new QVBoxLayout;
    vBoxLayout->addWidget(morphTypeLabel);
    vBoxLayout->addWidget(morphTypeComboBox);
    vBoxLayout->addWidget(ksizeLabel);
    vBoxLayout->addWidget(ksizeLineEdit);
    vBoxLayout->addWidget(morphShapeLabel);
    vBoxLayout->addWidget(morphShapeComboBox);
    vBoxLayout->addWidget(iterationsLabel);
    vBoxLayout->addWidget(iterationsLineEdit);

    mainWidget = new QWidget(this);
    mainWidget->setLayout(vBoxLayout);

    connect(ksizeLineEdit, &CustomLineEdit::returnPressed, [=](){
        int size = ksizeLineEdit->text().toInt();
        if (size > 0 && size % 2 == 0) size--;
        ksize = size;
        ksizeLineEdit->setText(QString::number(size));
    });
    connect(iterationsLineEdit, &CustomLineEdit::returnPressed, [=](){ iterations = iterationsLineEdit->text().toInt(); });
    connect(ksizeLineEdit, &CustomLineEdit::focusOut, [=](){ ksizeLineEdit->setText(QString::number(ksize)); });
    connect(iterationsLineEdit, &CustomLineEdit::focusOut, [=](){ iterationsLineEdit->setText(QString::number(iterations)); });
    connect(morphTypeComboBox, QOverload<int>::of(&QComboBox::activated), [=](int typeIndex){ morphType = morphTypes[typeIndex]; });
    connect(morphShapeComboBox, QOverload<int>::of(&QComboBox::activated), [=](int shapeIndex){ morphShape = morphShapes[shapeIndex]; });
}

MorphologyEx::~MorphologyEx()
{
    ksizeLineEdit->disconnect();
    morphTypeComboBox->disconnect();
    iterationsLineEdit->disconnect();
    morphShapeComboBox->disconnect();

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

cv::Mat MorphologyEx::applyOperation(const cv::Mat &src)
{
    cv::Mat element = cv::getStructuringElement(morphShape, cv::Size(ksize, ksize));
    cv::Mat dst;
    cv::morphologyEx(src, dst, morphType, element, cv::Point(-1, -1), iterations, cv::BORDER_ISOLATED);
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

    angleLineEdit->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
    scaleLineEdit->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);

    QDoubleValidator *angleValidator = new QDoubleValidator(-360.0, 360.0, 10, angleLineEdit);
    angleValidator->setLocale(QLocale::English);
    angleLineEdit->setValidator(angleValidator);
    angleLineEdit->setText(QString::number(angle));

    QDoubleValidator *scaleValidator = new QDoubleValidator(0.0, 100.0, 10, scaleLineEdit);
    scaleValidator->setLocale(QLocale::English);
    scaleLineEdit->setValidator(scaleValidator);
    scaleLineEdit->setText(QString::number(scale));

    flags[0] = cv::INTER_NEAREST;
    flags[1] = cv::INTER_LINEAR;
    flags[2] = cv::INTER_CUBIC;
    flags[3] = cv::INTER_AREA;
    flags[4] = cv::INTER_LANCZOS4;
    //flags[5] = cv::INTER_LINEAR_EXACT;

    int flagComboBoxIndex = 0;

    for (int i = 0; i < 5; i++)
        if (flag == flags[i])
            flagComboBoxIndex = i;

    flagComboBox = new QComboBox;
    flagComboBox->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
    flagComboBox->addItem("Nearest neighbor");
    flagComboBox->addItem("Bilinear");
    flagComboBox->addItem("Bicubic");
    flagComboBox->addItem("Area");
    flagComboBox->addItem("Lanczos 8x8");
    //flagComboBox->addItem("Bit exact biliniar");
    flagComboBox->setCurrentIndex(flagComboBoxIndex);

    QVBoxLayout *vBoxLayout = new QVBoxLayout;
    vBoxLayout->addWidget(angleLabel);
    vBoxLayout->addWidget(angleLineEdit);
    vBoxLayout->addWidget(scaleLabel);
    vBoxLayout->addWidget(scaleLineEdit);
    vBoxLayout->addWidget(flagLabel);
    vBoxLayout->addWidget(flagComboBox);

    mainWidget = new QWidget(this);
    mainWidget->setLayout(vBoxLayout);

    connect(angleLineEdit, &CustomLineEdit::returnPressed, [=](){ angle = angleLineEdit->text().toDouble(); });
    connect(scaleLineEdit, &CustomLineEdit::returnPressed, [=](){ scale = scaleLineEdit->text().toDouble(); });
    connect(angleLineEdit, &CustomLineEdit::focusOut, [=](){ angleLineEdit->setText(QString::number(angle)); });
    connect(scaleLineEdit, &CustomLineEdit::focusOut, [=](){ scaleLineEdit->setText(QString::number(scale)); });
    connect(flagComboBox, QOverload<int>::of(&QComboBox::activated), [=](int flagIndex){ flag = flags[flagIndex]; });
}

Rotation::~Rotation()
{
    angleLineEdit->disconnect();
    scaleLineEdit->disconnect();
    flagComboBox->disconnect();

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

cv::Mat Rotation::applyOperation(const cv::Mat &src)
{
    cv::Point center = cv::Point(src.cols / 2, src.rows / 2);
    cv::Mat rotationMat = cv::getRotationMatrix2D(center, angle, scale);
    cv::Mat dst;
    cv::warpAffine(src, dst, rotationMat, src.size(), flag);
    return dst;
}

// Sharpen

QString Sharpen::name = "Sharpen";

Sharpen::Sharpen(double s, double t, double a): sigma(s), threshold(t), amount(a)
{
    QLabel *sigmaLabel = new QLabel("Sigma");
    QLabel *thresholdLabel = new QLabel("Threshold");
    QLabel *amountLabel = new QLabel("Amount");

    sigmaLineEdit = new CustomLineEdit();
    thresholdLineEdit = new CustomLineEdit();
    amountLineEdit = new CustomLineEdit();

    sigmaLineEdit->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
    thresholdLineEdit->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
    amountLineEdit->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);

    QDoubleValidator *sigmaValidator = new QDoubleValidator(0.0, 10.0, 10, sigmaLineEdit);
    sigmaValidator->setLocale(QLocale::English);
    sigmaLineEdit->setValidator(sigmaValidator);
    sigmaLineEdit->setText(QString::number(sigma));

    QDoubleValidator *thresholdValidator = new QDoubleValidator(0.0, 100.0, 10, thresholdLineEdit);
    thresholdValidator->setLocale(QLocale::English);
    thresholdLineEdit->setValidator(thresholdValidator);
    thresholdLineEdit->setText(QString::number(threshold));

    QDoubleValidator *amountValidator = new QDoubleValidator(0.0, 10.0, 10, amountLineEdit);
    amountValidator->setLocale(QLocale::English);
    amountLineEdit->setValidator(amountValidator);
    amountLineEdit->setText(QString::number(amount));

    QVBoxLayout *vBoxLayout = new QVBoxLayout;
    vBoxLayout->addWidget(sigmaLabel);
    vBoxLayout->addWidget(sigmaLineEdit);
    vBoxLayout->addWidget(thresholdLabel);
    vBoxLayout->addWidget(thresholdLineEdit);
    vBoxLayout->addWidget(amountLabel);
    vBoxLayout->addWidget(amountLineEdit);

    mainWidget = new QWidget(this);
    mainWidget->setLayout(vBoxLayout);

    connect(sigmaLineEdit, &CustomLineEdit::returnPressed, [=](){ sigma = sigmaLineEdit->text().toDouble(); });
    connect(thresholdLineEdit, &CustomLineEdit::returnPressed, [=](){ threshold = thresholdLineEdit->text().toDouble(); });
    connect(amountLineEdit, &CustomLineEdit::returnPressed, [=](){ amount = amountLineEdit->text().toDouble(); });
    connect(sigmaLineEdit, &CustomLineEdit::focusOut, [=](){ sigmaLineEdit->setText(QString::number(sigma)); });
    connect(thresholdLineEdit, &CustomLineEdit::focusOut, [=](){ thresholdLineEdit->setText(QString::number(threshold)); });
    connect(amountLineEdit, &CustomLineEdit::focusOut, [=](){ amountLineEdit->setText(QString::number(amount)); });
}

Sharpen::~Sharpen()
{
    sigmaLineEdit->disconnect();
    thresholdLineEdit->disconnect();
    amountLineEdit->disconnect();

    QLayoutItem *child;
    while ((child = mainWidget->layout()->takeAt(0)) != 0)
    {
        delete child;
    }

    delete mainWidget->layout();
    delete mainWidget;
}

QWidget* Sharpen::getParametersWidget()
{
    return mainWidget;
}

cv::Mat Sharpen::applyOperation(const cv::Mat &src)
{
    cv::Mat blurred;
    cv::GaussianBlur(src, blurred, cv::Size(), sigma, sigma, cv::BORDER_ISOLATED);
    cv::Mat lowContrastMask = abs(src - blurred) < threshold;
    cv::Mat dst = src * (1 + amount) + blurred * (-amount);
    src.copyTo(dst, lowContrastMask);
    return dst;
}

QString ShiftHue::name = "Shift hue";

ShiftHue::ShiftHue(int d): delta(d)
{
    QLabel *deltaLabel = new QLabel("delta");

    deltaLineEdit = new CustomLineEdit();
    deltaLineEdit->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);

    QIntValidator *deltaValidator = new QIntValidator(-180, 180, deltaLineEdit);
    deltaLineEdit->setValidator(deltaValidator);
    deltaLineEdit->setText(QString::number(delta));

    QVBoxLayout *vBoxLayout = new QVBoxLayout;
    vBoxLayout->addWidget(deltaLabel);
    vBoxLayout->addWidget(deltaLineEdit);

    mainWidget = new QWidget(this);
    mainWidget->setLayout(vBoxLayout);

    connect(deltaLineEdit, &CustomLineEdit::returnPressed, [=](){ delta = deltaLineEdit->text().toInt(); });
    connect(deltaLineEdit, &CustomLineEdit::focusOut, [=](){ deltaLineEdit->setText(QString::number(delta)); });
}

ShiftHue::~ShiftHue()
{
    deltaLineEdit->disconnect();

    QLayoutItem *child;
    while ((child = mainWidget->layout()->takeAt(0)) != 0)
    {
        delete child;
    }

    delete mainWidget->layout();
    delete mainWidget;
}

QWidget* ShiftHue::getParametersWidget()
{
    return mainWidget;
}

cv::Mat ShiftHue::applyOperation(const cv::Mat &src)
{
    cv::Mat hsv;
    cv::cvtColor(src, hsv, cv::COLOR_BGR2HSV);

    hsv.forEach<cv::Vec3b>([&](cv::Vec3b &pixel, const int*){ pixel[0] = (pixel[0] + delta) % 180; });

    cv::Mat dst;
    cv::cvtColor(hsv, dst, cv::COLOR_HSV2BGR);

    return dst;
}
