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

#include "imageiterationplot.h"

ImageIterationPlot::ImageIterationPlot()
{
    itMin = 500;

    plot = new QCustomPlot(this);

    plot->xAxis->setLabel("Iteration");
    plot->yAxis->setLabel("Intensity");

    plot->yAxis->setRange(0.0, 1.0);

    plot->setInteractions(QCP::iRangeZoom | QCP::iRangeDrag);

    plot->axisRect()->setupFullAxesBox(true);
    plot->axisRect()->setRangeZoom(Qt::Vertical | Qt::Horizontal);
    plot->axisRect()->setRangeDrag(Qt::Vertical | Qt::Horizontal);

    plot->plotLayout()->insertRow(0);
    //QCPTextElement *title = new QCPTextElement(plot, "Image color", QFont("sans", 17, QFont::Bold));
    QCPTextElement *title = new QCPTextElement(plot, "Evolution of blue, green and red channels");
    plot->plotLayout()->addElement(0, 0, title);

    plot->addGraph();
    plot->graph(0)->setPen(QPen(Qt::blue));

    plot->addGraph();
    plot->graph(1)->setPen(QPen(Qt::green));

    plot->addGraph();
    plot->graph(2)->setPen(QPen(Qt::red));
}

void ImageIterationPlot::addPoint(double it, double blue, double green, double red)
{
    plot->graph(0)->addData(it, blue);
    plot->graph(1)->addData(it, green);
    plot->graph(2)->addData(it, red);

    if (it > itMin)
    {
        plot->xAxis->setRange(it - itMin, it);
    }
    else
    {
        plot->xAxis->rescale();
    }

    plot->replot();
}

void ImageIterationPlot::clearGraphsData()
{
    plot->graph(0)->data()->clear();
    plot->graph(1)->data()->clear();
    plot->graph(2)->data()->clear();
}
