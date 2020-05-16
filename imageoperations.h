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
#include <QSpinBox>
#include <QFormLayout>

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
protected:
    QWidget *mainWidget;
    int step;

public:
    bool isEnabled(int iteration){ return (step > 0) && (iteration % step == 0); }
    QWidget* getParametersWidget(){ return mainWidget; };
    virtual QString getName() = 0;
    virtual cv::Mat applyOperation(const cv::Mat &src) = 0;

    ImageOperation(int stp): step(stp){};
    ~ImageOperation()
    {
        delete mainWidget->layout();
        delete mainWidget;
    };
};

// Canny

class Canny: public QWidget, public ImageOperation
{
    double threshold1, threshold2;
    int apertureSize;
    bool L2gradient;

public:
    static QString name;

    Canny(int stp, double th1, double th2, int size, bool g);

    QString getName(){ return name; };
    cv::Mat applyOperation(const cv::Mat &src);
};

// Convert to

class ConvertTo: public QWidget, public ImageOperation
{
    double alpha, beta;

public:
    static QString name;

    ConvertTo(int stp, double a, double b);

    QString getName(){ return name; };
    cv::Mat applyOperation(const cv::Mat &src);
};

// Equalize histogram

class EqualizeHist: public QWidget, public ImageOperation
{
public:
    static QString name;

    EqualizeHist(int stp);

    QString getName(){ return name; };
    cv::Mat applyOperation(const cv::Mat &src);
};

// Gaussian blur

class GaussianBlur: public QWidget, public ImageOperation
{
    int ksize;
    double sigmaX, sigmaY;

public:
    static QString name;

    GaussianBlur(int stp, int k, double sx, double sy);

    QString getName(){ return name; };
    cv::Mat applyOperation(const cv::Mat &src);
};

// Laplacian

class Laplacian: public QWidget, public ImageOperation
{
    int ksize;
    double scale, delta;

public:
    static QString name;

    Laplacian(int stp, int k, double s, double d);

    QString getName(){ return name; };
    cv::Mat applyOperation(const cv::Mat &src);
};

// Mix channels

class MixChannels: public QWidget, public ImageOperation
{
    int blue, green, red;
    int fromTo[6];

public:
    static QString name;

    MixChannels(int stp, int b, int g, int r);

    QString getName(){ return name; };
    cv::Mat applyOperation(const cv::Mat &src);
};

// Morphological transformations

class MorphologyEx: public QWidget, public ImageOperation
{
    int ksize, iterations;
    cv::MorphTypes morphType, morphTypes[7];
    cv::MorphShapes morphShape, morphShapes[3];

public:
    static QString name;

    MorphologyEx(int stp, int k, int its, cv::MorphTypes t, cv::MorphShapes s);

    QString getName(){ return name; };
    cv::Mat applyOperation(const cv::Mat &src);
};

// Rotation

class Rotation: public QWidget, public ImageOperation
{
    double angle, scale;
    cv::InterpolationFlags flag, flags[5];

public:
    static QString name;

    Rotation(int stp, double a, double s, cv::InterpolationFlags f);

    QString getName(){ return name; };
    cv::Mat applyOperation(const cv::Mat &src);
};

// Sharpen

class Sharpen: public QWidget, public ImageOperation
{
    double sigma, threshold, amount;

public:
    static QString name;

    Sharpen(int stp, double s, double t, double a);

    QString getName(){ return name; };
    cv::Mat applyOperation(const cv::Mat &src);
};

// Shift hue

class ShiftHue: public QWidget, public ImageOperation
{
    int delta;

public:
    static QString name;

    ShiftHue(int stp, int d);

    QString getName(){ return name; };
    cv::Mat applyOperation(const cv::Mat &src);
};

#endif // IMAGEOPERATIONS_H
