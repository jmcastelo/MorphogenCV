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

// Canny

QString Canny::name = "Canny";

Canny::Canny(int stp, double th1, double th2, int size, bool g): ImageOperation(stp), threshold1(th1), threshold2(th2), apertureSize(size), L2gradient(g)
{
    CustomLineEdit *threshold1LineEdit = new CustomLineEdit();
    CustomLineEdit *threshold2LineEdit = new CustomLineEdit();
    CustomLineEdit *apertureSizeLineEdit = new CustomLineEdit();

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

    QCheckBox *L2gradientCheckBox = new QCheckBox("L2 gradient");
    L2gradientCheckBox->setChecked(L2gradient);

    QSpinBox *stepSpinBox = new QSpinBox;
    stepSpinBox->setRange(0, 10000);
    stepSpinBox->setValue(step);

    QFormLayout *formLayout = new QFormLayout;
    formLayout->addRow("Threshold 1:", threshold1LineEdit);
    formLayout->addRow("Threshold 2:", threshold2LineEdit);
    formLayout->addRow("Aperture size:", apertureSizeLineEdit);
    formLayout->addRow("Step:", stepSpinBox);

    mainWidget = new QWidget(this);
    mainWidget->setLayout(formLayout);

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
    connect(stepSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), [=](int i){ step = i; });
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

ConvertTo::ConvertTo(int stp, double a, double b): ImageOperation(stp), alpha(a), beta(b)
{
    CustomLineEdit *alphaLineEdit = new CustomLineEdit();
    CustomLineEdit *betaLineEdit = new CustomLineEdit();

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

    QSpinBox *stepSpinBox = new QSpinBox;
    stepSpinBox->setRange(0, 10000);
    stepSpinBox->setValue(step);

    QFormLayout *formLayout = new QFormLayout;
    formLayout->addRow("Alpha:", alphaLineEdit);
    formLayout->addRow("Beta:", betaLineEdit);
    formLayout->addRow("Step:", stepSpinBox);

    mainWidget = new QWidget(this);
    mainWidget->setLayout(formLayout);

    connect(alphaLineEdit, &CustomLineEdit::returnPressed, [=](){ alpha = alphaLineEdit->text().toDouble(); });
    connect(betaLineEdit, &CustomLineEdit::returnPressed, [=](){ beta = betaLineEdit->text().toDouble(); });
    connect(alphaLineEdit, &CustomLineEdit::focusOut, [=](){ alphaLineEdit->setText(QString::number(alpha)); });
    connect(betaLineEdit, &CustomLineEdit::focusOut, [=](){ betaLineEdit->setText(QString::number(beta)); });
    connect(stepSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), [=](int i){ step = i; });
}

cv::Mat ConvertTo::applyOperation(const cv::Mat &src)
{
    cv::Mat dst;
    src.convertTo(dst, -1, alpha, beta);
    return dst;
}

// Equalize histogram

QString EqualizeHist::name = "Equalize histogram";

EqualizeHist::EqualizeHist(int stp): ImageOperation(stp)
{
    QSpinBox *stepSpinBox = new QSpinBox;
    stepSpinBox->setRange(0, 10000);
    stepSpinBox->setValue(step);

    QFormLayout *formLayout = new QFormLayout;
    formLayout->addRow("Step:", stepSpinBox);

    mainWidget = new QWidget(this);
    mainWidget->setLayout(formLayout);

    connect(stepSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), [=](int i){ step = i; });
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

GaussianBlur::GaussianBlur(int stp, int k, double sx, double sy): ImageOperation(stp), ksize(k), sigmaX(sx), sigmaY(sy)
{
    CustomLineEdit *ksizeLineEdit = new CustomLineEdit();
    CustomLineEdit *sigmaXLineEdit = new CustomLineEdit();
    CustomLineEdit *sigmaYLineEdit = new CustomLineEdit();

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

    QSpinBox *stepSpinBox = new QSpinBox;
    stepSpinBox->setRange(0, 10000);
    stepSpinBox->setValue(step);

    QFormLayout *formLayout = new QFormLayout;
    formLayout->addRow("Kernel size:", ksizeLineEdit);
    formLayout->addRow("Sigma X:", sigmaXLineEdit);
    formLayout->addRow("Sigma Y:", sigmaYLineEdit);
    formLayout->addRow("Step:", stepSpinBox);

    mainWidget = new QWidget(this);
    mainWidget->setLayout(formLayout);

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
    connect(stepSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), [=](int i){ step = i; });
}

cv::Mat GaussianBlur::applyOperation(const cv::Mat &src)
{
    cv::Mat dst;
    cv::GaussianBlur(src, dst, cv::Size(ksize, ksize), sigmaX, sigmaY, cv::BORDER_ISOLATED);
    return dst;
}

// Laplacian

QString Laplacian::name = "Laplacian";

Laplacian::Laplacian(int stp, int k, double s, double d): ImageOperation(stp), ksize(k), scale(s), delta(d)
{
    CustomLineEdit *ksizeLineEdit = new CustomLineEdit();
    CustomLineEdit *scaleLineEdit = new CustomLineEdit();
    CustomLineEdit *deltaLineEdit = new CustomLineEdit();

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

    QSpinBox *stepSpinBox = new QSpinBox;
    stepSpinBox->setRange(0, 10000);
    stepSpinBox->setValue(step);

    QFormLayout *formLayout = new QFormLayout;
    formLayout->addRow("Kernel size:", ksizeLineEdit);
    formLayout->addRow("Scale:", scaleLineEdit);
    formLayout->addRow("Delta:", deltaLineEdit);
    formLayout->addRow("Step:", stepSpinBox);

    mainWidget = new QWidget(this);
    mainWidget->setLayout(formLayout);

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
    connect(stepSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), [=](int i){ step = i; });
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

MixChannels::MixChannels(int stp, int b, int g, int r): ImageOperation(stp), blue(b), green(g), red(r)
{
    QComboBox *blueComboBox = new QComboBox;
    blueComboBox->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
    blueComboBox->addItem("Blue");
    blueComboBox->addItem("Green");
    blueComboBox->addItem("Red");
    blueComboBox->setCurrentIndex(blue);

    QComboBox *greenComboBox = new QComboBox;
    greenComboBox->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
    greenComboBox->addItem("Blue");
    greenComboBox->addItem("Green");
    greenComboBox->addItem("Red");
    greenComboBox->setCurrentIndex(green);

    QComboBox *redComboBox = new QComboBox;
    redComboBox->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
    redComboBox->addItem("Blue");
    redComboBox->addItem("Green");
    redComboBox->addItem("Red");
    redComboBox->setCurrentIndex(red);

    QSpinBox *stepSpinBox = new QSpinBox;
    stepSpinBox->setRange(0, 10000);
    stepSpinBox->setValue(step);

    QFormLayout *formLayout = new QFormLayout;
    formLayout->addRow("Blue to:", blueComboBox);
    formLayout->addRow("Green to:", greenComboBox);
    formLayout->addRow("Red to:", redComboBox);
    formLayout->addRow("Step:", stepSpinBox);

    mainWidget = new QWidget(this);
    mainWidget->setLayout(formLayout);

    fromTo[0] = 0;
    fromTo[1] = blue;
    fromTo[2] = 1;
    fromTo[3] = green;
    fromTo[4] = 2;
    fromTo[5] = red;

    connect(blueComboBox, QOverload<int>::of(&QComboBox::activated), [=](int channelIndex){ fromTo[1] = channelIndex; });
    connect(greenComboBox, QOverload<int>::of(&QComboBox::activated), [=](int channelIndex){ fromTo[3] = channelIndex; });
    connect(redComboBox, QOverload<int>::of(&QComboBox::activated), [=](int channelIndex){ fromTo[5] = channelIndex; });
    connect(stepSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), [=](int i){ step = i; });
}

cv::Mat MixChannels::applyOperation(const cv::Mat &src)
{
    cv::Mat dst(src.rows, src.cols, src.type());
    cv::mixChannels(&src, 1, &dst, 1, fromTo, 3);
    return dst;
}

// Morphological operations

QString MorphologyEx::name = "Morphology operation";

MorphologyEx::MorphologyEx(int stp, int k, int its, cv::MorphTypes t, cv::MorphShapes s): ImageOperation(stp), ksize(k), iterations(its), morphType(t), morphShape(s)
{
    morphTypes[0] = cv::MORPH_ERODE;
    morphTypes[1] = cv::MORPH_DILATE;
    morphTypes[2] = cv::MORPH_OPEN;
    morphTypes[3] = cv::MORPH_CLOSE;
    morphTypes[4] = cv::MORPH_GRADIENT;
    morphTypes[5] = cv::MORPH_TOPHAT;
    morphTypes[6] = cv::MORPH_BLACKHAT;

    int morphTypeComboBoxIndex = 0;

    for (int i = 0; i < 7; i++)
        if (morphType == morphTypes[i])
            morphTypeComboBoxIndex = i;

    QComboBox *morphTypeComboBox = new QComboBox;
    morphTypeComboBox->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
    morphTypeComboBox->addItem("Erode");
    morphTypeComboBox->addItem("Dilate");
    morphTypeComboBox->addItem("Opening");
    morphTypeComboBox->addItem("Closing");
    morphTypeComboBox->addItem("Gradient");
    morphTypeComboBox->addItem("Top hat");
    morphTypeComboBox->addItem("Black hat");
    morphTypeComboBox->setCurrentIndex(morphTypeComboBoxIndex);

    CustomLineEdit *ksizeLineEdit = new CustomLineEdit();
    ksizeLineEdit->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);

    QIntValidator *ksizeValidator = new QIntValidator(1, 25, ksizeLineEdit);
    ksizeLineEdit->setValidator(ksizeValidator);
    ksizeLineEdit->setText(QString::number(ksize));

    morphShapes[0] = cv::MORPH_RECT;
    morphShapes[1] = cv::MORPH_CROSS;
    morphShapes[2] = cv::MORPH_ELLIPSE;

    int morphShapeComboBoxIndex = 0;

    for (int i = 0; i < 3; i++)
        if (morphShape == morphShapes[i])
            morphShapeComboBoxIndex = i;

    QComboBox *morphShapeComboBox = new QComboBox;
    morphShapeComboBox->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
    morphShapeComboBox->addItem("Rectangle");
    morphShapeComboBox->addItem("Cross");
    morphShapeComboBox->addItem("Ellipse");
    morphShapeComboBox->setCurrentIndex(morphShapeComboBoxIndex);

    CustomLineEdit *iterationsLineEdit = new CustomLineEdit();
    iterationsLineEdit->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);

    QIntValidator *iterationsValidator = new QIntValidator(1, 100, ksizeLineEdit);
    iterationsLineEdit->setValidator(iterationsValidator);
    iterationsLineEdit->setText(QString::number(iterations));

    QSpinBox *stepSpinBox = new QSpinBox;
    stepSpinBox->setRange(0, 10000);
    stepSpinBox->setValue(step);

    QFormLayout *formLayout = new QFormLayout;
    formLayout->addRow("Type:", morphTypeComboBox);
    formLayout->addRow("Kernel size:", ksizeLineEdit);
    formLayout->addRow("Shape:", morphShapeComboBox);
    formLayout->addRow("Iterations:", iterationsLineEdit);
    formLayout->addRow("Step:", stepSpinBox);

    mainWidget = new QWidget(this);
    mainWidget->setLayout(formLayout);

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
    connect(stepSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), [=](int i){ step = i; });
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

Rotation::Rotation(int stp, double a, double s, cv::InterpolationFlags f): ImageOperation(stp), angle(a), scale(s), flag(f)
{
    CustomLineEdit *angleLineEdit = new CustomLineEdit();
    CustomLineEdit *scaleLineEdit = new CustomLineEdit();

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

    QComboBox *flagComboBox = new QComboBox;
    flagComboBox->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
    flagComboBox->addItem("Nearest neighbor");
    flagComboBox->addItem("Bilinear");
    flagComboBox->addItem("Bicubic");
    flagComboBox->addItem("Area");
    flagComboBox->addItem("Lanczos 8x8");
    //flagComboBox->addItem("Bit exact biliniar");
    flagComboBox->setCurrentIndex(flagComboBoxIndex);

    QSpinBox *stepSpinBox = new QSpinBox;
    stepSpinBox->setRange(0, 10000);
    stepSpinBox->setValue(step);

    QFormLayout *formLayout = new QFormLayout;
    formLayout->addRow("Angle:", angleLineEdit);
    formLayout->addRow("Scale:", scaleLineEdit);
    formLayout->addRow("Interpolation:", flagComboBox);
    formLayout->addRow("Step:", stepSpinBox);

    mainWidget = new QWidget(this);
    mainWidget->setLayout(formLayout);

    connect(angleLineEdit, &CustomLineEdit::returnPressed, [=](){ angle = angleLineEdit->text().toDouble(); });
    connect(scaleLineEdit, &CustomLineEdit::returnPressed, [=](){ scale = scaleLineEdit->text().toDouble(); });
    connect(angleLineEdit, &CustomLineEdit::focusOut, [=](){ angleLineEdit->setText(QString::number(angle)); });
    connect(scaleLineEdit, &CustomLineEdit::focusOut, [=](){ scaleLineEdit->setText(QString::number(scale)); });
    connect(flagComboBox, QOverload<int>::of(&QComboBox::activated), [=](int flagIndex){ flag = flags[flagIndex]; });
    connect(stepSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), [=](int i){ step = i; });
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

Sharpen::Sharpen(int stp, double s, double t, double a): ImageOperation(stp), sigma(s), threshold(t), amount(a)
{
    CustomLineEdit *sigmaLineEdit = new CustomLineEdit();
    CustomLineEdit *thresholdLineEdit = new CustomLineEdit();
    CustomLineEdit *amountLineEdit = new CustomLineEdit();

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

    QSpinBox *stepSpinBox = new QSpinBox;
    stepSpinBox->setRange(0, 10000);
    stepSpinBox->setValue(step);

    QFormLayout *formLayout = new QFormLayout;
    formLayout->addRow("Sigma:", sigmaLineEdit);
    formLayout->addRow("Threshold:", thresholdLineEdit);
    formLayout->addRow("Amount:", amountLineEdit);
    formLayout->addRow("Step:", stepSpinBox);

    mainWidget = new QWidget(this);
    mainWidget->setLayout(formLayout);

    connect(sigmaLineEdit, &CustomLineEdit::returnPressed, [=](){ sigma = sigmaLineEdit->text().toDouble(); });
    connect(thresholdLineEdit, &CustomLineEdit::returnPressed, [=](){ threshold = thresholdLineEdit->text().toDouble(); });
    connect(amountLineEdit, &CustomLineEdit::returnPressed, [=](){ amount = amountLineEdit->text().toDouble(); });
    connect(sigmaLineEdit, &CustomLineEdit::focusOut, [=](){ sigmaLineEdit->setText(QString::number(sigma)); });
    connect(thresholdLineEdit, &CustomLineEdit::focusOut, [=](){ thresholdLineEdit->setText(QString::number(threshold)); });
    connect(amountLineEdit, &CustomLineEdit::focusOut, [=](){ amountLineEdit->setText(QString::number(amount)); });
    connect(stepSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), [=](int i){ step = i; });
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

ShiftHue::ShiftHue(int stp, int d): ImageOperation(stp), delta(d)
{
    CustomLineEdit *deltaLineEdit = new CustomLineEdit();
    deltaLineEdit->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);

    QIntValidator *deltaValidator = new QIntValidator(-180, 180, deltaLineEdit);
    deltaLineEdit->setValidator(deltaValidator);
    deltaLineEdit->setText(QString::number(delta));

    QSpinBox *stepSpinBox = new QSpinBox;
    stepSpinBox->setRange(0, 10000);
    stepSpinBox->setValue(step);

    QFormLayout *formLayout = new QFormLayout;
    formLayout->addRow("Delta:", deltaLineEdit);
    formLayout->addRow("Step:", stepSpinBox);

    mainWidget = new QWidget(this);
    mainWidget->setLayout(formLayout);

    connect(deltaLineEdit, &CustomLineEdit::returnPressed, [=](){ delta = deltaLineEdit->text().toInt(); });
    connect(deltaLineEdit, &CustomLineEdit::focusOut, [=](){ deltaLineEdit->setText(QString::number(delta)); });
    connect(stepSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), [=](int i){ step = i; });
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
