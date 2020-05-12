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
    virtual cv::Mat applyOperation(cv::Mat src) = 0;
    virtual ~ImageOperation() = 0;
};

// Convert to

class ConvertTo: public QWidget, public ImageOperation
{
    static QString name;

    double alpha, beta;

    CustomLineEdit *alphaLineEdit;
    CustomLineEdit *betaLineEdit;
    QWidget *mainWidget;

public:
    ConvertTo(double a, double b);
    ~ConvertTo();

    QString getName(){ return name; };
    QWidget *getParametersWidget();
    cv::Mat applyOperation(cv::Mat src);
};

// Dilate

class Dilate: public QWidget, public ImageOperation
{
    static QString name;

    int ksize;

    CustomLineEdit *ksizeLineEdit;
    QWidget *mainWidget;

public:
    Dilate(int k);
    ~Dilate();

    QString getName(){ return name; };
    QWidget *getParametersWidget();
    cv::Mat applyOperation(cv::Mat src);
};

// Erode

class Erode: public QWidget, public ImageOperation
{
    static QString name;

    int ksize;

    CustomLineEdit *ksizeLineEdit;
    QWidget *mainWidget;

public:
    Erode(int k);
    ~Erode();

    QString getName(){ return name; };
    QWidget *getParametersWidget();
    cv::Mat applyOperation(cv::Mat src);
};

// Gaussian blur

class GaussianBlur: public QWidget, public ImageOperation
{
    static QString name;

    int ksize;
    double sigmaX, sigmaY;

    CustomLineEdit *ksizeLineEdit;
    CustomLineEdit *sigmaXLineEdit;
    CustomLineEdit *sigmaYLineEdit;
    QWidget *mainWidget;

public:
    GaussianBlur(int k, double sx, double sy);
    ~GaussianBlur();

    QString getName(){ return name; };
    QWidget *getParametersWidget();
    cv::Mat applyOperation(cv::Mat src);
};

// Laplacian

class Laplacian: public QWidget, public ImageOperation
{
    static QString name;

    int ksize;
    double scale, delta;

    CustomLineEdit *ksizeLineEdit;
    CustomLineEdit *scaleLineEdit;
    CustomLineEdit *deltaLineEdit;
    QWidget *mainWidget;

public:
    Laplacian(int k, double s, double d);
    ~Laplacian();

    QString getName(){ return name; };
    QWidget *getParametersWidget();
    cv::Mat applyOperation(cv::Mat src);
};

// Rotation

class Rotation: public QWidget, public ImageOperation
{
    static QString name;

    double angle, scale;
    int flags;

    CustomLineEdit *angleLineEdit;
    CustomLineEdit *scaleLineEdit;
    CustomLineEdit *flagsLineEdit;
    QWidget *mainWidget;

public:
    Rotation(double a, double s, int f);
    ~Rotation();

    QString getName(){ return name; };
    QWidget *getParametersWidget();
    cv::Mat applyOperation(cv::Mat src);
};

#endif // IMAGEOPERATIONS_H
