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

#include "imageoperations.h"
#include "mat2qimage.h"
#include <vector>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QImage>
#include <QPixmap>
#include <QTimer>
#include <QPushButton>
#include <QLineEdit>
#include <QIntValidator>
#include <QString>
#include <QFileDialog>
#include <QGroupBox>
#include <QCheckBox>
#include <QDir>
#include <QChar>

class MainWidget : public QWidget
{
    Q_OBJECT

    cv::Mat mask;

    std::vector<cv::Mat> images;
    std::vector<std::vector<ImageOperation*>> imageOperations;

    cv::Mat blendedImage;

    QPushButton *initPushButton;
    QPushButton *pauseResumePushButton;

    QLineEdit *timerIntervalLineEdit;

    QLabel *imageLabel;

    QTimer *timer;
    int timerInterval;

    int imageSize;

    QLineEdit *imageSizeLineEdit;

    QString screenshotPath;
    QString screenshotFilename;
    QPushButton *screenshotPushButton;
    QPushButton *selectScreenshotPathPushButton;
    QLineEdit *screenshotFilenameLineEdit;
    QCheckBox *screenshotSeriesCheckBox;
    bool takeScreenshotSeries;
    unsigned int screenshotIndex;

    void initSystem();
    void pauseResumeSystem(bool checked);
    void setTimerInterval();

    void setImageSize();
    void setMask();

    void selectScreenshotPath();
    void setScreenshotFilename(QString text);
    void setTakeScreenshotSeries(int state);
    void takeScreenshot();
    void takeScreenshotSeriesElement();


    void initImageOperations();
    void applyImageOperations();
    void iterationLoop();

public:
    MainWidget(QWidget *parent = nullptr);
    ~MainWidget();
};
#endif // MAINWIDGET_H
