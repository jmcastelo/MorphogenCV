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

    screenshotPath = "";
    screenshotFilename = "screenshot";
    takeScreenshotSeries = false;
    screenshotIndex = 1;

    currentImageOperationIndex = {0, 0};

    // Plots

    imageIterationPlot = new ImageIterationPlot("Evolution of full image colors", 0.0, 1.0);
    pixelIterationPlot = new ImageIterationPlot("Evolution of single pixel colors", 0.0, 255.0);
    histogramPlot = new HistogramPlot("BGR Histogram", 0.0, 255.0);
    histogramPlot->setYMax(generator->getHistogramMax());
    colorSpacePlot = new ScatterPlot("Color-space plot", 0.0, 255.0, 0.0, 255.0);
    colorSpacePixelPlot = new CurvePlot("Color-space trajectory of single pixel", 0.0, 255.0, 0.0, 255.0);

    // Qt

    constructGeneralControls();
    constructImageManipulationControls();
    constructComputationControls();

    // Main tabs

    QTabWidget *mainTabWidget = new QTabWidget;
    mainTabWidget->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
    mainTabWidget->addTab(generalControlsWidget, "General");
    mainTabWidget->addTab(imageManipulationWidget, "Pipelines");
    mainTabWidget->addTab(computationWidget, "Plots");

    // Plot tabs

    plotsTabWidget = new QTabWidget;
    plotsTabWidget->addTab(histogramPlot->plot, "Histogram");
    plotsTabWidget->addTab(imageIterationPlot->plot, "Full image color intensity");
    plotsTabWidget->addTab(pixelIterationPlot->plot, "Single pixel color intensity");
    plotsTabWidget->addTab(colorSpacePlot->plot, "Full image color-space");
    plotsTabWidget->addTab(colorSpacePixelPlot->plot, "Single pixel color-space");
    plotsTabWidget->hide();

    // Main layout

    QHBoxLayout *mainHBoxLayout = new QHBoxLayout;

    mainHBoxLayout->addWidget(mainTabWidget);
    mainHBoxLayout->addWidget(plotsTabWidget);

    setLayout(mainHBoxLayout);
    setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);

    // Timer for iteration loop

    timer = new QTimer(this);
    timer->setTimerType(Qt::PreciseTimer);

    connect(timer, &QTimer::timeout, this, &MainWidget::iterationLoop);
}

MainWidget::~MainWidget()
{
    delete histogramPlot;
    delete imageIterationPlot;
    delete pixelIterationPlot;
    delete colorSpacePlot;
    delete colorSpacePixelPlot;
}

void MainWidget::constructGeneralControls()
{
    pauseResumePushButton = new QPushButton("Start");
    pauseResumePushButton->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
    pauseResumePushButton->setCheckable(true);

    QPushButton *loadConfigPushButton = new QPushButton("Load configuration");
    loadConfigPushButton->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);

    QPushButton *saveConfigPushButton = new QPushButton("Save configuration");
    saveConfigPushButton->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);

    QHBoxLayout *buttonsHBoxLayout = new QHBoxLayout;
    buttonsHBoxLayout->setAlignment(Qt::AlignHCenter);
    buttonsHBoxLayout->addWidget(loadConfigPushButton);
    buttonsHBoxLayout->addWidget(saveConfigPushButton);

    QPushButton *drawSeedPushButton = new QPushButton("Draw seed");
    drawSeedPushButton->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);

    coloredSeedCheckBox = new QCheckBox("Color");
    coloredSeedCheckBox->setChecked(false);

    bwSeedCheckBox = new QCheckBox("Grays");
    bwSeedCheckBox->setChecked(true);

    QHBoxLayout *seedHBoxLayout = new QHBoxLayout;
    seedHBoxLayout->setAlignment(Qt::AlignLeft);
    seedHBoxLayout->addWidget(drawSeedPushButton);
    seedHBoxLayout->addWidget(coloredSeedCheckBox);
    seedHBoxLayout->addWidget(bwSeedCheckBox);

    QButtonGroup *seedButtonGroup = new QButtonGroup;
    seedButtonGroup->setExclusive(true);
    seedButtonGroup->addButton(coloredSeedCheckBox, 0);
    seedButtonGroup->addButton(bwSeedCheckBox, 1);

    timerIntervalLineEdit = new QLineEdit;
    timerIntervalLineEdit->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
    QIntValidator *timeIntervalIntValidator = new QIntValidator(1, 10000, timerIntervalLineEdit);
    timeIntervalIntValidator->setLocale(QLocale::English);
    timerIntervalLineEdit->setValidator(timeIntervalIntValidator);
    timerIntervalLineEdit->setText(QString::number(timerInterval));

    imageSizeLineEdit = new QLineEdit;
    imageSizeLineEdit->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
    QIntValidator *imageSizeIntValidator = new QIntValidator(0, 4096, imageSizeLineEdit);
    imageSizeIntValidator->setLocale(QLocale::English);
    imageSizeLineEdit->setValidator(imageSizeIntValidator);
    imageSizeLineEdit->setText(QString::number(generator->getImageSize()));

    QFormLayout *formLayout = new QFormLayout;
    formLayout->addRow("Time interval (ms):", timerIntervalLineEdit);
    formLayout->addRow("Image size (px):", imageSizeLineEdit);

    // Screenshots controls

    screenshotPushButton = new QPushButton("Take screenshot");
    screenshotPushButton->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
    screenshotPushButton->setEnabled(false);
    screenshotPushButton->setCheckable(false);

    screenshotSeriesCheckBox = new QCheckBox("Series");

    selectScreenshotPathPushButton = new QPushButton("Select directory");
    selectScreenshotPathPushButton->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);

    QLabel *screenshotFilenameLabel = new QLabel("Filename (no extension)");

    screenshotFilenameLineEdit = new QLineEdit;
    screenshotFilenameLineEdit->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
    screenshotFilenameLineEdit->setText(screenshotFilename);

    QVBoxLayout *screenshotVBoxLayout = new QVBoxLayout;
    screenshotVBoxLayout->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    screenshotVBoxLayout->addWidget(screenshotPushButton);
    screenshotVBoxLayout->addWidget(screenshotSeriesCheckBox);
    screenshotVBoxLayout->addWidget(selectScreenshotPathPushButton);
    screenshotVBoxLayout->addWidget(screenshotFilenameLabel);
    screenshotVBoxLayout->addWidget(screenshotFilenameLineEdit);

    QGroupBox *screenshotGroupBox = new QGroupBox("Screenshots");
    screenshotGroupBox->setLayout(screenshotVBoxLayout);

    // Main layout

    QVBoxLayout *generalControlsVBoxLayout = new QVBoxLayout;
    generalControlsVBoxLayout->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
    generalControlsVBoxLayout->addWidget(pauseResumePushButton);
    generalControlsVBoxLayout->addLayout(buttonsHBoxLayout);
    generalControlsVBoxLayout->addLayout(seedHBoxLayout);
    generalControlsVBoxLayout->addLayout(formLayout);
    generalControlsVBoxLayout->addWidget(screenshotGroupBox);

    // Widget to put in tab

    generalControlsWidget = new QWidget;
    generalControlsWidget->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
    generalControlsWidget->setLayout(generalControlsVBoxLayout);

    // Signals + Slots

    connect(drawSeedPushButton, &QPushButton::clicked, [=](){ generator->drawSeed(bwSeedCheckBox->isChecked()); });
    connect(pauseResumePushButton, &QPushButton::clicked, this, &MainWidget::pauseResumeSystem);
    connect(saveConfigPushButton, &QPushButton::clicked, this, &MainWidget::saveConfig);
    connect(loadConfigPushButton, &QPushButton::clicked, this, &MainWidget::loadConfig);
    connect(timerIntervalLineEdit, &QLineEdit::returnPressed, this, &MainWidget::setTimerInterval);
    connect(imageSizeLineEdit, &QLineEdit::returnPressed, this, &MainWidget::setImageSize);
    connect(selectScreenshotPathPushButton, &QPushButton::clicked, this, &MainWidget::selectScreenshotPath);
    connect(screenshotFilenameLineEdit, &QLineEdit::textChanged, this, &MainWidget::setScreenshotFilename);
    connect(screenshotSeriesCheckBox, &QCheckBox::stateChanged, this, &MainWidget::setTakeScreenshotSeries);
    connect(screenshotPushButton, &QPushButton::clicked, this, &MainWidget::takeScreenshot);
}

void MainWidget::constructImageManipulationControls()
{
    // Pipelines

    imageSelectComboBox = new QComboBox;
    imageSelectComboBox->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);

    QFormLayout *imageSelectFormLayout = new QFormLayout;
    imageSelectFormLayout->addRow("Select pipeline:", imageSelectComboBox);

    QPushButton *addImagePushButton = new QPushButton("Add new");
    addImagePushButton->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);

    QPushButton *removeImagePushButton = new QPushButton("Remove selected");
    removeImagePushButton->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);

    QHBoxLayout *pipelineButtonsHBoxLayout = new QHBoxLayout;
    pipelineButtonsHBoxLayout->addWidget(addImagePushButton);
    pipelineButtonsHBoxLayout->addWidget(removeImagePushButton);

    pipelineBlendFactorsLayout = new QFormLayout;

    QPushButton *equalizeBlendFactorsPushButton = new QPushButton("Equalize blend factors");
    equalizeBlendFactorsPushButton->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);

    // Pipelines layout

    QVBoxLayout *pipelineVBoxLayout = new QVBoxLayout;
    pipelineVBoxLayout->addLayout(imageSelectFormLayout);
    pipelineVBoxLayout->addLayout(pipelineButtonsHBoxLayout);
    pipelineVBoxLayout->addLayout(pipelineBlendFactorsLayout);
    pipelineVBoxLayout->addWidget(equalizeBlendFactorsPushButton);

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
    imageOperationsListWidget->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
    imageOperationsListWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    imageOperationsListWidget->setDragDropMode(QAbstractItemView::InternalMove);

    // Parameters

    parametersLayout = new QVBoxLayout;

    QGroupBox *parametersGroupBox = new QGroupBox("Parameters");
    parametersGroupBox->setLayout(parametersLayout);

    // Selected real parameter

    selectedParameterSlider = new QSlider(Qt::Horizontal);
    selectedParameterSlider->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    selectedParameterSlider->setRange(0, 10000);

    QVBoxLayout *selectedParameterVBoxLayout = new QVBoxLayout;
    selectedParameterVBoxLayout->addWidget(selectedParameterSlider);

    selectedParameterGroupBox = new QGroupBox("No selected parameter");
    selectedParameterGroupBox->setLayout(selectedParameterVBoxLayout);

    // Image operations layout

    QVBoxLayout *imageOperationsVBoxLayout = new QVBoxLayout;
    imageOperationsVBoxLayout->addLayout(newImageOperationFormLayout);
    imageOperationsVBoxLayout->addLayout(insertRemoveHBoxLayout);
    imageOperationsVBoxLayout->addWidget(imageOperationsListWidget);
    imageOperationsVBoxLayout->addWidget(parametersGroupBox);
    imageOperationsVBoxLayout->addWidget(selectedParameterGroupBox);

    QGroupBox *imageOperationsGroupBox = new QGroupBox("Pipeline operations");
    imageOperationsGroupBox->setLayout(imageOperationsVBoxLayout);

    // Main

    QVBoxLayout *imageManipulationVBoxLayout = new QVBoxLayout;
    imageManipulationVBoxLayout->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
    imageManipulationVBoxLayout->addWidget(pipelineGroupBox);
    imageManipulationVBoxLayout->addWidget(imageOperationsGroupBox);

    imageManipulationWidget = new QWidget;
    imageManipulationWidget->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
    imageManipulationWidget->setLayout(imageManipulationVBoxLayout);

    // Signals + Slots

    connect(addImagePushButton, &QPushButton::clicked, [=](bool checked)
    {
        Q_UNUSED(checked)
        generator->addPipeline();
        initImageSelectComboBox(generator->getImageSize() - 1);
        initPipelineBlendFactorsLayout();
    });
    connect(removeImagePushButton, &QPushButton::clicked, [=](bool checked)
    {
        Q_UNUSED(checked)
        generator->removePipeline(imageSelectComboBox->currentIndex());
        initImageSelectComboBox(imageSelectComboBox->currentIndex());
        initPipelineBlendFactorsLayout();
    });
    connect(equalizeBlendFactorsPushButton, &QPushButton::clicked, [=](bool checked)
    {
        Q_UNUSED(checked)
        generator->equalizePipelineBlendFactors();
        for (int i = 0; i < generator->getPipelinesSize(); i++)
            pipelineBlendFactorLineEdit[i]->setText(QString::number(generator->getPipelineBlendFactor(i)));
    });
    connect(imageSelectComboBox, QOverload<int>::of(&QComboBox::activated), [=](int imageIndex){ initImageOperationsListWidget(imageIndex); });
    connect(imageOperationsListWidget, &QListWidget::currentRowChanged, this, &MainWidget::onImageOperationsListWidgetCurrentRowChanged);
    connect(imageOperationsListWidget->model(), &QAbstractItemModel::rowsMoved, this, &MainWidget::onRowsMoved);
    connect(insertImageOperationPushButton, &QPushButton::clicked, this, &MainWidget::insertImageOperation);
    connect(removeImageOperationPushButton, &QPushButton::clicked, this, &MainWidget::removeImageOperation);

    // Init

    initImageSelectComboBox(0);
    initPipelineBlendFactorsLayout();
    initNewImageOperationComboBox();
}

void MainWidget::constructComputationControls()
{
    togglePlotsPushButton = new QPushButton("Toggle plots");
    togglePlotsPushButton->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
    togglePlotsPushButton->setCheckable(true);

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
    fullImageLayout->addRow("DFT", dftPushButton);
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
    pixelLayout->addRow("Color-space plot", colorSpacePixelPushButton);
    pixelLayout->addRow("X-Axis:", colorSpacePixelXAxisComboBox);
    pixelLayout->addRow("Y-Axis:", colorSpacePixelYAxisComboBox);

    QGroupBox *pixelGroupBox = new QGroupBox("Single pixel");
    pixelGroupBox->setLayout(pixelLayout);

    // Main

    QVBoxLayout *computationVBoxLayout = new QVBoxLayout;
    computationVBoxLayout->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
    computationVBoxLayout->addWidget(togglePlotsPushButton);
    computationVBoxLayout->addWidget(fullImageGroupBox);
    computationVBoxLayout->addWidget(pixelGroupBox);

    computationWidget = new QWidget;
    computationWidget->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
    computationWidget->setLayout(computationVBoxLayout);

    // Signals + Slots

    connect(togglePlotsPushButton, &QPushButton::clicked, this, &MainWidget::togglePlots);
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

void MainWidget::pauseResumeSystem(bool checked)
{
    if (!checked)
    {
        timer->stop();
        pauseResumePushButton->setText("Resume");
    }
    else
    {
        timer->start(timerInterval);
        pauseResumePushButton->setText("Pause");
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

        initImageSelectComboBox(0);
        initPipelineBlendFactorsLayout();
    }
}

void MainWidget::setTimerInterval()
{
    timerInterval = timerIntervalLineEdit->text().toInt();
    timer->setInterval(timerInterval);
}

void MainWidget::setImageSize()
{
    bool stopTimer = timer->isActive();
    if (stopTimer) timer->stop();
    generator->setImageSize(imageSizeLineEdit->text().toInt());
    histogramPlot->setYMax(generator->getHistogramMax());
    if (stopTimer) timer->start(timerInterval);
}

void MainWidget::selectScreenshotPath()
{
    screenshotPath = QFileDialog::getExistingDirectory(this, "Open Directory", screenshotPath, QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    screenshotPushButton->setEnabled(!screenshotPath.isEmpty());
}

void MainWidget::setScreenshotFilename(QString text)
{
    screenshotFilename = text;
    screenshotPushButton->setEnabled(!text.isEmpty() && !screenshotPath.isEmpty());
}

void MainWidget::setTakeScreenshotSeries(int state)
{
    takeScreenshotSeries = (state == Qt::Checked);
    screenshotPushButton->setCheckable((state == Qt::Checked));
}

void MainWidget::takeScreenshot()
{
    if (!screenshotPushButton->isCheckable() && !screenshotPath.isEmpty() && !screenshotFilename.isEmpty())
    {
        QDir dir(screenshotPath);
        QString absPath = dir.absoluteFilePath(screenshotFilename + ".jpg");

        bool stopTimer = timer->isActive();
        if (stopTimer) timer->stop();
        generator->writeImage(absPath.toStdString());
        if (stopTimer) timer->start(timerInterval);
    }

    if (screenshotPushButton->isCheckable())
    {
        screenshotIndex = 1;
    }
}

void MainWidget::takeScreenshotSeriesElement()
{
    if (!screenshotPath.isEmpty() && !screenshotFilename.isEmpty())
    {
        QDir dir(screenshotPath);
        QString absPath = dir.absoluteFilePath(screenshotFilename + QString("%1").arg(screenshotIndex, 5, 10, QChar('0')) + ".jpg");

        generator->writeImage(absPath.toStdString());

        screenshotIndex++;
    }
}

void MainWidget::initImageSelectComboBox(int imageIndex)
{
    imageSelectComboBox->clear();

    for (int i = 0; i < generator->getPipelinesSize(); i++)
        imageSelectComboBox->addItem(QString("Pipeline %1").arg(i + 1));

    if (imageSelectComboBox->count() > 0)
    {
        if (imageIndex > imageSelectComboBox->count() - 1)
        {
            imageIndex = imageSelectComboBox->count() - 1;
        }

        imageSelectComboBox->setCurrentIndex(imageIndex);
        initImageOperationsListWidget(imageIndex);
    }
    else
    {
        initImageOperationsListWidget(0);
    }
}

void MainWidget::initPipelineBlendFactorsLayout()
{
    for (int i = pipelineBlendFactorLineEdit.size() - 1; i >= 0; i--)
        pipelineBlendFactorsLayout->removeRow(i);

    pipelineBlendFactorLineEdit.clear();

    for (int i = 0; i < generator->getPipelinesSize(); i++)
    {
        QLineEdit *blendFactorLineEdit = new QLineEdit;
        blendFactorLineEdit->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
        QDoubleValidator *blendFactorValidator = new QDoubleValidator(0.0, 1.0, 10, blendFactorLineEdit);
        blendFactorValidator->setLocale(QLocale::English);
        blendFactorLineEdit->setText(QString::number(generator->getPipelineBlendFactor(i)));

        pipelineBlendFactorsLayout->addRow(QString("Blend factor %1:").arg(i + 1), blendFactorLineEdit);

        pipelineBlendFactorLineEdit.push_back(blendFactorLineEdit);

        connect(blendFactorLineEdit, &QLineEdit::returnPressed, [=](){ setPipelineBlendFactorLineEditText(i); });
    }
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

void MainWidget::initImageOperationsListWidget(int imageIndex)
{
    imageOperationsListWidget->clear();

    for (int i = 0; i < generator->getImageOperationsSize(imageIndex); i++)
    {
        QListWidgetItem *newOperation = new QListWidgetItem;
        newOperation->setText(QString::fromStdString(generator->getImageOperationName(imageIndex, i)));
        imageOperationsListWidget->insertItem(i, newOperation);
    }

    if (generator->getImageOperationsSize(imageIndex) == 0)
    {
        currentImageOperationIndex[imageIndex] = -1;

        removeImageOperationPushButton->setEnabled(false);

        if (!parametersLayout->isEmpty())
        {
            QWidget *widget = parametersLayout->itemAt(0)->widget();
            parametersLayout->removeWidget(widget);
            widget->hide();
        }
    }
    else
    {
        QListWidgetItem *operation = imageOperationsListWidget->item(currentImageOperationIndex[imageIndex]);
        imageOperationsListWidget->setCurrentItem(operation);

        removeImageOperationPushButton->setEnabled(true);
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

    if (currentRow >= 0)
    {
        int imageIndex = imageSelectComboBox->currentIndex();

        if (operationsWidget)
            delete operationsWidget;

        operationsWidget = new OperationsWidget(generator->pipelines[imageIndex]->imageOperations[currentRow]);
        parametersLayout->addWidget(operationsWidget);
        operationsWidget->show();

        selectedParameterSlider->disconnect();
        selectedParameterSlider->setValue(0);
        selectedParameterGroupBox->setTitle("No selected parameter");

        for (auto widget: operationsWidget->doubleParameterWidget)
            connect(widget, &DoubleParameterWidget::focusIn, [=]()
            {
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
                selectedParameterGroupBox->setTitle("Selected parameter: " + widget->getName());
            });

        currentImageOperationIndex[imageIndex] = currentRow;

        removeImageOperationPushButton->setEnabled(true);
    }
}

void MainWidget::onRowsMoved(QModelIndex parent, int start, int end, QModelIndex destination, int row)
{
    Q_UNUSED(parent)
    Q_UNUSED(destination)
    Q_UNUSED(end)

    int imageIndex = imageSelectComboBox->currentIndex();

    if (row >= generator->getImageOperationsSize(imageIndex))
    {
        row--;
    }

    generator->swapImageOperations(imageIndex, start, row);

    currentImageOperationIndex[imageIndex] = row;
}

void MainWidget::insertImageOperation()
{
    int imageIndex = imageSelectComboBox->currentIndex();
    int newOperationIndex = newImageOperationComboBox->currentIndex();
    int currentOperationIndex = imageOperationsListWidget->currentRow();

    generator->insertImageOperation(imageIndex, newOperationIndex, currentOperationIndex);

    QListWidgetItem *newOperation = new QListWidgetItem;
    newOperation->setText(QString::fromStdString(generator->getImageOperationName(imageIndex, currentOperationIndex + 1)));
    imageOperationsListWidget->insertItem(currentOperationIndex + 1, newOperation);
    imageOperationsListWidget->setCurrentItem(newOperation);
}

void MainWidget::removeImageOperation()
{
    int imageIndex = imageSelectComboBox->currentIndex();
    int operationIndex = imageOperationsListWidget->currentRow();

    imageOperationsListWidget->takeItem(operationIndex);

    generator->removeImageOperation(imageIndex, operationIndex);

    currentImageOperationIndex[imageIndex] = operationIndex;
}

void MainWidget::iterationLoop()
{
    generator->iterate();

    // Full image computations/plots

    if (colorSpacePushButton->isChecked())
    {
        int xAxisIndex = colorSpaceXAxisComboBox->currentIndex();
        int yAxisIndex = colorSpaceYAxisComboBox->currentIndex();

        colorSpacePlot->setData(generator->getColorComponents(xAxisIndex), generator->getColorComponents(yAxisIndex));
    }

    if (dftPushButton->isChecked())
    {
        generator->computeDFT();
    }

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
    {
        generator->computeBGRPixel();
    }

    if (pixelIterationPushButton->isChecked())
    {
        pixelIterationPlot->addPoint(generator->getIterationNumber(), generator->getPixelComponent(0), generator->getPixelComponent(1), generator->getPixelComponent(2));
    }

    if (colorSpacePixelPushButton->isChecked())
    {
        int xAxisIndex = colorSpacePixelXAxisComboBox->currentIndex();
        int yAxisIndex = colorSpacePixelYAxisComboBox->currentIndex();

        colorSpacePixelPlot->addPoint(generator->getPixelComponent(xAxisIndex), generator->getPixelComponent(yAxisIndex));
    }

    // Show out image

    if (selectPixelPushButton->isChecked())
    {
        generator->showPixelSelectionCursor();
    }
    else
    {
        generator->showImage();
    }

    // Take screenshot series

    if (takeScreenshotSeries && screenshotPushButton->isChecked())
    {
        takeScreenshotSeriesElement();
    }
}

void MainWidget::togglePlots(bool checked)
{
    plotsTabWidget->setVisible(checked);

    if (checked)
    {
        resize(1100, height());
    }
    else
    {
        adjustSize();
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
    delete generator;
    event->accept();
}
