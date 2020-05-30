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

#include "mainwidget.h"

MainWidget::MainWidget(QWidget *parent): QWidget(parent)
{
    // The generator

    generator = new GeneratorCV();

    // Init variables

    timerInterval = 30; // ms
    batchSize = 1;

    // Plots

    imageIterationPlot = new ImageIterationPlot("Evolution of full image colors", 0.0, 1.0, this);
    pixelIterationPlot = new ImageIterationPlot("Evolution of single pixel colors", 0.0, 255.0, this);
    histogramPlot = new HistogramPlot("BGR Histogram", 0.0, 255.0, this);
    histogramPlot->setYMax(generator->getHistogramMax());
    colorSpacePlot = new ScatterPlot("Color-space plot", 0.0, 255.0, 0.0, 255.0, this);
    colorSpacePixelPlot = new CurvePlot("Color-space trajectory of single pixel", 0.0, 255.0, 0.0, 255.0, this);

    // Qt

    constructGeneralControls();
    constructImageManipulationControls();
    constructComputationControls();

    // Main tabs

    mainTabWidget = new QTabWidget;
    mainTabWidget->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    mainTabWidget->addTab(generalControlsWidget, "General");
    mainTabWidget->addTab(imageManipulationWidget, "Pipelines");
    mainTabWidget->addTab(computationWidget, "Plots");

    resizeMainTabs(0);

    connect(mainTabWidget, &QTabWidget::currentChanged, this, &MainWidget::resizeMainTabs);

    // Plot tabs

    plotsTabWidget = new QTabWidget;
    plotsTabWidget->setGeometry(0, 0, 800, 600);
    plotsTabWidget->addTab(histogramPlot->plot, "Histogram");
    plotsTabWidget->addTab(imageIterationPlot->plot, "Full image color intensity");
    plotsTabWidget->addTab(pixelIterationPlot->plot, "Single pixel color intensity");
    plotsTabWidget->addTab(colorSpacePlot->plot, "Full image color-space");
    plotsTabWidget->addTab(colorSpacePixelPlot->plot, "Single pixel color-space");
    plotsTabWidget->hide();

    // Status bar

    statusBar = new QStatusBar;
    statusBar->setFont(QFont("sans", 8));
    statusBar->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);
    statusBar->setSizeGripEnabled(false);
    statusBar->showMessage(QString("%1 Iterations | %2 ms / iteration | %3 ms / pipelines").arg(0).arg(0).arg(0));

    // Main layout

    QVBoxLayout *mainVBoxLayout = new QVBoxLayout;
    mainVBoxLayout->setAlignment(Qt::AlignTop);
    mainVBoxLayout->addWidget(mainTabWidget);
    mainVBoxLayout->addWidget(statusBar);

    setLayout(mainVBoxLayout);
    layout()->setSizeConstraint(QLayout::SetFixedSize);

    // Style

    QString startButtonStyle = "QPushButton#startButton{ color: #ffffff; background-color: #00ba00; }";
    QString pipelineButtonStyle = "QPushButton#pipelineButton{ color: #ffffff; background-color: #6600a6; } QPushButton:checked#pipelineButton { color: #000000; background-color: #fcff59; }";
    setStyleSheet(startButtonStyle + pipelineButtonStyle);

    // Timer for iteration loop

    timer = new QTimer(this);
    timer->setTimerType(Qt::PreciseTimer);

    connect(timer, &QTimer::timeout, this, &MainWidget::iterationLoop);
}

MainWidget::~MainWidget()
{
    delete timer;

    delete plotsTabWidget;

    delete histogramPlot;
    delete imageIterationPlot;
    delete pixelIterationPlot;
    delete colorSpacePlot;
    delete colorSpacePixelPlot;

    delete generator;
}

void MainWidget::constructGeneralControls()
{
    // Configurations

    QPushButton *loadConfigPushButton = new QPushButton("Load");
    loadConfigPushButton->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);

    QPushButton *saveConfigPushButton = new QPushButton("Save");
    saveConfigPushButton->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);

    QHBoxLayout *configHBoxLayout = new QHBoxLayout;
    configHBoxLayout->addWidget(loadConfigPushButton);
    configHBoxLayout->addWidget(saveConfigPushButton);

    QGroupBox *configGroupBox = new QGroupBox("Configurations");
    configGroupBox->setLayout(configHBoxLayout);

    // Main

    pauseResumePushButton = new QPushButton("Start iterating");
    pauseResumePushButton->setObjectName("startButton");
    pauseResumePushButton->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
    pauseResumePushButton->setCheckable(true);

    batchPushButton = new QPushButton("Start batch");
    batchPushButton->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
    batchPushButton->setCheckable(true);

    QHBoxLayout *startButtonsHBoxLayout = new QHBoxLayout;
    startButtonsHBoxLayout->setAlignment(Qt::AlignHCenter);
    startButtonsHBoxLayout->addWidget(pauseResumePushButton);
    startButtonsHBoxLayout->addWidget(batchPushButton);

    batchSizeLineEdit = new CustomLineEdit;
    batchSizeLineEdit->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
    QIntValidator *batchSizeValidator = new QIntValidator(1, 9999999, batchSizeLineEdit);
    batchSizeValidator->setLocale(QLocale::English);
    batchSizeLineEdit->setValidator(batchSizeValidator);
    batchSizeLineEdit->setText(QString::number(batchSize));

    timerIntervalLineEdit = new CustomLineEdit;
    timerIntervalLineEdit->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
    QIntValidator *timeIntervalIntValidator = new QIntValidator(1, 10000, timerIntervalLineEdit);
    timeIntervalIntValidator->setLocale(QLocale::English);
    timerIntervalLineEdit->setValidator(timeIntervalIntValidator);
    timerIntervalLineEdit->setText(QString::number(timerInterval));

    imageSizeLineEdit = new CustomLineEdit;
    imageSizeLineEdit->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
    QIntValidator *imageSizeIntValidator = new QIntValidator(0, 4096, imageSizeLineEdit);
    imageSizeIntValidator->setLocale(QLocale::English);
    imageSizeLineEdit->setValidator(imageSizeIntValidator);
    imageSizeLineEdit->setText(QString::number(generator->getImageSize()));

    QFormLayout *formLayout = new QFormLayout;
    formLayout->addRow("Batch size (its):", batchSizeLineEdit);
    formLayout->addRow("Time interval (ms):", timerIntervalLineEdit);
    formLayout->addRow("Image size (px):", imageSizeLineEdit);

    QVBoxLayout *mainControlsVBoxLayout = new QVBoxLayout;
    mainControlsVBoxLayout->addLayout(startButtonsHBoxLayout);
    mainControlsVBoxLayout->addLayout(formLayout);

    QGroupBox *mainControlsGroupBox = new QGroupBox("Main");
    mainControlsGroupBox->setLayout(mainControlsVBoxLayout);

    // Video capture

    videoFilenamePushButton = new QPushButton("Select output file");
    videoFilenamePushButton->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);

    videoCapturePushButton = new QPushButton("Start");
    videoCapturePushButton->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
    videoCapturePushButton->setCheckable(true);
    videoCapturePushButton->setEnabled(false);

    fpsLineEdit = new CustomLineEdit;
    fpsLineEdit->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
    QIntValidator *fpsValidator = new QIntValidator(1, 1000, fpsLineEdit);
    fpsValidator->setLocale(QLocale::English);
    fpsLineEdit->setValidator(fpsValidator);
    fpsLineEdit->setText(QString::number(generator->getFramesPerSecond()));

    videoCaptureElapsedTimeLabel = new QLabel("00:00:00.000");
    videoCaptureElapsedTimeLabel->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);

    QHBoxLayout *videoHBoxLayout = new QHBoxLayout;
    videoHBoxLayout->addWidget(videoFilenamePushButton);
    videoHBoxLayout->addWidget(videoCapturePushButton);

    QFormLayout *videoFormLayout = new QFormLayout;
    videoFormLayout->addRow("Frames per second:", fpsLineEdit);
    videoFormLayout->addRow("Elapsed time:", videoCaptureElapsedTimeLabel);

    QVBoxLayout *videoVBoxLayout = new QVBoxLayout;
    videoVBoxLayout->addLayout(videoHBoxLayout);
    videoVBoxLayout->addLayout(videoFormLayout);

    QGroupBox *videoGroupBox = new QGroupBox("Video capture");
    videoGroupBox->setLayout(videoVBoxLayout);

    // Pointer

    QPushButton *drawPointerPushButton = new QPushButton("Draw");
    drawPointerPushButton->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
    drawPointerPushButton->setCheckable(true);

    QPushButton *clearPointerCanvasPushButton = new QPushButton("Clear");
    clearPointerCanvasPushButton->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);

    QHBoxLayout *pointerButtonsHBoxLayout1 = new QHBoxLayout;
    pointerButtonsHBoxLayout1->addWidget(drawPointerPushButton);
    pointerButtonsHBoxLayout1->addWidget(clearPointerCanvasPushButton);

    CustomLineEdit *pointerRadiusLineEdit = new CustomLineEdit;
    pointerRadiusLineEdit->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
    QIntValidator *pointerRadiusValidator = new QIntValidator(1, generator->getImageSize() / 2, pointerRadiusLineEdit);
    pointerRadiusValidator->setLocale(QLocale::English);
    pointerRadiusLineEdit->setValidator(pointerRadiusValidator);
    pointerRadiusLineEdit->setText(QString::number(generator->getPointerRadius()));

    CustomLineEdit *pointerThicknessLineEdit = new CustomLineEdit;
    pointerThicknessLineEdit->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
    QIntValidator *pointerThicknessValidator = new QIntValidator(-1, 100, pointerRadiusLineEdit);
    pointerThicknessValidator->setLocale(QLocale::English);
    pointerThicknessLineEdit->setValidator(pointerThicknessValidator);
    pointerThicknessLineEdit->setText(QString::number(generator->getPointerThickness()));

    QFormLayout *pointerFormLayout = new QFormLayout;
    pointerFormLayout->addRow("Radius (px):", pointerRadiusLineEdit);
    pointerFormLayout->addRow("Thickness (px):", pointerThicknessLineEdit);

    QPushButton *pickPointerColorPushButton = new QPushButton("Pick color");
    pickPointerColorPushButton->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);

    QPushButton *drawCenteredPointerPushButton = new QPushButton("Center");
    drawCenteredPointerPushButton->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);

    QHBoxLayout *pointerButtonsHBoxLayout2 = new QHBoxLayout;
    pointerButtonsHBoxLayout2->addWidget(pickPointerColorPushButton);
    pointerButtonsHBoxLayout2->addWidget(drawCenteredPointerPushButton);

    QVBoxLayout *pointerVBoxLayout = new QVBoxLayout;
    pointerVBoxLayout->setAlignment(Qt::AlignHCenter);
    pointerVBoxLayout->addLayout(pointerButtonsHBoxLayout1);
    pointerVBoxLayout->addLayout(pointerFormLayout);
    pointerVBoxLayout->addLayout(pointerButtonsHBoxLayout2);

    QGroupBox *pointerGroupBox = new QGroupBox("Pointer");
    pointerGroupBox->setLayout(pointerVBoxLayout);

    // About

    QPushButton *aboutPushButton = new QPushButton("About");
    aboutPushButton->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);

    // Main layouts

    QVBoxLayout *vBoxLayout1 = new QVBoxLayout;
    vBoxLayout1->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
    vBoxLayout1->addWidget(configGroupBox);
    vBoxLayout1->addWidget(pointerGroupBox);

    QVBoxLayout *vBoxLayout2 = new QVBoxLayout;
    vBoxLayout2->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
    vBoxLayout2->addWidget(mainControlsGroupBox);
    vBoxLayout2->addWidget(videoGroupBox);

    QHBoxLayout *generalControlsHBoxLayout = new QHBoxLayout;
    generalControlsHBoxLayout->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
    generalControlsHBoxLayout->addLayout(vBoxLayout1);
    generalControlsHBoxLayout->addLayout(vBoxLayout2);

    // Widget to put in tab

    generalControlsWidget = new QWidget;
    generalControlsWidget->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    generalControlsWidget->setLayout(generalControlsHBoxLayout);

    // Signals + Slots

    connect(pauseResumePushButton, &QPushButton::clicked, this, &MainWidget::pauseResumeSystem);
    connect(batchPushButton, &QPushButton::clicked, this, &MainWidget::toggleBatch);
    connect(saveConfigPushButton, &QPushButton::clicked, this, &MainWidget::saveConfig);
    connect(loadConfigPushButton, &QPushButton::clicked, this, &MainWidget::loadConfig);
    connect(batchSizeLineEdit, &CustomLineEdit::returnPressed, [=](){ batchSize = batchSizeLineEdit->text().toInt(); });
    connect(batchSizeLineEdit, &CustomLineEdit::focusOut, [=](){ batchSizeLineEdit->setText(QString::number(batchSize)); });
    connect(timerIntervalLineEdit, &CustomLineEdit::returnPressed, this, &MainWidget::setTimerInterval);
    connect(timerIntervalLineEdit, &CustomLineEdit::focusOut, [=](){ timerIntervalLineEdit->setText(QString::number(timerInterval)); });
    connect(imageSizeLineEdit, &CustomLineEdit::returnPressed, this, &MainWidget::setImageSize);
    connect(imageSizeLineEdit, &CustomLineEdit::focusOut, [=](){ imageSizeLineEdit->setText(QString::number(generator->getImageSize())); });
    connect(videoFilenamePushButton, &QPushButton::clicked, this, &MainWidget::openVideoWriter);
    connect(videoCapturePushButton, &QPushButton::clicked, this, &MainWidget::onVideoCapturePushButtonClicked);
    connect(fpsLineEdit, &CustomLineEdit::returnPressed, [=](){ generator->setFramesPerSecond(fpsLineEdit->text().toInt()); });
    connect(fpsLineEdit, &CustomLineEdit::focusOut, [=](){ fpsLineEdit->setText(QString::number(generator->getFramesPerSecond())); });
    connect(drawPointerPushButton, &QPushButton::clicked, [=](bool checked){ generator->drawingPointer = checked; });
    connect(clearPointerCanvasPushButton, &QPushButton::clicked, [=](){ generator->clearPointerCanvas(); });
    connect(pickPointerColorPushButton, &QPushButton::clicked, [=]()
    {
        QColor color = QColorDialog::getColor(Qt::white, this, "Pick pointer color");
        generator->setPointerColor(color.red(), color.green(), color.blue());
    });
    connect(drawCenteredPointerPushButton, &QPushButton::clicked, [=](){ generator->drawCenteredPointer(); });
    connect(pointerRadiusLineEdit, &CustomLineEdit::returnPressed, [=](){ generator->setPointerRadius(pointerRadiusLineEdit->text().toInt()); });
    connect(pointerRadiusLineEdit, &CustomLineEdit::focusOut, [=](){ pointerRadiusLineEdit->setText(QString::number(generator->getPointerRadius())); });
    connect(pointerThicknessLineEdit, &CustomLineEdit::returnPressed, [=](){ generator->setPointerThickness(pointerThicknessLineEdit->text().toInt()); });
    connect(pointerThicknessLineEdit, &CustomLineEdit::focusOut, [=](){ pointerThicknessLineEdit->setText(QString::number(generator->getPointerThickness())); });
    connect(aboutPushButton, &QPushButton::clicked, this, &MainWidget::about);
}

void MainWidget::constructImageManipulationControls()
{
    // Seed

    QPushButton *drawRandomSeedPushButton = new QPushButton("Draw random");
    drawRandomSeedPushButton->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);

    coloredSeedCheckBox = new QCheckBox("Color");
    coloredSeedCheckBox->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
    coloredSeedCheckBox->setChecked(true);

    bwSeedCheckBox = new QCheckBox("Grays");
    bwSeedCheckBox->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
    bwSeedCheckBox->setChecked(false);

    QHBoxLayout *randomSeedHBoxLayout = new QHBoxLayout;
    randomSeedHBoxLayout->setAlignment(Qt::AlignLeft);
    randomSeedHBoxLayout->addWidget(drawRandomSeedPushButton);
    randomSeedHBoxLayout->addWidget(coloredSeedCheckBox);
    randomSeedHBoxLayout->addWidget(bwSeedCheckBox);

    QPushButton *drawSeedImagePushButton = new QPushButton("Draw image");
    drawSeedImagePushButton->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);

    QPushButton *loadSeedImagePushButton = new QPushButton("Load image");
    loadSeedImagePushButton->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);

    QHBoxLayout *seedImageHBoxLayout = new QHBoxLayout;
    seedImageHBoxLayout->setAlignment(Qt::AlignLeft);
    seedImageHBoxLayout->addWidget(drawSeedImagePushButton);
    seedImageHBoxLayout->addWidget(loadSeedImagePushButton);

    QVBoxLayout *seedVBoxLayout = new QVBoxLayout;
    seedVBoxLayout->addLayout(randomSeedHBoxLayout);
    seedVBoxLayout->addLayout(seedImageHBoxLayout);

    QGroupBox *seedGroupBox = new QGroupBox("Seed");
    seedGroupBox->setLayout(seedVBoxLayout);

    QButtonGroup *seedButtonGroup = new QButtonGroup(this);
    seedButtonGroup->setExclusive(true);
    seedButtonGroup->addButton(coloredSeedCheckBox, 0);
    seedButtonGroup->addButton(bwSeedCheckBox, 1);

    // Pipelines

    QPushButton *addPipelinePushButton = new QPushButton("Add new");
    addPipelinePushButton->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);

    QPushButton *removePipelinePushButton = new QPushButton("Remove selected");
    removePipelinePushButton->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);

    QHBoxLayout *pipelineButtonsHBoxLayout = new QHBoxLayout;
    pipelineButtonsHBoxLayout->addWidget(addPipelinePushButton);
    pipelineButtonsHBoxLayout->addWidget(removePipelinePushButton);

    pipelinesLabel = new QLabel("Pipelines");
    pipelinesLabel->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
    pipelinesLabel->hide();

    blendFactorsLabel = new QLabel("Blend factors");
    blendFactorsLabel->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
    blendFactorsLabel->hide();

    equalizeBlendFactorsPushButton = new QPushButton("Equalize blend factors");
    equalizeBlendFactorsPushButton->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
    equalizeBlendFactorsPushButton->hide();

    outputPipelinePushButton = new QPushButton("Output pipeline");
    outputPipelinePushButton->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    outputPipelinePushButton->setObjectName("pipelineButton");
    outputPipelinePushButton->setCheckable(true);
    outputPipelinePushButton->setChecked(true);

    pipelinesGridLayout = new QGridLayout;
    pipelinesGridLayout->setAlignment(Qt::AlignLeft);
    pipelinesGridLayout->addWidget(pipelinesLabel, 0, 0);
    pipelinesGridLayout->addWidget(blendFactorsLabel, 0, 1);
    pipelinesGridLayout->addWidget(outputPipelinePushButton, 1, 0);
    pipelinesGridLayout->addWidget(equalizeBlendFactorsPushButton, 1, 1);

    pipelinesButtonGroup = new QButtonGroup(this);
    pipelinesButtonGroup->setExclusive(true);
    pipelinesButtonGroup->addButton(outputPipelinePushButton);
    pipelinesButtonGroup->setId(outputPipelinePushButton, -2);

    QVBoxLayout *pipelineVBoxLayout = new QVBoxLayout;
    pipelineVBoxLayout->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
    pipelineVBoxLayout->addLayout(pipelineButtonsHBoxLayout);
    pipelineVBoxLayout->addLayout(pipelinesGridLayout);

    QGroupBox *pipelineGroupBox = new QGroupBox("Pipelines");
    pipelineGroupBox->setLayout(pipelineVBoxLayout);

    // Image operations

    newImageOperationComboBox = new QComboBox;
    newImageOperationComboBox->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);

    QFormLayout *newImageOperationFormLayout = new QFormLayout;
    newImageOperationFormLayout->addRow("New operation:", newImageOperationComboBox);

    insertImageOperationPushButton = new QPushButton("Insert");
    insertImageOperationPushButton->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);

    removeImageOperationPushButton = new QPushButton("Remove");
    removeImageOperationPushButton->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);

    QHBoxLayout *insertRemoveHBoxLayout = new QHBoxLayout;
    insertRemoveHBoxLayout->addWidget(insertImageOperationPushButton);
    insertRemoveHBoxLayout->addWidget(removeImageOperationPushButton);

    imageOperationsListWidget = new QListWidget;
    imageOperationsListWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    imageOperationsListWidget->setFixedHeight(100);
    imageOperationsListWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    imageOperationsListWidget->setDragDropMode(QAbstractItemView::InternalMove);

    // Parameters

    parametersLayout = new QVBoxLayout;

    QGroupBox *parametersGroupBox = new QGroupBox("Parameters");
    parametersGroupBox->setLayout(parametersLayout);

    // Selected real parameter

    selectedParameterSlider = new QSlider(Qt::Horizontal);
    selectedParameterSlider->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    selectedParameterSlider->setRange(0, 10000);

    selectedParameterMinLineEdit = new CustomLineEdit;
    selectedParameterMinLineEdit->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
    selectedParameterMinLineEdit->setPlaceholderText("Minimum");

    selectedParameterMinValidator = new QDoubleValidator(selectedParameterMinLineEdit);
    selectedParameterMinValidator->setDecimals(10);
    selectedParameterMinLineEdit->setValidator(selectedParameterMinValidator);

    selectedParameterMaxLineEdit = new CustomLineEdit;
    selectedParameterMaxLineEdit->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
    selectedParameterMaxLineEdit->setPlaceholderText("Maximum");

    selectedParameterMaxValidator = new QDoubleValidator(selectedParameterMaxLineEdit);
    selectedParameterMaxValidator->setDecimals(10);
    selectedParameterMaxLineEdit->setValidator(selectedParameterMaxValidator);

    QHBoxLayout *minMaxHBoxLayout = new QHBoxLayout;
    minMaxHBoxLayout->setAlignment(Qt::AlignJustify);
    minMaxHBoxLayout->addWidget(selectedParameterMinLineEdit);
    minMaxHBoxLayout->addWidget(selectedParameterMaxLineEdit);

    QVBoxLayout *selectedParameterVBoxLayout = new QVBoxLayout;
    selectedParameterVBoxLayout->addWidget(selectedParameterSlider);
    selectedParameterVBoxLayout->addLayout(minMaxHBoxLayout);

    selectedParameterGroupBox = new QGroupBox("No parameter selected");
    selectedParameterGroupBox->setLayout(selectedParameterVBoxLayout);

    // Image operations layout

    QVBoxLayout *imageOperationsVBoxLayout = new QVBoxLayout;
    imageOperationsVBoxLayout->setAlignment(Qt::AlignTop);
    imageOperationsVBoxLayout->addLayout(newImageOperationFormLayout);
    imageOperationsVBoxLayout->addLayout(insertRemoveHBoxLayout);
    imageOperationsVBoxLayout->addWidget(imageOperationsListWidget);
    imageOperationsVBoxLayout->addWidget(parametersGroupBox);
    imageOperationsVBoxLayout->addWidget(selectedParameterGroupBox);

    QGroupBox *imageOperationsGroupBox = new QGroupBox("Pipeline operations");
    imageOperationsGroupBox->setLayout(imageOperationsVBoxLayout);

    // Main layouts

    QVBoxLayout *vBoxLayout1 = new QVBoxLayout;
    vBoxLayout1->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
    vBoxLayout1->addWidget(seedGroupBox);
    vBoxLayout1->addWidget(pipelineGroupBox);

    QVBoxLayout *vBoxLayout2 = new QVBoxLayout;
    vBoxLayout2->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
    vBoxLayout2->addWidget(imageOperationsGroupBox);

    QHBoxLayout *imageManipulationHBoxLayout = new QHBoxLayout;
    imageManipulationHBoxLayout->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
    imageManipulationHBoxLayout->addLayout(vBoxLayout1);
    imageManipulationHBoxLayout->addLayout(vBoxLayout2);

    imageManipulationWidget = new QWidget;
    imageManipulationWidget->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    imageManipulationWidget->setLayout(imageManipulationHBoxLayout);

    // Signals + Slots

    connect(drawRandomSeedPushButton, &QPushButton::clicked, [=](){ generator->drawRandomSeed(bwSeedCheckBox->isChecked()); });
    connect(drawSeedImagePushButton, &QPushButton::clicked, [=](){ generator->drawSeedImage(); });
    connect(loadSeedImagePushButton, &QPushButton::clicked, [=]()
    {
        QString filename = QFileDialog::getOpenFileName(this, "Load image", "", "Images (*.bmp *.jpeg *.jpg *.png *.tiff *.tif)");
        if (!filename.isEmpty())
            generator->loadSeedImage(filename.toStdString());
    });
    connect(outputPipelinePushButton, &QPushButton::clicked, [=](bool checked){ if (checked) initImageOperationsListWidget(pipelinesButtonGroup->checkedId()); });
    connect(addPipelinePushButton, &QPushButton::clicked, [=]()
    {
        generator->addPipeline();
        initPipelineControls(generator->getPipelinesSize() - 1);
    });
    connect(removePipelinePushButton, &QPushButton::clicked, [=]()
    {
        generator->removePipeline(pipelinesButtonGroup->checkedId());
        initPipelineControls(pipelinesButtonGroup->checkedId());
    });
    connect(equalizeBlendFactorsPushButton, &QPushButton::clicked, [=]()
    {
        generator->equalizePipelineBlendFactors();
        for (int i = 0; i < generator->getPipelinesSize(); i++)
            pipelineBlendFactorLineEdit[i]->setText(QString::number(generator->getPipelineBlendFactor(i)));
    });
    connect(imageOperationsListWidget, &QListWidget::currentRowChanged, this, &MainWidget::onImageOperationsListWidgetCurrentRowChanged);
    connect(imageOperationsListWidget->model(), &QAbstractItemModel::rowsMoved, this, &MainWidget::onRowsMoved);
    connect(insertImageOperationPushButton, &QPushButton::clicked, this, &MainWidget::insertImageOperation);
    connect(removeImageOperationPushButton, &QPushButton::clicked, this, &MainWidget::removeImageOperation);

    // Init

    initNewImageOperationComboBox();
}

void MainWidget::constructComputationControls()
{
    togglePlotsPushButton = new QPushButton("Show plots");
    togglePlotsPushButton->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);

    // Full image controls

    histogramPushButton = new QPushButton("Start plotting");
    histogramPushButton->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
    histogramPushButton->setCheckable(true);

    dftPushButton = new QPushButton("Start plotting");
    dftPushButton->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
    dftPushButton->setCheckable(true);

    imageIterationPushButton = new QPushButton("Start plotting");
    imageIterationPushButton->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
    imageIterationPushButton->setCheckable(true);

    colorSpacePushButton = new QPushButton("Start plotting");
    colorSpacePushButton->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
    colorSpacePushButton->setCheckable(true);

    colorSpaceXAxisComboBox = new QComboBox;
    colorSpaceXAxisComboBox->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
    colorSpaceXAxisComboBox->addItem("Blue");
    colorSpaceXAxisComboBox->addItem("Green");
    colorSpaceXAxisComboBox->addItem("Red");

    colorSpaceYAxisComboBox = new QComboBox;
    colorSpaceYAxisComboBox->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
    colorSpaceYAxisComboBox->addItem("Blue");
    colorSpaceYAxisComboBox->addItem("Green");
    colorSpaceYAxisComboBox->addItem("Red");

    QFormLayout *fullImageLayout = new QFormLayout;
    fullImageLayout->addRow("Histogram:", histogramPushButton);
    fullImageLayout->addRow("DFT:", dftPushButton);
    fullImageLayout->addRow("Color intensity plot:", imageIterationPushButton);
    fullImageLayout->addRow("Color-space plot:", colorSpacePushButton);
    fullImageLayout->addRow("X-Axis:", colorSpaceXAxisComboBox);
    fullImageLayout->addRow("Y-Axis:", colorSpaceYAxisComboBox);

    QGroupBox *fullImageGroupBox = new QGroupBox("Full image");
    fullImageGroupBox->setLayout(fullImageLayout);

    // Pixel controls

    selectPixelPushButton = new QPushButton("Select pixel");
    selectPixelPushButton->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
    selectPixelPushButton->setCheckable(true);

    pixelIterationPushButton = new QPushButton("Start plotting");
    pixelIterationPushButton->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
    pixelIterationPushButton->setCheckable(true);

    colorSpacePixelPushButton = new QPushButton("Start plotting");
    colorSpacePixelPushButton->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
    colorSpacePixelPushButton->setCheckable(true);

    colorSpacePixelXAxisComboBox = new QComboBox;
    colorSpacePixelXAxisComboBox->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
    colorSpacePixelXAxisComboBox->addItem("Blue");
    colorSpacePixelXAxisComboBox->addItem("Green");
    colorSpacePixelXAxisComboBox->addItem("Red");

    colorSpacePixelYAxisComboBox = new QComboBox;
    colorSpacePixelYAxisComboBox->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
    colorSpacePixelYAxisComboBox->addItem("Blue");
    colorSpacePixelYAxisComboBox->addItem("Green");
    colorSpacePixelYAxisComboBox->addItem("Red");

    QFormLayout *pixelLayout = new QFormLayout;
    pixelLayout->addRow("Pixel selection:", selectPixelPushButton);
    pixelLayout->addRow("Color intensity plot:", pixelIterationPushButton);
    pixelLayout->addRow("Color-space plot:", colorSpacePixelPushButton);
    pixelLayout->addRow("X-Axis:", colorSpacePixelXAxisComboBox);
    pixelLayout->addRow("Y-Axis:", colorSpacePixelYAxisComboBox);

    QGroupBox *pixelGroupBox = new QGroupBox("Single pixel");
    pixelGroupBox->setLayout(pixelLayout);

    // Main

    QHBoxLayout *computationHBoxLayout = new QHBoxLayout;
    computationHBoxLayout->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
    computationHBoxLayout->addWidget(fullImageGroupBox);
    computationHBoxLayout->addWidget(pixelGroupBox);

    QVBoxLayout *computationVBoxLayout = new QVBoxLayout;
    computationVBoxLayout->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
    computationVBoxLayout->addLayout(computationHBoxLayout);
    computationVBoxLayout->addWidget(togglePlotsPushButton);

    computationWidget = new QWidget;
    computationWidget->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    computationWidget->setLayout(computationVBoxLayout);

    // Signals + Slots

    connect(togglePlotsPushButton, &QPushButton::clicked, [=](){ if (!plotsTabWidget->isVisible()) plotsTabWidget->show(); });
    connect(imageIterationPushButton, &QPushButton::clicked, [=](bool checked){ if (checked) imageIterationPlot->clearGraphsData(); });
    connect(pixelIterationPushButton, &QPushButton::clicked, [=](bool checked){ if (checked) pixelIterationPlot->clearGraphsData(); });
    connect(colorSpacePixelPushButton, &QPushButton::clicked, [=](bool checked){ if (checked) colorSpacePixelPlot->clearCurveData(); });
    connect(selectPixelPushButton, &QPushButton::clicked, [=](bool checked){ generator->selectingPixel = checked; });
    connect(colorSpaceXAxisComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), [=](int axisIndex)
    {
        colorSpaceAxisChanged(axisIndex, colorSpaceYAxisComboBox);
        colorSpacePlot->setAxesLabels(colorSpaceXAxisComboBox->currentText(), colorSpaceYAxisComboBox->currentText());
    });
    connect(colorSpaceYAxisComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), [=](int axisIndex)
    {
        colorSpaceAxisChanged(axisIndex, colorSpaceXAxisComboBox);
        colorSpacePlot->setAxesLabels(colorSpaceXAxisComboBox->currentText(), colorSpaceYAxisComboBox->currentText());
    });
    connect(colorSpacePixelXAxisComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), [=](int axisIndex)
    {
        colorSpaceAxisChanged(axisIndex, colorSpacePixelYAxisComboBox);
        colorSpacePixelPlot->setAxesLabels(colorSpacePixelXAxisComboBox->currentText(), colorSpacePixelYAxisComboBox->currentText());
        colorSpacePixelPlot->clearCurveData();
    });
    connect(colorSpacePixelYAxisComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), [=](int axisIndex)
    {
        colorSpaceAxisChanged(axisIndex, colorSpacePixelXAxisComboBox);
        colorSpacePixelPlot->setAxesLabels(colorSpacePixelXAxisComboBox->currentText(), colorSpacePixelYAxisComboBox->currentText());
        colorSpacePixelPlot->clearCurveData();
    });

    // Init axes

    colorSpaceXAxisComboBox->setCurrentIndex(0);
    colorSpaceYAxisComboBox->setCurrentIndex(1);

    colorSpacePixelXAxisComboBox->setCurrentIndex(0);
    colorSpacePixelYAxisComboBox->setCurrentIndex(1);
}

void MainWidget::resizeMainTabs(int index)
{
    for (int i = 0; i < mainTabWidget->count(); i++)
        if (i != index)
            mainTabWidget->widget(i)->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);

    mainTabWidget->widget(index)->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    mainTabWidget->widget(index)->resize(mainTabWidget->widget(index)->minimumSizeHint());
    mainTabWidget->widget(index)->adjustSize();
}

void MainWidget::pauseResumeSystem(bool checked)
{
    if (!checked)
    {
        timer->stop();
        pauseResumePushButton->setText("Resume");
        batchPushButton->setEnabled(true);
    }
    else
    {
        timePoint = std::chrono::steady_clock::now();
        timer->start(timerInterval);
        pauseResumePushButton->setText("Pause");
        batchPushButton->setEnabled(false);
    }
}

void MainWidget::toggleBatch(bool checked)
{
    if (!checked)
    {
        timer->stop();
        batchPushButton->setText("Start batch");
        pauseResumePushButton->setEnabled(true);
    }
    else
    {
        timePoint = std::chrono::steady_clock::now();
        timer->start(timerInterval);
        batchNumber = 0;
        batchPushButton->setText("Stop batch");
        pauseResumePushButton->setEnabled(false);
    }
}

void MainWidget::saveConfig()
{
    QString filename = QFileDialog::getSaveFileName(this, "Save configuration", "", "MorphogenCV configurations (*.morph)");

    if (!filename.isEmpty())
    {
        ConfigurationParser parser(generator, filename);
        parser.write();
    }
}

void MainWidget::loadConfig()
{
    QString filename = QFileDialog::getOpenFileName(this, "Load configuration", "", "MorphogenCV configurations (*.morph)");

    if (!filename.isEmpty())
    {
        ConfigurationParser parser(generator, filename);
        parser.read();

        for (int i = 0; i < generator->getPipelinesSize(); i++)
            currentImageOperationIndex[i] = 0;

        initPipelineControls(0);

        generator->resetIterationNumer();

        if (!pauseResumePushButton->isChecked())
            pauseResumePushButton->setText("Start iterating");

        statusBar->showMessage(QString("%1 Iterations | %2 ms / iteration | %3 ms / pipelines").arg(0).arg(0).arg(0));

        imageIterationPlot->clearGraphsData();
        pixelIterationPlot->clearGraphsData();
        colorSpacePixelPlot->clearCurveData();
    }
}

void MainWidget::setTimerInterval()
{
    timerInterval = timerIntervalLineEdit->text().toInt();
    timer->setInterval(timerInterval);
}

void MainWidget::setImageSize()
{
    generator->setImageSize(imageSizeLineEdit->text().toInt());
    histogramPlot->setYMax(generator->getHistogramMax());
}

void MainWidget::openVideoWriter()
{
    QString videoPath = QFileDialog::getSaveFileName(this, "Output video file", "", "Videos (*.avi)");

    if (!videoPath.isEmpty())
    {
        generator->openVideoWriter(videoPath.toStdString());
        fpsLineEdit->setEnabled(false);
        videoCaptureElapsedTimeLabel->setText("00:00:00.000");
    }

    videoCapturePushButton->setEnabled(!videoPath.isEmpty());
}

void MainWidget::onVideoCapturePushButtonClicked(bool checked)
{
    if (checked)
    {
        imageSizeLineEdit->setEnabled(false);
        videoFilenamePushButton->setEnabled(false);
        videoCapturePushButton->setText("Stop");
    }
    else
    {
        generator->closeVideoWriter();
        imageSizeLineEdit->setEnabled(true);
        videoFilenamePushButton->setEnabled(true);
        videoCapturePushButton->setText("Start");
        videoCapturePushButton->setEnabled(false);
        fpsLineEdit->setEnabled(true);
    }
}

void MainWidget::setVideoCaptureElapsedTimeLabel()
{
    int milliseconds = static_cast<int>(1000.0 * generator->getFrameCount() / generator->getFramesPerSecond());

    QTime start(0, 0, 0, 0);

    videoCaptureElapsedTimeLabel->setText(start.addMSecs(milliseconds).toString("hh:mm:ss.zzz"));
}

void MainWidget::initPipelineControls(int selectedPipelineIndex)
{
    // Remove all pipeline controls

    for (int pipelineIndex = pipelinePushButton.size() - 1; pipelineIndex >= 0; pipelineIndex--)
    {
        pipelinesButtonGroup->removeButton(pipelinePushButton[pipelineIndex]);
        pipelinesGridLayout->removeWidget(pipelinePushButton[pipelineIndex]);
        pipelinesGridLayout->removeWidget(pipelineBlendFactorLineEdit[pipelineIndex]);

        delete pipelinePushButton[pipelineIndex];
        delete pipelineBlendFactorLineEdit[pipelineIndex];

        pipelinePushButton.erase(pipelinePushButton.begin() + pipelineIndex);
        pipelineBlendFactorLineEdit.erase(pipelineBlendFactorLineEdit.begin() + pipelineIndex);
    }

    // Keep index within range

    if (selectedPipelineIndex < -2)
        selectedPipelineIndex = -2;

    if (selectedPipelineIndex > generator->getPipelinesSize() - 1)
        selectedPipelineIndex = generator->getPipelinesSize() - 1;

    // Add all pipeline controls

    for (int pipelineIndex = 0; pipelineIndex < generator->getPipelinesSize(); pipelineIndex++)
    {
        // New pipeline select button

        QPushButton *pushButton = new QPushButton(QString("Pipeline %1").arg(pipelineIndex + 1));
        pushButton->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
        pushButton->setObjectName("pipelineButton");
        pushButton->setCheckable(true);
        pushButton->setChecked(pipelineIndex == selectedPipelineIndex);

        pipelinesButtonGroup->addButton(pushButton);
        pipelinesButtonGroup->setId(pushButton, pipelineIndex);

        connect(pushButton, &QPushButton::clicked, [=](bool checked)
        {
            if (checked)
                initImageOperationsListWidget(pipelinesButtonGroup->checkedId());
        });

        pipelinePushButton.push_back(pushButton);

        // New pipeline blend factor line edit

        CustomLineEdit *blendFactorLineEdit = new CustomLineEdit;
        blendFactorLineEdit->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
        QDoubleValidator *blendFactorValidator = new QDoubleValidator(0.0, 1.0, 10, blendFactorLineEdit);
        blendFactorValidator->setLocale(QLocale::English);
        blendFactorLineEdit->setValidator(blendFactorValidator);
        blendFactorLineEdit->setText(QString::number(generator->getPipelineBlendFactor(pipelineIndex)));

        connect(blendFactorLineEdit, &CustomLineEdit::returnPressed, [=](){ setPipelineBlendFactorLineEditText(pipelineIndex); });
        connect(blendFactorLineEdit, &CustomLineEdit::focusOut, [=](){ blendFactorLineEdit->setText(QString::number(generator->getPipelineBlendFactor(pipelineIndex))); });

        pipelineBlendFactorLineEdit.push_back(blendFactorLineEdit);

        // Add controls to pipelines layout

        pipelinesGridLayout->addWidget(pushButton, pipelineIndex + 1, 0);
        pipelinesGridLayout->addWidget(blendFactorLineEdit, pipelineIndex + 1, 1);
    }

    pipelinesGridLayout->removeWidget(equalizeBlendFactorsPushButton);
    pipelinesGridLayout->removeWidget(outputPipelinePushButton);
    pipelinesGridLayout->addWidget(outputPipelinePushButton, generator->getPipelinesSize() + 1, 0);
    pipelinesGridLayout->addWidget(equalizeBlendFactorsPushButton, generator->getPipelinesSize() + 1, 1);

    // Set visibility of labels and equalize blend factors button

    pipelinesLabel->setVisible(generator->getPipelinesSize() > 0);
    blendFactorsLabel->setVisible(generator->getPipelinesSize() > 0);
    equalizeBlendFactorsPushButton->setVisible(generator->getPipelinesSize() > 1);

    // Check output pipeline button

    if (selectedPipelineIndex == -2 || generator->getPipelinesSize() == 0)
        outputPipelinePushButton->setChecked(true);
    else
        outputPipelinePushButton->setChecked(false);

    // Init operations

    if (generator->getPipelinesSize() > 0)
        initImageOperationsListWidget(selectedPipelineIndex);
    else
        initImageOperationsListWidget(0);
}

void MainWidget::setPipelineBlendFactorLineEditText(int pipelineIndex)
{
    generator->setPipelineBlendFactor(pipelineIndex, pipelineBlendFactorLineEdit[pipelineIndex]->text().toDouble());

    for (int i = 0; i < generator->getPipelinesSize(); i++)
        pipelineBlendFactorLineEdit[i]->setText(QString::number(generator->getPipelineBlendFactor(i)));

}

void MainWidget::initNewImageOperationComboBox()
{
    for (auto op: generator->availableImageOperations)
    {
        newImageOperationComboBox->addItem(QString::fromStdString(op));
    }
}

void MainWidget::initImageOperationsListWidget(int pipelineIndex)
{
    imageOperationsListWidget->clear();

    for (int i = 0; i < generator->getImageOperationsSize(pipelineIndex); i++)
    {
        QListWidgetItem *newOperation = new QListWidgetItem;
        newOperation->setText(QString::fromStdString(generator->getImageOperationName(pipelineIndex, i)));
        imageOperationsListWidget->insertItem(i, newOperation);
    }

    if (generator->getImageOperationsSize(pipelineIndex) == 0)
    {
        currentImageOperationIndex[pipelineIndex] = -1;

        if (!parametersLayout->isEmpty())
        {
            QWidget *widget = parametersLayout->itemAt(0)->widget();
            parametersLayout->removeWidget(widget);
            widget->hide();
        }

        if (operationsWidget)
        {
            delete operationsWidget;
            operationsWidget = nullptr;
        }
    }
    else
    {
        QListWidgetItem *operation = imageOperationsListWidget->item(currentImageOperationIndex[pipelineIndex]);
        imageOperationsListWidget->setCurrentItem(operation);

        int size = imageOperationsListWidget->sizeHintForRow(0);
        if (size > 0) rowSize = size;

        imageOperationsListWidget->setFixedHeight(rowSize * 5 + 2 * imageOperationsListWidget->frameWidth());
    }
}

void MainWidget::onImageOperationsListWidgetCurrentRowChanged(int currentRow)
{
    if (!parametersLayout->isEmpty())
    {
        QWidget *widget = parametersLayout->itemAt(0)->widget();
        parametersLayout->removeWidget(widget);
        widget->hide();
    }

    selectedParameterSlider->disconnect();
    selectedParameterSlider->setValue(0);
    selectedParameterGroupBox->setTitle("No parameter selected");

    selectedParameterMinLineEdit->disconnect();
    selectedParameterMinLineEdit->clear();

    selectedParameterMaxLineEdit->disconnect();
    selectedParameterMaxLineEdit->clear();

    if (currentRow >= 0) // currentRow = -1 if QListWidget empty
    {
        int pipelineIndex = pipelinesButtonGroup->checkedId();

        if (operationsWidget)
        {
            delete operationsWidget;
            operationsWidget = nullptr;
        }

        operationsWidget = new OperationsWidget(pipelineIndex >= 0 ? generator->pipelines[pipelineIndex]->imageOperations[currentRow] : generator->outputPipeline->imageOperations[currentRow]);
        parametersLayout->addWidget(operationsWidget);
        operationsWidget->show();

        for (auto widget: operationsWidget->doubleParameterWidget)
            connect(widget, &DoubleParameterWidget::focusIn, [=](){ onDoubleParameterWidgetFocusIn(widget); });

        currentImageOperationIndex[pipelineIndex] = currentRow;
    }
}

void MainWidget::onDoubleParameterWidgetFocusIn(DoubleParameterWidget *widget)
{
    // Slider

    selectedParameterSlider->disconnect();
    selectedParameterSlider->setRange(0, widget->indexMax);
    selectedParameterSlider->setValue(widget->getIndex());

    connect(selectedParameterSlider, &QAbstractSlider::valueChanged, widget, &DoubleParameterWidget::setValue);

    connect(widget, &DoubleParameterWidget::currentIndexChanged, [=](int currentIndex)
    {
        disconnect(selectedParameterSlider, &QAbstractSlider::valueChanged, nullptr, nullptr);
        selectedParameterSlider->setValue(currentIndex);
        connect(selectedParameterSlider, &QAbstractSlider::valueChanged, widget, &DoubleParameterWidget::setValue);
    });

    // Value changed: check if within min/max range and adjust controls

    connect(widget, &DoubleParameterWidget::currentValueChanged, [=](double currentValue)
    {
        if (currentValue < widget->getMin())
        {
            widget->setMin(currentValue);

            selectedParameterMinLineEdit->setText(QString::number(currentValue));

            disconnect(selectedParameterSlider, &QAbstractSlider::valueChanged, nullptr, nullptr);
            selectedParameterSlider->setValue(widget->getIndex());
            connect(selectedParameterSlider, &QAbstractSlider::valueChanged, widget, &DoubleParameterWidget::setValue);
        }
        else if (currentValue > widget->getMax())
        {
            widget->setMax(currentValue);

            selectedParameterMaxLineEdit->setText(QString::number(currentValue));

            disconnect(selectedParameterSlider, &QAbstractSlider::valueChanged, nullptr, nullptr);
            selectedParameterSlider->setValue(widget->getIndex());
            connect(selectedParameterSlider, &QAbstractSlider::valueChanged, widget, &DoubleParameterWidget::setValue);
        }
    });

    // Minimum

    selectedParameterMinLineEdit->disconnect();
    selectedParameterMinLineEdit->setText(QString::number(widget->getMin()));

    connect(selectedParameterMinLineEdit, &CustomLineEdit::returnPressed, [=]()
    {
        widget->setMin(selectedParameterMinLineEdit->text().toDouble());
        widget->setIndex();
    });
    connect(selectedParameterMinLineEdit, &CustomLineEdit::focusOut, [=](){ selectedParameterMinLineEdit->setText(QString::number(widget->getMin())); });

    // Maximum

    selectedParameterMaxLineEdit->disconnect();
    selectedParameterMaxLineEdit->setText(QString::number(widget->getMax()));

    connect(selectedParameterMaxLineEdit, &CustomLineEdit::returnPressed, [=]()
    {
        widget->setMax(selectedParameterMaxLineEdit->text().toDouble());
        widget->setIndex();
    });
    connect(selectedParameterMaxLineEdit, &CustomLineEdit::focusOut, [=](){ selectedParameterMaxLineEdit->setText(QString::number(widget->getMax())); });

    // Validators

    selectedParameterMinValidator->setBottom(widget->getInf());
    selectedParameterMinValidator->setTop(widget->getMax());

    selectedParameterMaxValidator->setBottom(widget->getMin());
    selectedParameterMaxValidator->setTop(widget->getSup());

    // Title

    selectedParameterGroupBox->setTitle("Selected parameter: " + widget->getName());
}

void MainWidget::onRowsMoved(QModelIndex parent, int start, int end, QModelIndex destination, int row)
{
    Q_UNUSED(parent)
    Q_UNUSED(destination)
    Q_UNUSED(end)

    int pipelineIndex = pipelinesButtonGroup->checkedId();

    generator->swapImageOperations(pipelineIndex, start, row);

    currentImageOperationIndex[pipelineIndex] = row;
}

void MainWidget::insertImageOperation()
{
    int pipelineIndex = pipelinesButtonGroup->checkedId();
    int newOperationIndex = newImageOperationComboBox->currentIndex();
    int currentOperationIndex = imageOperationsListWidget->currentRow();

    generator->insertImageOperation(pipelineIndex, newOperationIndex, currentOperationIndex);

    QListWidgetItem *newOperation = new QListWidgetItem;
    newOperation->setText(QString::fromStdString(generator->getImageOperationName(pipelineIndex, currentOperationIndex + 1)));
    imageOperationsListWidget->insertItem(currentOperationIndex + 1, newOperation);

    int size = imageOperationsListWidget->sizeHintForRow(0);
    if (size > 0) rowSize = size;

    imageOperationsListWidget->setFixedHeight(rowSize * 5 + 2 * imageOperationsListWidget->frameWidth());

    imageOperationsListWidget->setCurrentItem(newOperation);
}

void MainWidget::removeImageOperation()
{
    int pipelineIndex = pipelinesButtonGroup->checkedId();
    int operationIndex = imageOperationsListWidget->currentRow();

    imageOperationsListWidget->takeItem(operationIndex);

    int size = imageOperationsListWidget->sizeHintForRow(0);
    if (size > 0) rowSize = size;

    imageOperationsListWidget->setFixedHeight(rowSize * 5 + 2 * imageOperationsListWidget->frameWidth());

    generator->removeImageOperation(pipelineIndex, operationIndex);

    currentImageOperationIndex[pipelineIndex] = imageOperationsListWidget->currentRow();
}

void MainWidget::iterationLoop()
{
    if (pauseResumePushButton->isChecked() || (batchPushButton->isChecked() && batchNumber < batchSize))
    {
        auto start = std::chrono::steady_clock::now();
        generator->iterate();
        auto end = std::chrono::steady_clock::now();

        // Full image computations/plots

        if (colorSpacePushButton->isChecked())
        {
            int xAxisIndex = colorSpaceXAxisComboBox->currentIndex();
            int yAxisIndex = colorSpaceYAxisComboBox->currentIndex();

            colorSpacePlot->setData(generator->getColorComponents(xAxisIndex), generator->getColorComponents(yAxisIndex));
        }

        if (dftPushButton->isChecked())
            generator->computeDFT();

        if (histogramPushButton->isChecked())
        {
            generator->computeHistogram();
            histogramPlot->setData(generator->getHistogramBins(), generator->getBlueHistogram(), generator->getGreenHistogram(), generator->getRedHistogram());
        }

        if (imageIterationPushButton->isChecked())
        {
            generator->computeBGRSum();
            imageIterationPlot->addPoint(generator->getIterationNumber(), generator->getBSum(), generator->getGSum(), generator->getRSum());
        }

        // Single pixel computations/plots

        if (pixelIterationPushButton->isChecked() || colorSpacePixelPushButton->isChecked())
            generator->computeBGRPixel();

        if (pixelIterationPushButton->isChecked())
            pixelIterationPlot->addPoint(generator->getIterationNumber(), generator->getPixelComponent(0), generator->getPixelComponent(1), generator->getPixelComponent(2));

        if (colorSpacePixelPushButton->isChecked())
        {
            int xAxisIndex = colorSpacePixelXAxisComboBox->currentIndex();
            int yAxisIndex = colorSpacePixelYAxisComboBox->currentIndex();

            colorSpacePixelPlot->addPoint(generator->getPixelComponent(xAxisIndex), generator->getPixelComponent(yAxisIndex));
        }

        // Show out image

        if (selectPixelPushButton->isChecked())
            generator->showPixelSelectionCursor();
        else
            generator->showImage();

        if (videoCapturePushButton->isChecked())
        {
            generator->writeVideoFrame();
            setVideoCaptureElapsedTimeLabel();
        }

        // Batch

        if (batchPushButton->isChecked())
        {
            batchNumber++;

            if (batchNumber == batchSize)
                batchPushButton->click();
        }

        // Timing

        auto now = std::chrono::steady_clock::now();
        auto iterationTime = std::chrono::duration_cast<std::chrono::milliseconds>(now - timePoint).count();
        timePoint = now;

        auto pipelineTime = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

        // Status bar

        statusBar->clearMessage();
        statusBar->showMessage(QString("%1 Iterations | %2 ms / iteration | %3 ms / pipelines").arg(generator->getIterationNumber()).arg(iterationTime).arg(pipelineTime));
    }
}

void MainWidget::colorSpaceAxisChanged(int axisIndex, QComboBox *axisComboBox)
{
    QStandardItemModel *model = qobject_cast<QStandardItemModel*>(axisComboBox->model());

    for (int i = 0; i < axisComboBox->count(); i++)
    {
        QStandardItem *item = model->item(i);

        if (i == axisIndex)
        {
            item->setFlags(item->flags() & ~Qt::ItemIsEnabled);
        }
        else
        {
            item->setFlags(item->flags() | Qt::ItemIsEnabled);
        }
    }
}

void MainWidget::closeEvent(QCloseEvent *event)
{
    timer->stop();
    plotsTabWidget->close();
    generator->destroyAllWindows();
    event->accept();
}

void MainWidget::about()
{
    QMessageBox *aboutBox = new QMessageBox(this);

    aboutBox->setTextFormat(Qt::RichText);

    aboutBox->setWindowTitle("About");

    QStringList lines;
    lines.append("<h2>MorphogenCV 1.0</h2>");
    lines.append("<h4>Videofeedback simulation software.</h4>");
    lines.append("Looking for help? Please visit:<br>");
    lines.append("<a href='https://github.com/jmcastelo/MorphogenCV'>https://github.com/jmcastelo/MorphogenCV</a><br><br>");
    lines.append("Let the pixels come alive!");

    QString text = lines.join("");

    aboutBox->setText(text);

    aboutBox->setInformativeText("Copyright 2020 José María Castelo Ares\njose.maria.castelo@gmail.com\nLicense: GPLv3");

    aboutBox->exec();
}
