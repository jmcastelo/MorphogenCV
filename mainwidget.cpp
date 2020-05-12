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
    // Images + Operations

    cv::Mat img;
    images = {img, img};

    std::vector<ImageOperation*> ops;
    imageOperations = {ops, ops};

    initImageOperations();

    // Init variables

    availableImageOperations = {"Convert to", "Dilate", "Erode", "Gaussian blur", "Laplacian", "Rotation"};

    timerInterval = 25;
    imageSize = 700;

    screenshotPath = "";
    screenshotFilename = "screenshot";
    takeScreenshotSeries = false;
    screenshotIndex = 1;

    blendFactor = 0.4;

    currentImageIndex = 0;
    currentImageOperationIndex = {0, 0};

    // OpenCV

    setMask();
    initSystem();

    // Qt

    // General controls

    initPushButton = new QPushButton("Init");

    pauseResumePushButton = new QPushButton("Pause");
    pauseResumePushButton->setCheckable(true);

    QHBoxLayout *hBoxLayout = new QHBoxLayout;
    hBoxLayout->addWidget(initPushButton);
    hBoxLayout->addWidget(pauseResumePushButton);

    QLabel *timerIntervalLabel = new QLabel("Time interval (ms)");

    timerIntervalLineEdit = new QLineEdit;
    QIntValidator *timeIntervalIntValidator = new QIntValidator(0, 10000, timerIntervalLineEdit);
    timerIntervalLineEdit->setValidator(timeIntervalIntValidator);
    timerIntervalLineEdit->setText(QString::number(timerInterval));

    QLabel *imageSizeLabel = new QLabel("Image size (px)");

    imageSizeLineEdit = new QLineEdit;
    QIntValidator *imageSizeIntValidator = new QIntValidator(1, 4096, imageSizeLineEdit);
    imageSizeLineEdit->setValidator(imageSizeIntValidator);
    imageSizeLineEdit->setText(QString::number(imageSize));

    screenshotPushButton = new QPushButton("Take screenshot");
    screenshotPushButton->setEnabled(false);
    screenshotPushButton->setCheckable(false);

    screenshotSeriesCheckBox = new QCheckBox("Series");

    selectScreenshotPathPushButton = new QPushButton("Select directory");

    QLabel *screenshotFilenameLabel = new QLabel("Filename (no extension)");

    screenshotFilenameLineEdit = new QLineEdit;
    screenshotFilenameLineEdit->setText(screenshotFilename);

    QVBoxLayout *screenshotVBoxLayout = new QVBoxLayout;
    screenshotVBoxLayout->addWidget(screenshotPushButton);
    screenshotVBoxLayout->addWidget(screenshotSeriesCheckBox);
    screenshotVBoxLayout->addWidget(selectScreenshotPathPushButton);
    screenshotVBoxLayout->addWidget(screenshotFilenameLabel);
    screenshotVBoxLayout->addWidget(screenshotFilenameLineEdit);

    QGroupBox *screenshotGroupBox = new QGroupBox("Screenshots");
    screenshotGroupBox->setLayout(screenshotVBoxLayout);

    QVBoxLayout *generalControlsVBoxLayout = new QVBoxLayout;
    generalControlsVBoxLayout->setAlignment(Qt::AlignTop);
    generalControlsVBoxLayout->addLayout(hBoxLayout);
    generalControlsVBoxLayout->addWidget(timerIntervalLabel);
    generalControlsVBoxLayout->addWidget(timerIntervalLineEdit);
    generalControlsVBoxLayout->addWidget(imageSizeLabel);
    generalControlsVBoxLayout->addWidget(imageSizeLineEdit);
    generalControlsVBoxLayout->addWidget(screenshotGroupBox);

    QWidget *generalControlsWidget = new QWidget;
    generalControlsWidget->setLayout(generalControlsVBoxLayout);

    // Image manipulation controls

    QLabel *blendFactorLabel = new QLabel("Blend factor");

    blendFactorLineEdit = new QLineEdit;
    QDoubleValidator *blendFactorDoubleValidator = new QDoubleValidator(0.0, 1.0, 10, blendFactorLineEdit);
    blendFactorDoubleValidator->setLocale(QLocale::English);
    blendFactorLineEdit->setValidator(blendFactorDoubleValidator);
    blendFactorLineEdit->setText(QString::number(blendFactor));

    QLabel *imageSelectLabel = new QLabel("Select image");

    imageSelectComboBox = new QComboBox;
    imageSelectComboBox->addItem("Image 1");
    imageSelectComboBox->addItem("Image 2");
    imageSelectComboBox->setCurrentIndex(0);

    QLabel *newImageOperationLabel = new QLabel("New operation");

    newImageOperationComboBox = new QComboBox;
    initNewImageOperationComboBox();

    insertImageOperationPushButton = new QPushButton("Insert");

    removeImageOperationPushButton = new QPushButton("Remove");

    QHBoxLayout *insertRemoveHBoxLayout = new QHBoxLayout;
    insertRemoveHBoxLayout->addWidget(insertImageOperationPushButton);
    insertRemoveHBoxLayout->addWidget(removeImageOperationPushButton);

    imageOperationsListWidget = new QListWidget;
    imageOperationsListWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    imageOperationsListWidget->setDragDropMode(QAbstractItemView::InternalMove);

    parametersLayout = new QVBoxLayout;

    QGroupBox *parametersGroupBox = new QGroupBox("Parameters");
    parametersGroupBox->setLayout(parametersLayout);

    QVBoxLayout *imageManipulationVBoxLayout = new QVBoxLayout;
    imageManipulationVBoxLayout->setAlignment(Qt::AlignTop);
    imageManipulationVBoxLayout->addWidget(blendFactorLabel);
    imageManipulationVBoxLayout->addWidget(blendFactorLineEdit);
    imageManipulationVBoxLayout->addWidget(imageSelectLabel);
    imageManipulationVBoxLayout->addWidget(imageSelectComboBox);
    imageManipulationVBoxLayout->addWidget(newImageOperationLabel);
    imageManipulationVBoxLayout->addWidget(newImageOperationComboBox);
    imageManipulationVBoxLayout->addLayout(insertRemoveHBoxLayout);
    imageManipulationVBoxLayout->addWidget(imageOperationsListWidget);
    imageManipulationVBoxLayout->addWidget(parametersGroupBox);

    QWidget *imageManipulationWidget = new QWidget;
    imageManipulationWidget->setLayout(imageManipulationVBoxLayout);

    // Main tabs

    QTabWidget *mainTabWidget = new QTabWidget;
    mainTabWidget->addTab(generalControlsWidget, "General controls");
    mainTabWidget->addTab(imageManipulationWidget, "Image operations");

    // Main layout

    QHBoxLayout *mainHBoxLayout = new QHBoxLayout;

    imageLabel = new QLabel;
    imageLabel->setFixedSize(imageSize, imageSize);

    mainHBoxLayout->setAlignment(Qt::AlignTop);
    mainHBoxLayout->addWidget(mainTabWidget, 1);
    mainHBoxLayout->addWidget(imageLabel, 0);

    setLayout(mainHBoxLayout);

    timer = new QTimer(this);

    connect(initPushButton, &QPushButton::clicked, this, &MainWidget::initSystem);
    connect(pauseResumePushButton, &QPushButton::clicked, this, &MainWidget::pauseResumeSystem);
    connect(timerIntervalLineEdit, &QLineEdit::returnPressed, this, &MainWidget::setTimerInterval);
    connect(imageSizeLineEdit, &QLineEdit::returnPressed, this, &MainWidget::setImageSize);
    connect(selectScreenshotPathPushButton, &QPushButton::clicked, this, &MainWidget::selectScreenshotPath);
    connect(screenshotFilenameLineEdit, &QLineEdit::textChanged, this, &MainWidget::setScreenshotFilename);
    connect(screenshotSeriesCheckBox, &QCheckBox::stateChanged, this, &MainWidget::setTakeScreenshotSeries);
    connect(screenshotPushButton, &QPushButton::clicked, this, &MainWidget::takeScreenshot);
    connect(blendFactorLineEdit, &QLineEdit::returnPressed, this, &MainWidget::setBlendFactor);
    connect(imageSelectComboBox, QOverload<int>::of(&QComboBox::activated), [=](int imageIndex){ initImageOperationsListWidget(imageIndex); });
    connect(imageSelectComboBox, QOverload<int>::of(&QComboBox::activated), [=](int imageIndex){ currentImageIndex = imageIndex; });
    connect(imageOperationsListWidget, &QListWidget::currentRowChanged, this, &MainWidget::onImageOperationsListWidgetCurrentRowChanged);
    connect(imageOperationsListWidget->model(), &QAbstractItemModel::rowsMoved, this, &MainWidget::onRowsMoved);
    connect(insertImageOperationPushButton, &QPushButton::clicked, this, &MainWidget::insertImageOperation);
    connect(removeImageOperationPushButton, &QPushButton::clicked, this, &MainWidget::removeImageOperation);
    connect(timer, &QTimer::timeout, this, &MainWidget::iterationLoop);

    // Init widgets

    initImageOperationsListWidget(0);
    timer->start(timerInterval);
}

MainWidget::~MainWidget()
{
}

void MainWidget::initSystem()
{
    cv::Mat seedImage = cv::Mat(imageSize, imageSize, CV_8U);
    cv::randu(seedImage, cv::Scalar::all(0), cv::Scalar::all(255));

    for (size_t i = 0; i < images.size(); i++)
    {
        seedImage.copyTo(images[i], mask);
    }
}

void MainWidget::pauseResumeSystem(bool checked)
{
    if (checked)
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
    int interval = timerIntervalLineEdit->text().toInt();
    timer->setInterval(interval);
}

void MainWidget::setImageSize()
{
    imageSize = imageSizeLineEdit->text().toInt();

    timer->stop();

    for (auto &img: images)
    {
        cv::Mat dst;
        cv::resize(img, dst, cv::Size(imageSize, imageSize));
        img = dst.clone();
    }

    setMask();

    imageLabel->setFixedSize(imageSize, imageSize);

    QImage qImage = Mat2QImage(images[0]);
    imageLabel->setPixmap(QPixmap::fromImage(qImage));

    adjustSize();

    timer->start(timerInterval);
}

void MainWidget::setMask()
{
    mask = cv::Mat::zeros(imageSize, imageSize, CV_8U);
    cv::Point center = cv::Point(imageSize / 2, imageSize / 2);
    cv::circle(mask, center, imageSize / 2, cv::Scalar(255, 255, 255), -1);
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

        cv::String filename = absPath.toStdString();
        std::vector<int> params = {cv::IMWRITE_JPEG_QUALITY, 100};

        timer->stop();
        cv::imwrite(filename, images[0], params);
        timer->start(timerInterval);
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

        cv::String filename = absPath.toStdString();
        std::vector<int> params = {cv::IMWRITE_JPEG_QUALITY, 100};

        cv::imwrite(filename, images[0], params);

        screenshotIndex++;
    }
}

void MainWidget::setBlendFactor()
{
    blendFactor = blendFactorLineEdit->text().toDouble();
}

void MainWidget::initImageOperations()
{
    imageOperations[0].clear();
    //imageOperations[0].push_back(new ConvertTo(1.2, 0.0));
    imageOperations[0].push_back(new GaussianBlur(9, 0.9, 0.9));
    imageOperations[0].push_back(new Laplacian(3, 1.0, 0.0));

    imageOperations[1].clear();
    imageOperations[1].push_back(new Rotation(45.0, 1.08, cv::INTER_NEAREST | cv::WARP_FILL_OUTLIERS));
}

void MainWidget::initNewImageOperationComboBox()
{
    for (auto op: availableImageOperations)
    {
        newImageOperationComboBox->addItem(op);
    }
}

void MainWidget::initImageOperationsListWidget(int imageIndex)
{
    imageOperationsListWidget->clear();

    for (size_t i = 0; i < imageOperations[imageIndex].size(); i++)
    {
        QListWidgetItem *newOperation = new QListWidgetItem;
        newOperation->setText(imageOperations[imageIndex][i]->getName());
        imageOperationsListWidget->insertItem(i, newOperation);
    }

    if (imageOperations[imageIndex].empty())
    {
        currentImageOperationIndex[imageIndex] = -1;
        removeImageOperationPushButton->setEnabled(false);
    }
    else
    {
        QListWidgetItem *operation = imageOperationsListWidget->item(currentImageOperationIndex[imageIndex]);
        imageOperationsListWidget->setCurrentItem(operation);

        if (!parametersLayout->isEmpty())
        {
            parametersLayout->removeWidget(imageOperations[currentImageIndex][currentImageOperationIndex[currentImageIndex]]->getParametersWidget());
            imageOperations[currentImageIndex][currentImageOperationIndex[currentImageIndex]]->getParametersWidget()->hide();
        }
        parametersLayout->addWidget(imageOperations[imageIndex][currentImageOperationIndex[imageIndex]]->getParametersWidget());
        imageOperations[imageIndex][currentImageOperationIndex[imageIndex]]->getParametersWidget()->show();

        removeImageOperationPushButton->setEnabled(true);
    }
}

void MainWidget::onImageOperationsListWidgetCurrentRowChanged(int currentRow)
{
    if (currentRow >= 0)
    {
        int imageIndex = imageSelectComboBox->currentIndex();

        if (!parametersLayout->isEmpty())
        {
            parametersLayout->removeWidget(imageOperations[imageIndex][currentImageOperationIndex[imageIndex]]->getParametersWidget());
            imageOperations[imageIndex][currentImageOperationIndex[imageIndex]]->getParametersWidget()->hide();
        }
        parametersLayout->addWidget(imageOperations[imageIndex][currentRow]->getParametersWidget());
        imageOperations[imageIndex][currentRow]->getParametersWidget()->show();

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

    if (row >= static_cast<int>(imageOperations[imageIndex].size()))
    {
        row--;
    }

    ImageOperation *operation = imageOperations[imageIndex][start];
    imageOperations[imageIndex][start] = imageOperations[imageIndex][row];
    imageOperations[imageIndex][row] = operation;

    currentImageOperationIndex[imageIndex] = row;
}

void MainWidget::insertImageOperation()
{
    int imageIndex = imageSelectComboBox->currentIndex();
    int newOperationIndex = newImageOperationComboBox->currentIndex();
    int currentOperationIndex = imageOperationsListWidget->currentRow();

    std::vector<ImageOperation*>::iterator it = imageOperations[imageIndex].begin();

    if (newOperationIndex == 0)
    {
        imageOperations[imageIndex].insert(it + currentOperationIndex + 1, new ConvertTo(1.0, 0.0));
    }
    else if (newOperationIndex == 1)
    {
        imageOperations[imageIndex].insert(it + currentOperationIndex + 1, new Dilate(3));
    }
    else if (newOperationIndex == 2)
    {
        imageOperations[imageIndex].insert(it + currentOperationIndex + 1, new Erode(3));
    }
    else if (newOperationIndex == 3)
    {
        imageOperations[imageIndex].insert(it + currentOperationIndex + 1, new GaussianBlur(3, 0.0, 0.0));
    }
    else if (newOperationIndex == 4)
    {
        imageOperations[imageIndex].insert(it + currentOperationIndex + 1, new Laplacian(3, 1.0, 0.0));
    }
    else if (newOperationIndex == 5)
    {
        imageOperations[imageIndex].insert(it + currentOperationIndex + 1, new Rotation(0.0, 1.0, cv::INTER_NEAREST | cv::WARP_FILL_OUTLIERS));
    }

    QListWidgetItem *newOperation = new QListWidgetItem;
    newOperation->setText(imageOperations[imageIndex][currentOperationIndex + 1]->getName());
    imageOperationsListWidget->insertItem(currentOperationIndex + 1, newOperation);
    imageOperationsListWidget->setCurrentItem(newOperation);

    // Update parameters layout

    if (!parametersLayout->isEmpty())
    {
        parametersLayout->removeWidget(imageOperations[imageIndex][currentImageOperationIndex[imageIndex]]->getParametersWidget());
        imageOperations[imageIndex][currentImageOperationIndex[imageIndex]]->getParametersWidget()->hide();
    }

    parametersLayout->addWidget(imageOperations[imageIndex][currentOperationIndex + 1]->getParametersWidget());
    imageOperations[imageIndex][currentOperationIndex + 1]->getParametersWidget()->show();

    currentImageOperationIndex[imageIndex] = currentOperationIndex + 1;
}

void MainWidget::removeImageOperation()
{
    int imageIndex = imageSelectComboBox->currentIndex();
    int operationIndex = imageOperationsListWidget->currentRow();

    parametersLayout->removeWidget(imageOperations[imageIndex][operationIndex]->getParametersWidget());
    imageOperations[imageIndex][operationIndex]->getParametersWidget()->hide();

    std::vector<ImageOperation*>::iterator it = imageOperations[imageIndex].begin();

    imageOperations[imageIndex].erase(it + operationIndex);

    imageOperationsListWidget->takeItem(operationIndex);
}

void MainWidget::applyImageOperations()
{
    for (size_t i = 0; i < images.size(); i++)
    {
        cv::Mat src = images[i];

        for (size_t j = 0; j < imageOperations[i].size(); j++)
        {
            cv::Mat dst = imageOperations[i][j]->applyOperation(src);
            dst.copyTo(src, mask);
        }

        images[i] = src.clone();
    }
}

void MainWidget::iterationLoop()
{
    applyImageOperations();

    cv::addWeighted(images[0], blendFactor, images[1], 1.0 - blendFactor, 0.0, blendedImage);

    for (size_t i = 0; i < images.size(); i++)
    {
        blendedImage.copyTo(images[i], mask);
    }

    QImage qImage = Mat2QImage(blendedImage);
    imageLabel->setPixmap(QPixmap::fromImage(qImage));

    if (screenshotPushButton->isChecked() && takeScreenshotSeries)
    {
        takeScreenshotSeriesElement();
    }
}
