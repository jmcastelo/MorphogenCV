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

    // OpenCV

    cv::Mat seedImage = cv::Mat (700, 700, CV_8U);
    cv::randu(seedImage, cv::Scalar::all(0), cv::Scalar::all(255));

    mask = cv::Mat::zeros(seedImage.rows, seedImage.cols, CV_8U);
    center = cv::Point(seedImage.cols / 2, seedImage.rows / 2);
    cv::circle(mask, center, seedImage.rows / 2, cv::Scalar(255, 255, 255), -1);

    for (size_t i = 0; i < images.size(); i++)
    {
        seedImage.copyTo(images[i], mask);
    }

    // Qt

    QVBoxLayout *mainVBoxLayout = new QVBoxLayout;

    imageLabel = new QLabel;

    mainVBoxLayout->addWidget(imageLabel);

    setLayout(mainVBoxLayout);

    timer = new QTimer(this);

    connect(timer, &QTimer::timeout, this, &MainWidget::iterationLoop);

    timer->start(25);
}

MainWidget::~MainWidget()
{
}

void MainWidget::initImageOperations()
{
    imageOperations[0].clear();
    imageOperations[0].push_back(new GaussianBlur(0, 1.0, 1.0));
    imageOperations[0].push_back(new Laplacian(3, 1.0, 0.0));
    imageOperations[0].push_back(new ConvertTo(2.0, 0.0));

    imageOperations[1].clear();
    imageOperations[1].push_back(new Rotation(15.0, 1.01));
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

    cv::Mat blendedImage;
    cv::addWeighted(images[0], 0.25, images[1], 0.75, 0.0, blendedImage);

    for (size_t i = 0; i < images.size(); i++)
    {
        blendedImage.copyTo(images[i], mask);
    }

    QImage qImage = Mat2QImage(blendedImage);
    imageLabel->setPixmap(QPixmap::fromImage(qImage));
}
