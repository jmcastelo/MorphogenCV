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

#ifndef IMAGEOPERATIONS_H
#define IMAGEOPERATIONS_H

#include <opencv2/imgproc.hpp>
#include <QWidget>
#include <QString>
#include <QLabel>
#include <QLineEdit>
#include <QIntValidator>
#include <QDoubleValidator>
#include <QComboBox>
#include <QCheckBox>
#include <QVBoxLayout>

// A custom QLineEdit that signals focus out

class CustomLineEdit: public QLineEdit
{
    Q_OBJECT

public:
    CustomLineEdit(QWidget *parent = nullptr): QLineEdit(parent){}

protected:
    void focusOutEvent(QFocusEvent *event)
    {
        QLineEdit::focusOutEvent(event);
        emit focusOut();
    }

signals:
    void focusOut();
};

// Base image operation class

class ImageOperation
{
public:
    virtual QString getName() = 0;
    virtual QWidget* getParametersWidget() = 0;
    virtual cv::Mat applyOperation(const cv::Mat &src) = 0;
    virtual ~ImageOperation() = 0;
};

// Canny

class Canny: public QWidget, public ImageOperation
{
    double threshold1, threshold2;
    int apertureSize;
    bool L2gradient;

    CustomLineEdit *threshold1LineEdit;
    CustomLineEdit *threshold2LineEdit;
    CustomLineEdit *apertureSizeLineEdit;
    QCheckBox *L2gradientCheckBox;
    QWidget *mainWidget;

public:
    static QString name;

    Canny(double th1, double th2, int s, bool g);
    ~Canny();

    QString getName(){ return name; };
    QWidget *getParametersWidget();
    cv::Mat applyOperation(const cv::Mat &src);
};

// Convert to

class ConvertTo: public QWidget, public ImageOperation
{
    double alpha, beta;

    CustomLineEdit *alphaLineEdit;
    CustomLineEdit *betaLineEdit;
    QWidget *mainWidget;

public:
    static QString name;

    ConvertTo(double a, double b);
    ~ConvertTo();

    QString getName(){ return name; };
    QWidget *getParametersWidget();
    cv::Mat applyOperation(const cv::Mat &src);
};

class EqualizeHist: public QWidget, public ImageOperation
{
    QWidget *mainWidget;

public:
    static QString name;

    EqualizeHist(){ mainWidget = new QWidget(this); };
    ~EqualizeHist(){ delete mainWidget; };

    QString getName(){ return name; };
    QWidget *getParametersWidget();
    cv::Mat applyOperation(const cv::Mat &src);
};

// Gaussian blur

class GaussianBlur: public QWidget, public ImageOperation
{
    int ksize;
    double sigmaX, sigmaY;

    CustomLineEdit *ksizeLineEdit;
    CustomLineEdit *sigmaXLineEdit;
    CustomLineEdit *sigmaYLineEdit;
    QWidget *mainWidget;

public:
    static QString name;

    GaussianBlur(int k, double sx, double sy);
    ~GaussianBlur();

    QString getName(){ return name; };
    QWidget *getParametersWidget();
    cv::Mat applyOperation(const cv::Mat &src);
};

// Laplacian

class Laplacian: public QWidget, public ImageOperation
{
    int ksize;
    double scale, delta;

    CustomLineEdit *ksizeLineEdit;
    CustomLineEdit *scaleLineEdit;
    CustomLineEdit *deltaLineEdit;
    QWidget *mainWidget;

public:
    static QString name;

    Laplacian(int k, double s, double d);
    ~Laplacian();

    QString getName(){ return name; };
    QWidget *getParametersWidget();
    cv::Mat applyOperation(const cv::Mat &src);
};

// Mix channels

class MixChannels: public QWidget, public ImageOperation
{
    int blue, green, red;
    int fromTo[6];

    QComboBox *blueComboBox;
    QComboBox *greenComboBox;
    QComboBox *redComboBox;
    QWidget *mainWidget;

public:
    static QString name;

    MixChannels(int b, int g, int r);
    ~MixChannels();

    QString getName(){ return name; };
    QWidget *getParametersWidget();
    cv::Mat applyOperation(const cv::Mat &src);
};

// Morphological transformations

class MorphologyEx: public QWidget, public ImageOperation
{
    int ksize, iterations;
    cv::MorphTypes morphType, morphTypes[7];
    cv::MorphShapes morphShape, morphShapes[3];

    CustomLineEdit *ksizeLineEdit;
    CustomLineEdit *iterationsLineEdit;
    QComboBox *morphTypeComboBox;
    QComboBox *morphShapeComboBox;
    QWidget *mainWidget;

public:
    static QString name;

    MorphologyEx(int k, int its, cv::MorphTypes t, cv::MorphShapes s);
    ~MorphologyEx();

    QString getName(){ return name; };
    QWidget *getParametersWidget();
    cv::Mat applyOperation(const cv::Mat &src);
};

// Rotation

class Rotation: public QWidget, public ImageOperation
{
    double angle, scale;
    cv::InterpolationFlags flag, flags[5];

    CustomLineEdit *angleLineEdit;
    CustomLineEdit *scaleLineEdit;
    QComboBox *flagComboBox;
    QWidget *mainWidget;

public:
    static QString name;

    Rotation(double a, double s, cv::InterpolationFlags f);
    ~Rotation();

    QString getName(){ return name; };
    QWidget *getParametersWidget();
    cv::Mat applyOperation(const cv::Mat &src);
};

// Sharpen

class Sharpen: public QWidget, public ImageOperation
{
    double sigma, threshold, amount;

    CustomLineEdit *sigmaLineEdit;
    CustomLineEdit *thresholdLineEdit;
    CustomLineEdit *amountLineEdit;
    QWidget *mainWidget;

public:
    static QString name;

    Sharpen(double s, double t, double a);
    ~Sharpen();

    QString getName(){ return name; };
    QWidget *getParametersWidget();
    cv::Mat applyOperation(const cv::Mat &src);
};

#endif // IMAGEOPERATIONS_H
