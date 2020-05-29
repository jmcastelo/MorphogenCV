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

#include "plots.h"

// Image iteration plot

ImageIterationPlot::ImageIterationPlot(QString title, double yMin, double yMax, QWidget *parent = nullptr): QWidget(parent)
{
    itMin = 1000;

    plot = new QCustomPlot(this);

    plot->xAxis->setLabel("Iteration");
    plot->yAxis->setLabel("Intensity");

    plot->yAxis->setRange(yMin, yMax);

    plot->setInteractions(QCP::iRangeZoom | QCP::iRangeDrag);

    plot->axisRect()->setupFullAxesBox(true);
    plot->axisRect()->setRangeZoom(Qt::Vertical | Qt::Horizontal);
    plot->axisRect()->setRangeDrag(Qt::Vertical | Qt::Horizontal);

    plot->plotLayout()->insertRow(0);
    QCPTextElement *text = new QCPTextElement(plot, title);
    plot->plotLayout()->addElement(0, 0, text);

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

// Histogram plot

HistogramPlot::HistogramPlot(QString title, double xMin, double xMax, QWidget *parent = nullptr): QWidget(parent)
{
    plot = new QCustomPlot(this);

    plot->xAxis->setLabel("Intensity");
    plot->yAxis->setLabel("Count");

    plot->xAxis->setRange(xMin, xMax);

    plot->setInteractions(QCP::iRangeZoom | QCP::iRangeDrag);

    plot->axisRect()->setupFullAxesBox(true);
    plot->axisRect()->setRangeZoom(Qt::Vertical | Qt::Horizontal);
    plot->axisRect()->setRangeDrag(Qt::Vertical | Qt::Horizontal);

    plot->plotLayout()->insertRow(0);
    QCPTextElement *text = new QCPTextElement(plot, title);
    plot->plotLayout()->addElement(0, 0, text);

    plot->addGraph();
    plot->graph(0)->setPen(QPen(Qt::blue));

    plot->addGraph();
    plot->graph(1)->setPen(QPen(Qt::green));

    plot->addGraph();
    plot->graph(2)->setPen(QPen(Qt::red));
}

void HistogramPlot::setYMax(double yMax)
{
    plot->yAxis->setRange(0.0, yMax);
}

void HistogramPlot::setData(const QVector<double> &bins, const QVector<double> &blue, const QVector<double> &green, const QVector<double> &red)
{
    plot->graph(0)->setData(bins, blue, true);
    plot->graph(1)->setData(bins, green, true);
    plot->graph(2)->setData(bins, red, true);

    plot->replot();
}

// Scatter plot

ScatterPlot::ScatterPlot(QString title, double xMin, double xMax, double yMin, double yMax, QWidget *parent = nullptr): QWidget(parent)
{
    plot = new QCustomPlot(this);

    plot->xAxis->setLabel("Blue");
    plot->yAxis->setLabel("Green");

    plot->xAxis->setRange(xMin, xMax);
    plot->yAxis->setRange(yMin, yMax);

    plot->setInteractions(QCP::iRangeZoom | QCP::iRangeDrag);

    plot->axisRect()->setupFullAxesBox(true);
    plot->axisRect()->setRangeZoom(Qt::Vertical | Qt::Horizontal);
    plot->axisRect()->setRangeDrag(Qt::Vertical | Qt::Horizontal);

    plot->plotLayout()->insertRow(0);
    QCPTextElement *text = new QCPTextElement(plot, title);
    plot->plotLayout()->addElement(0, 0, text);

    plot->addGraph();
    plot->graph(0)->setLineStyle(QCPGraph::lsNone);
    plot->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc, 1.0));
    plot->graph(0)->setPen(QPen(Qt::black));
    plot->graph(0)->setAdaptiveSampling(false);
}

void ScatterPlot::setAxesLabels(QString xLabel, QString yLabel)
{
    plot->xAxis->setLabel(xLabel);
    plot->yAxis->setLabel(yLabel);
}

void ScatterPlot::setData(const QVector<double> &x, const QVector<double> &y)
{
    plot->graph(0)->setData(x, y, true);
    plot->replot();
}

// Curve plot

CurvePlot::CurvePlot(QString title, double xMin, double xMax, double yMin, double yMax, QWidget *parent = nullptr): QWidget(parent)
{
    plot = new QCustomPlot(this);

    plot->xAxis->setLabel("Blue");
    plot->yAxis->setLabel("Green");

    plot->xAxis->setRange(xMin, xMax);
    plot->yAxis->setRange(yMin, yMax);

    plot->setInteractions(QCP::iRangeZoom | QCP::iRangeDrag);

    plot->axisRect()->setupFullAxesBox(true);
    plot->axisRect()->setRangeZoom(Qt::Vertical | Qt::Horizontal);
    plot->axisRect()->setRangeDrag(Qt::Vertical | Qt::Horizontal);

    plot->plotLayout()->insertRow(0);
    QCPTextElement *text = new QCPTextElement(plot, title);
    plot->plotLayout()->addElement(0, 0, text);

    curve = new QCPCurve(plot->xAxis, plot->yAxis);
}

void CurvePlot::setAxesLabels(QString xLabel, QString yLabel)
{
    plot->xAxis->setLabel(xLabel);
    plot->yAxis->setLabel(yLabel);
}

void CurvePlot::addPoint(double x, double y)
{
    curve->addData(x, y);
    plot->replot();
}

void CurvePlot::clearCurveData()
{
    curve->data()->clear();
}
