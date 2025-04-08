/*
*  @file      StatusViewer.cpp
*  @author    Kai Isozumi (kai.isozumi@kohyoung.com)
*  @date      March 13, 2025
*  @brief     This file is controller status viewing file for VTKWidget GUI application.
*
*  @copyright Copyright (c) 2025 Koh Young Inc., All rights reserved.
*/

#include "StatusViewer.h"
#include "ui_StatusViewer.h"

StatusViewer::StatusViewer(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::StatusViewer)
{
    ui->setupUi(this);
    ui->m_transparencyLabel->setText("Transparency: 0");
    ui->m_colorLabel->setText("Color: ");
}

void StatusViewer::updateTransparencyValue(double value) {
    //ui->m_transparencyLabel->setText(QString("Transparency: ").append(QString::number(value)));
    QString transparencyText = "Transparency: ";
    transparencyText.append(QString::number(1-value, 'f', 3));
    ui->m_transparencyLabel->setText(transparencyText);
}

void StatusViewer::updateColorValue(const QColor &color)
{
    QString colorText = "Color: ";
    QString colorString = color.name(QColor::HexArgb);
    colorText.append(colorString);
    ui->m_colorLabel->setText(colorText);
}

StatusViewer::~StatusViewer()
{
    delete ui;
}
