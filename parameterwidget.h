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

#ifndef PARAMETERWIDGET_H
#define PARAMETERWIDGET_H

#include "parameter.h"
#include "imageoperations.h"
#include <vector>
#include <string>
#include <QWidget>
#include <QString>
#include <QLineEdit>
#include <QIntValidator>
#include <QDoubleValidator>
#include <QComboBox>
#include <QCheckBox>
#include <QPushButton>
#include <QFormLayout>
#include <QVBoxLayout>

// A custom QLineEdit that signals focus out and in

class CustomLineEdit: public QLineEdit
{
    Q_OBJECT

public:
    CustomLineEdit(QWidget *parent = nullptr): QLineEdit(parent){}

protected:
    void focusOutEvent(QFocusEvent *event)
    {
        QLineEdit::focusOutEvent(event);
        emit focusOut();
    }
    void focusInEvent(QFocusEvent *event)
    {
        QLineEdit::focusInEvent(event);
        emit focusIn();
    }

signals:
    void focusOut();
    void focusIn();
};

// Bool parameter widget: QCheckBox

class BoolParameterWidget: public QWidget
{
public:
    QCheckBox *checkBox;

    BoolParameterWidget(BoolParameter *theBoolParameter, QWidget *parent = nullptr): QWidget(parent), boolParameter(theBoolParameter)
    {
        checkBox = new QCheckBox(QString::fromStdString(boolParameter->name), this);
        checkBox->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
        checkBox->setChecked(boolParameter->value);

        connect(checkBox, &QCheckBox::stateChanged, [=](int state){ boolParameter->value = (state == Qt::Checked); });
    }

private:
    BoolParameter *boolParameter;
};

// Options parameter widget: QComboBox

template <class T>
class OptionsParameterWidget: public QWidget
{
public:
    QComboBox *comboBox;

    OptionsParameterWidget(OptionsParameter<T> *theOptionsParameter, QWidget *parent = nullptr): QWidget(parent), optionsParameter(theOptionsParameter)
    {
        int index = 0;
        for (size_t i = 0; i < optionsParameter->values.size(); i++)
            if (optionsParameter->value == optionsParameter->values[i])
                index = i;

        comboBox = new QComboBox(this);
        comboBox->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
        for (auto valueName: optionsParameter->valueNames)
            comboBox->addItem(QString::fromStdString(valueName));
        comboBox->setCurrentIndex(index);

        connect(comboBox, QOverload<int>::of(&QComboBox::activated), [=](int index){ optionsParameter->value = optionsParameter->values[index]; });
    }

private:
    OptionsParameter<T> *optionsParameter;
};

// Integer parameter widget

class IntParameterWidget: public QWidget
{
public:
    CustomLineEdit *lineEdit;

    IntParameterWidget(IntParameter *theIntParameter, QWidget *parent = nullptr): QWidget(parent), intParameter(theIntParameter)
    {
        lineEdit = new CustomLineEdit(this);
        lineEdit->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);

        QIntValidator *validator = new QIntValidator(intParameter->min, intParameter->max, lineEdit);
        lineEdit->setValidator(validator);
        lineEdit->setText(QString::number(intParameter->value));

        connect(lineEdit, &CustomLineEdit::returnPressed, [=](){ intParameter->value = lineEdit->text().toInt(); });
        connect(lineEdit, &CustomLineEdit::focusOut, [=](){ lineEdit->setText(QString::number(intParameter->value)); });
    }

private:
    IntParameter *intParameter;
};

// Integer parameter widget

class IntOddParameterWidget: public QWidget
{
public:
    CustomLineEdit *lineEdit;

    IntOddParameterWidget(IntParameter *theIntParameter, QWidget *parent = nullptr): QWidget(parent), intParameter(theIntParameter)
    {
        lineEdit = new CustomLineEdit(this);
        lineEdit->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);

        QIntValidator *validator = new QIntValidator(intParameter->min, intParameter->max, lineEdit);
        lineEdit->setValidator(validator);
        lineEdit->setText(QString::number(intParameter->value));

        connect(lineEdit, &CustomLineEdit::returnPressed, [=]()
        {
            int value = lineEdit->text().toInt();
            if (value > 0 && value % 2 == 0)
            {
                value--;
                lineEdit->setText(QString::number(value));
            }
            intParameter->value = value;
        });
        connect(lineEdit, &CustomLineEdit::focusOut, [=](){ lineEdit->setText(QString::number(intParameter->value)); });
    }

private:
    IntParameter *intParameter;
};

// Double parameter widget

class DoubleParameterWidget: public QWidget
{
    Q_OBJECT

public:
    CustomLineEdit *lineEdit;

    int indexMax;

    DoubleParameterWidget(DoubleParameter *theDoubleParameter, int theIndexMax, QWidget *parent = nullptr): QWidget(parent), indexMax(theIndexMax), doubleParameter(theDoubleParameter)
    {
        name = QString::fromStdString(doubleParameter->name);

        lineEdit = new CustomLineEdit(this);
        lineEdit->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);

        QDoubleValidator *validator = new QDoubleValidator(doubleParameter->min, doubleParameter->max, 10, lineEdit);
        validator->setLocale(QLocale::English);
        lineEdit->setValidator(validator);
        lineEdit->setText(QString::number(doubleParameter->value));

        connect(lineEdit, &CustomLineEdit::returnPressed, [=](){ doubleParameter->value = lineEdit->text().toDouble(); setIndex(); });
        connect(lineEdit, &CustomLineEdit::focusOut, [=](){ lineEdit->setText(QString::number(doubleParameter->value)); });
        connect(lineEdit, &CustomLineEdit::focusIn, [=](){ emit focusIn(); });
    }

    QString getName(){ return name; }

    void setValue(int newIndex)
    {
        double newValue = doubleParameter->min + (doubleParameter->max - doubleParameter->min) * newIndex / indexMax;
        doubleParameter->value = newValue;
        lineEdit->setText(QString::number(newValue));
    }

    void setIndex()
    {
        int index = static_cast<int>(indexMax * (doubleParameter->value - doubleParameter->min) / (doubleParameter->max - doubleParameter->min));
        emit currentIndexChanged(index);
    }

    int getIndex()
    {
        return static_cast<int>(indexMax * (doubleParameter->value - doubleParameter->min) / (doubleParameter->max - doubleParameter->min));
    }

private:
    DoubleParameter *doubleParameter;
    QString name;

signals:
    void focusIn();
    void currentIndexChanged(int currentIndex);
};

// Kernel parameter widget

class KernelParameterWidget: public QWidget
{
public:
    QGridLayout *gridLayout;
    QPushButton *normalizePushButton;

    KernelParameterWidget(KernelParameter *theKernelParameter, QWidget *parent = nullptr): QWidget(parent), kernelParameter(theKernelParameter)
    {
        gridLayout = new QGridLayout;

        int row = 0;
        int col = 0;

        for (auto element: kernelParameter->values)
        {
            CustomLineEdit *lineEdit = new CustomLineEdit(this);
            lineEdit->setFixedWidth(50);

            QDoubleValidator *validator = new QDoubleValidator(kernelParameter->min, kernelParameter->max, 3, lineEdit);
            lineEdit->setValidator(validator);
            lineEdit->setText(QString::number(element));

            gridLayout->addWidget(lineEdit, row, col, Qt::AlignCenter);

            col++;
            if (col == 3)
            {
                row++;
                col = 0;
            }

            lineEdits.push_back(lineEdit);
        }
        for (size_t i = 0; i < kernelParameter->values.size(); i++)
        {
            connect(lineEdits[i], &CustomLineEdit::returnPressed, [=](){ kernelParameter->values[i] = lineEdits[i]->text().toFloat(); });
            connect(lineEdits[i], &CustomLineEdit::focusOut, [=](){ lineEdits[i]->setText(QString::number(kernelParameter->values[i])); });
        }

        normalizePushButton = new QPushButton("Normalize");
        normalizePushButton->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);

        connect(normalizePushButton, &QPushButton::clicked, [=]()
        {
            float sum = 0.0;
            for (auto element: kernelParameter->values)
                sum += element;
            for (size_t i = 0; i < kernelParameter->values.size(); i++)
            {
                kernelParameter->values[i] /= sum;
                lineEdits[i]->setText(QString::number(kernelParameter->values[i]));
            }
        });
    }

private:
    KernelParameter *kernelParameter;
    std::vector<CustomLineEdit*> lineEdits;
};

// Operations widget

class OperationsWidget: public QWidget
{
public:
    std::vector<DoubleParameterWidget*> doubleParameterWidget;

    OperationsWidget(ImageOperation *operation)
    {
        QVBoxLayout *vBoxLayout = new QVBoxLayout;
        QFormLayout *formLayout = new QFormLayout;

        for (auto parameter: operation->getIntParameters())
        {
            if (parameter->isOdd)
            {
                IntOddParameterWidget *widget = new IntOddParameterWidget(parameter, this);
                formLayout->addRow(QString::fromStdString(parameter->name) + ":", widget->lineEdit);
            }
            else
            {
                IntParameterWidget *widget = new IntParameterWidget(parameter, this);
                formLayout->addRow(QString::fromStdString(parameter->name) + ":", widget->lineEdit);
            }
        }
        for (auto parameter: operation->getDoubleParameters())
        {
            DoubleParameterWidget *widget = new DoubleParameterWidget(parameter, 100000, this);
            doubleParameterWidget.push_back(widget);
            formLayout->addRow(QString::fromStdString(parameter->name + ":"), widget->lineEdit);
        }
        for (auto parameter: operation->getBoolParameters())
        {
            BoolParameterWidget *widget = new BoolParameterWidget(parameter, this);
            formLayout->addRow("", widget->checkBox);
        }
        for (auto parameter: operation->getOptionsIntParameters())
        {
            OptionsParameterWidget<int> *widget = new OptionsParameterWidget<int>(parameter, this);
            formLayout->addRow(QString::fromStdString(parameter->name + ":"), widget->comboBox);
        }
        for (auto parameter: operation->getInterpolationFlagParameters())
        {
            OptionsParameterWidget<cv::InterpolationFlags> *widget = new OptionsParameterWidget<cv::InterpolationFlags>(parameter, this);
            formLayout->addRow(QString::fromStdString(parameter->name + ":"), widget->comboBox);
        }
        for (auto parameter: operation->getMorphTypeParameters())
        {
            OptionsParameterWidget<cv::MorphTypes> *widget = new OptionsParameterWidget<cv::MorphTypes>(parameter, this);
            formLayout->addRow(QString::fromStdString(parameter->name + ":"), widget->comboBox);
        }
        for (auto parameter: operation->getMorphShapeParameters())
        {
            OptionsParameterWidget<cv::MorphShapes> *widget = new OptionsParameterWidget<cv::MorphShapes>(parameter, this);
            formLayout->addRow(QString::fromStdString(parameter->name + ":"), widget->comboBox);
        }
        if (operation->getKernelParameter())
        {
            KernelParameterWidget *widget = new KernelParameterWidget(operation->getKernelParameter(), this);
            vBoxLayout->addLayout(widget->gridLayout);
            vBoxLayout->addWidget(widget->normalizePushButton);
        }

        QCheckBox *enabledCheckBox = new QCheckBox("Enabled", this);
        enabledCheckBox->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
        enabledCheckBox->setChecked(operation->isEnabled());

        formLayout->addWidget(enabledCheckBox);

        vBoxLayout->addLayout(formLayout);

        setLayout(vBoxLayout);

        connect(enabledCheckBox, &QCheckBox::stateChanged, [=](int state){ operation->enabled = (state == Qt::Checked); });
    }
    ~OperationsWidget()
    {
    }
};

#endif // PARAMETERWIDGET_H
