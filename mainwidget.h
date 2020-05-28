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
#include "parameterwidget.h"
#include "plots.h"
#include "configparser.h"
#include <vector>
#include <chrono>
#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QGridLayout>
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
#include <QSlider>
#include <QMessageBox>
#include <QStringList>
#include <QColor>
#include <QColorDialog>
#include <QStatusBar>

class MainWidget : public QWidget
{
    Q_OBJECT

    GeneratorCV *generator;

    OperationsWidget *operationsWidget = nullptr;

    ImageIterationPlot *imageIterationPlot;
    ImageIterationPlot *pixelIterationPlot;
    HistogramPlot *histogramPlot;
    ScatterPlot *colorSpacePlot;
    CurvePlot *colorSpacePixelPlot;

    QStatusBar *statusBar;

    std::chrono::steady_clock::time_point timePoint;

    QTabWidget *mainTabWidget;

    QPushButton *pauseResumePushButton;

    QCheckBox *coloredSeedCheckBox;
    QCheckBox *bwSeedCheckBox;

    QLineEdit *timerIntervalLineEdit;

    QTimer *timer;
    int timerInterval;

    QLineEdit *imageSizeLineEdit;

    QPushButton *videoFilenamePushButton;
    QPushButton *videoCapturePushButton;
    QLineEdit *fpsLineEdit;
    QLabel *videoCaptureElapsedTimeLabel;

    QComboBox *newImageOperationComboBox;
    QPushButton *insertImageOperationPushButton;
    QPushButton *removeImageOperationPushButton;

    QGridLayout *pipelinesGridLayout;
    QLabel *pipelinesLabel;
    QLabel *blendFactorsLabel;
    QButtonGroup *pipelinesButtonGroup;
    std::vector<QPushButton*> pipelinePushButton;
    std::vector<QLineEdit*> pipelineBlendFactorLineEdit;
    QPushButton *equalizeBlendFactorsPushButton;

    QListWidget *imageOperationsListWidget;
    std::vector<int> currentImageOperationIndex;

    QSlider *selectedParameterSlider;
    CustomLineEdit *selectedParameterMinLineEdit;
    CustomLineEdit *selectedParameterMaxLineEdit;
    QDoubleValidator *selectedParameterMinValidator;
    QDoubleValidator *selectedParameterMaxValidator;
    QGroupBox *selectedParameterGroupBox;

    QVBoxLayout *parametersLayout;

    QPushButton *togglePlotsPushButton;
    QPushButton *dftPushButton;
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

    void resizeMainTabs(int index);

    void pauseResumeSystem(bool checked);

    void saveConfig();
    void loadConfig();

    void setTimerInterval();

    void setImageSize();

    void openVideoWriter();
    void onVideoCapturePushButtonClicked(bool checked);
    void setVideoCaptureElapsedTimeLabel();

    void initPipelineControls(int selectedPipelineIndex);

    void setPipelineBlendFactorLineEditText(int pipelineIndex);

    void initNewImageOperationComboBox();
    void initImageOperationsListWidget(int imageIndex);

    void onImageOperationsListWidgetCurrentRowChanged(int currentRow);
    void onRowsMoved(QModelIndex parent, int start, int end, QModelIndex destination, int row);
    void onDoubleParameterWidgetFocusIn(DoubleParameterWidget *widget);

    void insertImageOperation();
    void removeImageOperation();

    void applyImageOperations();
    void iterationLoop();

    void colorSpaceAxisChanged(int axisIndex, QComboBox *axisComboBox);

    void closeEvent(QCloseEvent *event);

    void about();

public:
    MainWidget(QWidget *parent = nullptr);
    ~MainWidget();
};
#endif // MAINWIDGET_H
