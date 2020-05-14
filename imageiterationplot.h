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

#ifndef IMAGEITERATIONPLOT_H
#define IMAGEITERATIONPLOT_H

#include "qcustomplot.h"
#include <QWidget>
#include <QVector>
#include <QString>

class ImageIterationPlot: public QWidget
{
public:
    int itMin;
    QCustomPlot *plot;
    QString plotTitle;

    ImageIterationPlot(QString title, double yMin, double yMax);

    void addPoint(double it, double blue, double green, double red);
    void clearGraphsData();
};

class HistogramPlot: public QWidget
{
public:
    QCustomPlot *plot;
    QString plotTitle;

    HistogramPlot(QString title);

    void setYMax(double yMax);
    void setData(const QVector<double> &bins, const QVector<double> &blue, const QVector<double> &green, const QVector<double> &red);
};

#endif // IMAGEITERATIONPLOT_H
