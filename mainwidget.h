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

#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include "generator.h"
#include "imageiterationplot.h"
#include <vector>
#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QImage>
#include <QPixmap>
#include <QTimer>
#include <QPushButton>
#include <QButtonGroup>
#include <QLineEdit>
#include <QIntValidator>
#include <QDoubleValidator>
#include <QString>
#include <QFileDialog>
#include <QGroupBox>
#include <QCheckBox>
#include <QDir>
#include <QChar>
#include <QComboBox>
#include <QListWidget>
#include <QListWidgetItem>
#include <QModelIndex>
#include <QTabWidget>
#include <QStandardItemModel>
#include <QStandardItem>

class MainWidget : public QWidget
{
    Q_OBJECT

    GeneratorCV *generator;

    ImageIterationPlot *imageIterationPlot;
    ImageIterationPlot *pixelIterationPlot;
    HistogramPlot *histogramPlot;
    ScatterPlot *colorSpacePlot;
    CurvePlot *colorSpacePixelPlot;

    QPushButton *pauseResumePushButton;

    QCheckBox *coloredSeedCheckBox;
    QCheckBox *bwSeedCheckBox;

    QLineEdit *timerIntervalLineEdit;

    QTimer *timer;
    int timerInterval;

    QLineEdit *imageSizeLineEdit;

    QString screenshotPath;
    QString screenshotFilename;
    QPushButton *screenshotPushButton;
    QPushButton *selectScreenshotPathPushButton;
    QLineEdit *screenshotFilenameLineEdit;
    QCheckBox *screenshotSeriesCheckBox;
    bool takeScreenshotSeries;
    unsigned int screenshotIndex;

    QLineEdit *blendFactorLineEdit;

    QComboBox *imageSelectComboBox;

    QComboBox *newImageOperationComboBox;
    QPushButton *insertImageOperationPushButton;
    QPushButton *removeImageOperationPushButton;

    QFormLayout *pipelineBlendFactorsLayout;
    std::vector<QLineEdit*> pipelineBlendFactorLineEdit;

    QListWidget *imageOperationsListWidget;
    std::vector<int> currentImageOperationIndex;

    QVBoxLayout *parametersLayout;

    QPushButton *togglePlotsPushButton;
    QPushButton *imageIterationPushButton;
    QPushButton *pixelIterationPushButton;
    QPushButton *selectPixelPushButton;
    QPushButton *histogramPushButton;
    QPushButton *colorSpacePushButton;
    QPushButton *colorSpacePixelPushButton;

    QComboBox *colorSpaceXAxisComboBox;
    QComboBox *colorSpaceYAxisComboBox;
    QComboBox *colorSpacePixelXAxisComboBox;
    QComboBox *colorSpacePixelYAxisComboBox;

    QTabWidget *plotsTabWidget;

    QWidget *generalControlsWidget;
    QWidget *imageManipulationWidget;
    QWidget *computationWidget;

    void constructGeneralControls();
    void constructImageManipulationControls();
    void constructComputationControls();

    void pauseResumeSystem(bool checked);
    void setTimerInterval();

    void setImageSize();

    void selectScreenshotPath();
    void setScreenshotFilename(QString text);
    void setTakeScreenshotSeries(int state);
    void takeScreenshot();
    void takeScreenshotSeriesElement();

    void initImageSelectComboBox(int imageIndex);

    void initPipelineBlendFactorsLayout();
    void setPipelineBlendFactorLineEditText(int pipelineIndex);

    void initNewImageOperationComboBox();
    void initImageOperationsListWidget(int imageIndex);

    void onImageOperationsListWidgetCurrentRowChanged(int currentRow);
    void onRowsMoved(QModelIndex parent, int start, int end, QModelIndex destination, int row);

    void insertImageOperation();
    void removeImageOperation();

    void applyImageOperations();
    void iterationLoop();

    void togglePlots(bool checked);
    void colorSpaceAxisChanged(int axisIndex, QComboBox *axisComboBox);

    void closeEvent(QCloseEvent *event);

public:
    MainWidget(QWidget *parent = nullptr);
    ~MainWidget();
};
#endif // MAINWIDGET_H
