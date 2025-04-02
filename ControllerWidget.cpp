/*
*  @file      ControllerWidget.cpp
*  @author    Kai Isozumi (kai.isozumi@kohyoung.com)
*  @date      March 13, 2025
*  @brief     This file is controller file for VTKWidget GUI application.
*
*  @copyright Copyright (c) 2025 Koh Young Inc., All rights reserved.
*/

#include "ControllerWidget.h"
#include "ui_ControllerWidget.h"
#include <vtkPiecewiseFunction.h>
#include <vtkSmartPointer.h>
#include <vtkVolume.h>
#include <QPushButton>

ControllerWidget::ControllerWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ControllerWidget)
{
    ui->setupUi(this);
    //transparencySlider = new QSlider(Qt::Horizontal, this);
    ui->m_transparencySlider->setRange(0,100);
    ui->m_transparencySlider->setTickInterval(1);
    ui->m_transparencySlider->setValue(100);
    ui->m_transparencySlider->setFixedWidth(300);

    //colorButton = new QPushButton("Select Bone Color", this);

    connect(ui->m_transparencySlider, &QSlider::valueChanged, this, &ControllerWidget::changeTransparency);
    connect(ui->m_colorButton, &QPushButton::clicked, this, &ControllerWidget::openColorDialog);
}

ControllerWidget::~ControllerWidget()
{
    delete ui;
}

void ControllerWidget::changeTransparency(double value)
{
    double opacityScale = value / 100.0;
    emit transparencyChanged(opacityScale);
}

void ControllerWidget::openColorDialog()
{
    QColor color = QColorDialog::getColor(Qt::white, nullptr, "Select Color");
    if (color.isValid())
        emit colorChanged(color);
}
