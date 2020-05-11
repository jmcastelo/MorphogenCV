#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include "imageoperations.h"
#include "mat2qimage.h"
#include <vector>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QImage>
#include <QPixmap>
#include <QTimer>

class MainWidget : public QWidget
{
    Q_OBJECT

    cv::Mat mask;
    cv::Point center;

    std::vector<cv::Mat> images;
    std::vector<std::vector<ImageOperation*>> imageOperations;

    QLabel *imageLabel;

    QTimer *timer;

    void initImageOperations();
    void applyImageOperations();
    void iterationLoop();

public:
    MainWidget(QWidget *parent = nullptr);
    ~MainWidget();
};
#endif // MAINWIDGET_H
