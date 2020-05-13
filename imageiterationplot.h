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

class ImageIterationPlot: public QWidget
{
public:
    int itMin;
    QCustomPlot *plot;

    ImageIterationPlot();

    void addPoint(double it, double blue, double green, double red);
    void clearGraphsData();
};

#endif // IMAGEITERATIONPLOT_H
