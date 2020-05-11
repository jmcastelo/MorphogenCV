#include "mainwidget.h"

MainWidget::MainWidget(QWidget *parent): QWidget(parent)
{
    // Images + Operations

    cv::Mat img;
    images.push_back(img);
    images.push_back(img);

    std::vector<ImageOperation*> ops;
    imageOperations.push_back(ops);
    imageOperations.push_back(ops);

    initImageOperations();

    // Init variables

    timerInterval = 25;
    imageSize = 700;
    screenshotPath = "";
    screenshotFilename = "screenshot";
    takeScreenshotSeries = false;
    screenshotIndex = 1;

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

    // Main layout

    QHBoxLayout *mainHBoxLayout = new QHBoxLayout;

    imageLabel = new QLabel;
    imageLabel->setFixedSize(imageSize, imageSize);

    mainHBoxLayout->setAlignment(Qt::AlignTop);
    mainHBoxLayout->addLayout(generalControlsVBoxLayout, 0);
    mainHBoxLayout->addWidget(imageLabel, 1);

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
    connect(timer, &QTimer::timeout, this, &MainWidget::iterationLoop);

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

void MainWidget::initImageOperations()
{
    imageOperations[0].clear();
    imageOperations[0].push_back(new ConvertTo(1.5, 0.0));
    imageOperations[0].push_back(new GaussianBlur(9, 0.9, 0.9));
    imageOperations[0].push_back(new Laplacian(3, 1.0, 0.0));

    imageOperations[1].clear();
    imageOperations[1].push_back(new Rotation(45.0, 1.08, cv::INTER_NEAREST | cv::WARP_FILL_OUTLIERS));
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

    cv::addWeighted(images[0], 0.5, images[1], 0.5, 0.0, blendedImage);

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
