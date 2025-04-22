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
//#include "NiiViewer.h"

ControllerWidget::ControllerWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ControllerWidget)
    , colorButton(nullptr)
    , m_lineShown(false)
    , statusViewer(nullptr)
{
    ui->setupUi(this);

    ui->m_transparencySlider->setRange(0,100);
    ui->m_transparencySlider->setTickInterval(1);
    ui->m_transparencySlider->setValue(100);
    ui->m_transparencySlider->setFixedWidth(300);

    connect(ui->m_transparencySlider, &QSlider::valueChanged, this, &ControllerWidget::changeTransparency);
    connect(ui->m_colorButton, &QPushButton::clicked, this, &ControllerWidget::openColorDialog);
    connect(ui->m_ShowLinePushButton, &QPushButton::clicked, this, &ControllerWidget::addLinePressed);
    connect(ui->m_opacityReturnDefaultButton, &QPushButton::clicked, this, &ControllerWidget::resetOpacityToDefault);
    connect(ui->m_colorReturnDefaultButton, &QPushButton::clicked, this, &ControllerWidget::resetColorToDefault);
}

void ControllerWidget::changeTransparency(double value)
{
    double opacityScale = value / 100.0;
    statusViewer->addLog(QString("Opacity changed to %1").arg(opacityScale, 0, 'f', 2));
    emit transparencyChanged(opacityScale);
}

void ControllerWidget::openColorDialog()
{
    QColor color = QColorDialog::getColor(Qt::white, nullptr, "Select Color");
    if (color.isValid())
    {
        statusViewer->addLog(QString("Color changed to RGB(%1, %2, %3)").arg(color.red()).arg(color.green()).arg(color.blue()));
        emit colorChanged(color);
    }
}

void ControllerWidget::setStatusViewer(StatusViewer* viewer)
{
    statusViewer = viewer;
}

void ControllerWidget::resetOpacityToDefault()
{
    ui->m_transparencySlider->setValue(50);
    statusViewer->addLog("Reset to default transparency: 0.5");
}

void ControllerWidget::resetColorToDefault()
{
    QColor color = Qt::white;
    statusViewer->addLog(QString("Color reset to default: RGB(%1, %2, %3)").arg(color.red()).arg(color.green()).arg(color.blue()));
    emit colorChanged(color);
}

void ControllerWidget::addLinePressed()
{
    if (m_lineShown)
    {
        statusViewer->addLog(QString("Electrode line shown"));
        emit addLineRequested(true);
        m_lineShown = false;
    }
    else
    {
        statusViewer->addLog(QString("Electrode line hidden"));
        emit addLineRequested(false);
        m_lineShown = true;
    }
}

ControllerWidget::~ControllerWidget()
{
    disconnect(ui->m_transparencySlider, &QSlider::valueChanged, this, &ControllerWidget::changeTransparency);
    disconnect(ui->m_colorButton, &QPushButton::clicked, this, &ControllerWidget::openColorDialog);
    delete ui;
}
