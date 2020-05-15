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
    mainTabWidget->addTab(generalControlsWidget, "General controls");
    mainTabWidget->addTab(imageManipulationWidget, "Image operations");
    mainTabWidget->addTab(computationWidget, "Computation/plots");

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

    connect(initPushButton, &QPushButton::clicked, [=](){ generator->initSystem(bwSeedCheckBox->isChecked()); });
    connect(pauseResumePushButton, &QPushButton::clicked, this, &MainWidget::pauseResumeSystem);
    connect(timerIntervalLineEdit, &QLineEdit::returnPressed, this, &MainWidget::setTimerInterval);
    connect(imageSizeLineEdit, &QLineEdit::returnPressed, this, &MainWidget::setImageSize);
    connect(selectScreenshotPathPushButton, &QPushButton::clicked, this, &MainWidget::selectScreenshotPath);
    connect(screenshotFilenameLineEdit, &QLineEdit::textChanged, this, &MainWidget::setScreenshotFilename);
    connect(screenshotSeriesCheckBox, &QCheckBox::stateChanged, this, &MainWidget::setTakeScreenshotSeries);
    connect(screenshotPushButton, &QPushButton::clicked, this, &MainWidget::takeScreenshot);
    connect(blendFactorLineEdit, &QLineEdit::returnPressed, [=](){ generator->setBlendFactor(blendFactorLineEdit->text().toDouble()); });
    connect(imageSelectComboBox, QOverload<int>::of(&QComboBox::activated), [=](int imageIndex){ initImageOperationsListWidget(imageIndex); });
    connect(imageOperationsListWidget, &QListWidget::currentRowChanged, this, &MainWidget::onImageOperationsListWidgetCurrentRowChanged);
    connect(imageOperationsListWidget->model(), &QAbstractItemModel::rowsMoved, this, &MainWidget::onRowsMoved);
    connect(insertImageOperationPushButton, &QPushButton::clicked, this, &MainWidget::insertImageOperation);
    connect(removeImageOperationPushButton, &QPushButton::clicked, this, &MainWidget::removeImageOperation);
    connect(togglePlotsPushButton, &QPushButton::clicked, this, &MainWidget::togglePlots);
    connect(imageIterationPushButton, &QPushButton::clicked, [=](bool checked){ if (checked) imageIterationPlot->clearGraphsData(); });
    connect(pixelIterationPushButton, &QPushButton::clicked, [=](bool checked){ if (checked) pixelIterationPlot->clearGraphsData(); });
    connect(colorSpacePixelPushButton, &QPushButton::clicked, [=](bool checked){ if (checked) colorSpacePixelPlot->clearCurveData(); });
    connect(selectPixelPushButton, &QPushButton::clicked, [=](bool checked){ generator->selectingPixel = checked; });
    connect(timer, &QTimer::timeout, this, &MainWidget::iterationLoop);

    initImageOperationsListWidget(0);
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
    initPushButton = new QPushButton("Draw seed");
    initPushButton->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);

    pauseResumePushButton = new QPushButton("Start");
    pauseResumePushButton->setCheckable(true);
    pauseResumePushButton->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);

    QHBoxLayout *hBoxLayout = new QHBoxLayout;
    hBoxLayout->addWidget(initPushButton);
    hBoxLayout->addWidget(pauseResumePushButton);

    coloredSeedCheckBox = new QCheckBox("Color");
    coloredSeedCheckBox->setChecked(false);

    bwSeedCheckBox = new QCheckBox("Grays");
    bwSeedCheckBox->setChecked(true);

    QButtonGroup *seedButtonGroup = new QButtonGroup;
    seedButtonGroup->setExclusive(true);
    seedButtonGroup->addButton(coloredSeedCheckBox, 0);
    seedButtonGroup->addButton(bwSeedCheckBox, 1);

    timerIntervalLineEdit = new QLineEdit;
    timerIntervalLineEdit->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
    QIntValidator *timeIntervalIntValidator = new QIntValidator(1, 10000, timerIntervalLineEdit);
    timerIntervalLineEdit->setValidator(timeIntervalIntValidator);
    timerIntervalLineEdit->setText(QString::number(timerInterval));

    imageSizeLineEdit = new QLineEdit;
    imageSizeLineEdit->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
    QIntValidator *imageSizeIntValidator = new QIntValidator(0, 4096, imageSizeLineEdit);
    imageSizeLineEdit->setValidator(imageSizeIntValidator);
    imageSizeLineEdit->setText(QString::number(generator->getImageSize()));

    QFormLayout *formLayout = new QFormLayout;
    formLayout->addRow("Time interval (ms):", timerIntervalLineEdit);
    formLayout->addRow("Image size (px):", imageSizeLineEdit);

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

    QVBoxLayout *generalControlsVBoxLayout = new QVBoxLayout;
    generalControlsVBoxLayout->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
    generalControlsVBoxLayout->addLayout(hBoxLayout);
    generalControlsVBoxLayout->addWidget(coloredSeedCheckBox);
    generalControlsVBoxLayout->addWidget(bwSeedCheckBox);
    generalControlsVBoxLayout->addLayout(formLayout);
    generalControlsVBoxLayout->addWidget(screenshotGroupBox);

    generalControlsWidget = new QWidget;
    generalControlsWidget->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
    generalControlsWidget->setLayout(generalControlsVBoxLayout);
}

void MainWidget::constructImageManipulationControls()
{
    blendFactorLineEdit = new QLineEdit;
    blendFactorLineEdit->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
    QDoubleValidator *blendFactorDoubleValidator = new QDoubleValidator(0.0, 1.0, 10, blendFactorLineEdit);
    blendFactorDoubleValidator->setLocale(QLocale::English);
    blendFactorLineEdit->setValidator(blendFactorDoubleValidator);
    blendFactorLineEdit->setText(QString::number(generator->getBlendFactor()));

    imageSelectComboBox = new QComboBox;
    imageSelectComboBox->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
    imageSelectComboBox->addItem("Image 1");
    imageSelectComboBox->addItem("Image 2");
    imageSelectComboBox->setCurrentIndex(0);

    newImageOperationComboBox = new QComboBox;
    newImageOperationComboBox->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);

    initNewImageOperationComboBox();

    QFormLayout *formLayout = new QFormLayout;
    formLayout->addRow("Blend factor:", blendFactorLineEdit);
    formLayout->addRow("Select image:", imageSelectComboBox);
    formLayout->addRow("New operation:", newImageOperationComboBox);

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

    parametersLayout = new QVBoxLayout;

    QGroupBox *parametersGroupBox = new QGroupBox("Parameters");
    parametersGroupBox->setLayout(parametersLayout);

    QVBoxLayout *imageManipulationVBoxLayout = new QVBoxLayout;
    imageManipulationVBoxLayout->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
    imageManipulationVBoxLayout->addLayout(formLayout);
    imageManipulationVBoxLayout->addLayout(insertRemoveHBoxLayout);
    imageManipulationVBoxLayout->addWidget(imageOperationsListWidget);
    imageManipulationVBoxLayout->addWidget(parametersGroupBox);

    imageManipulationWidget = new QWidget;
    imageManipulationWidget->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
    imageManipulationWidget->setLayout(imageManipulationVBoxLayout);
}

void MainWidget::constructComputationControls()
{
    togglePlotsPushButton = new QPushButton("Toggle plots");
    togglePlotsPushButton->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
    togglePlotsPushButton->setCheckable(true);

    // Full image controls

    QLabel *histogramLabel = new QLabel("Histogram");

    histogramPushButton = new QPushButton("Start plotting");
    histogramPushButton->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
    histogramPushButton->setCheckable(true);

    QLabel *imageIterationLabel = new QLabel("Color intensity plot");

    imageIterationPushButton = new QPushButton("Start plotting");
    imageIterationPushButton->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
    imageIterationPushButton->setCheckable(true);

    QLabel *colorSpaceLabel = new QLabel("Color-space plot");

    colorSpacePushButton = new QPushButton("Start plotting");
    colorSpacePushButton->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
    colorSpacePushButton->setCheckable(true);

    QVBoxLayout *fullImageVBoxLayout = new QVBoxLayout;
    fullImageVBoxLayout->setAlignment(Qt::AlignTop);
    fullImageVBoxLayout->addWidget(histogramLabel);
    fullImageVBoxLayout->addWidget(histogramPushButton);
    fullImageVBoxLayout->addWidget(imageIterationLabel);
    fullImageVBoxLayout->addWidget(imageIterationPushButton);
    fullImageVBoxLayout->addWidget(colorSpaceLabel);
    fullImageVBoxLayout->addWidget(colorSpacePushButton);

    QGroupBox *fullImageGroupBox = new QGroupBox("Full image");
    fullImageGroupBox->setLayout(fullImageVBoxLayout);

    // Pixel controls

    QLabel *pixelSelectionLabel = new QLabel("Pixel selection");

    selectPixelPushButton = new QPushButton("Select pixel");
    selectPixelPushButton->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
    selectPixelPushButton->setCheckable(true);

    QLabel *pixelIterationLabel = new QLabel("Color intensity plot");

    pixelIterationPushButton = new QPushButton("Start plotting");
    pixelIterationPushButton->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
    pixelIterationPushButton->setCheckable(true);

    QLabel *colorSpacePixelLabel = new QLabel("Color-space plot");

    colorSpacePixelPushButton = new QPushButton("Start plotting");
    colorSpacePixelPushButton->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
    colorSpacePixelPushButton->setCheckable(true);

    QVBoxLayout *pixelVBoxLayout = new QVBoxLayout;
    pixelVBoxLayout->setAlignment(Qt::AlignTop);
    pixelVBoxLayout->addWidget(pixelSelectionLabel);
    pixelVBoxLayout->addWidget(selectPixelPushButton);
    pixelVBoxLayout->addWidget(pixelIterationLabel);
    pixelVBoxLayout->addWidget(pixelIterationPushButton);
    pixelVBoxLayout->addWidget(colorSpacePixelLabel);
    pixelVBoxLayout->addWidget(colorSpacePixelPushButton);

    QGroupBox *pixelGroupBox = new QGroupBox("Single pixel");
    pixelGroupBox->setLayout(pixelVBoxLayout);

    // Main

    QHBoxLayout *computationHBoxLayout = new QHBoxLayout;
    computationHBoxLayout->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
    computationHBoxLayout->addWidget(fullImageGroupBox);
    computationHBoxLayout->addWidget(pixelGroupBox);

    QVBoxLayout *computationVBoxLayout = new QVBoxLayout;
    computationVBoxLayout->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
    computationVBoxLayout->addWidget(togglePlotsPushButton);
    computationVBoxLayout->addLayout(computationHBoxLayout);

    computationWidget = new QWidget;
    computationWidget->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
    computationWidget->setLayout(computationVBoxLayout);

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

void MainWidget::initNewImageOperationComboBox()
{
    for (auto op: generator->availableImageOperations)
    {
        newImageOperationComboBox->addItem(op);
    }
}

void MainWidget::initImageOperationsListWidget(int imageIndex)
{
    imageOperationsListWidget->clear();

    for (int i = 0; i < generator->getImageOperationsSize(imageIndex); i++)
    {
        QListWidgetItem *newOperation = new QListWidgetItem;
        newOperation->setText(generator->getImageOperationName(imageIndex, i));
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

        QWidget *widget = generator->getImageOperationParametersWidget(imageIndex, currentRow);
        parametersLayout->addWidget(widget);
        widget->show();

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
    newOperation->setText(generator->getImageOperationName(imageIndex, currentOperationIndex + 1));
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
        colorSpacePlot->setData(generator->getColorComponents(0), generator->getColorComponents(1));
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
        colorSpacePixelPlot->addPoint(generator->getPixelComponent(0), generator->getPixelComponent(1));
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

void MainWidget::closeEvent(QCloseEvent *event)
{
    timer->stop();
    delete generator;
    event->accept();
}
