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
#include <QTime>
#include <QTextEdit>

StatusViewer::StatusViewer(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::StatusViewer)
{
    ui->setupUi(this);
}

void StatusViewer::addLog(const QString &message)
{
    QString timeStampedMessage = "[" + QTime::currentTime().toString("hh:mm:ss") + "]  \t" + message + "\n";
    if(ui->m_logTextEdit->toPlainText().isEmpty())
        ui->m_logTextEdit->setText((timeStampedMessage));
    ui->m_logTextEdit->append(timeStampedMessage);
}

StatusViewer::~StatusViewer()
{
    delete ui;
}
