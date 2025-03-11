/*
*  @file      MainWindow.cpp
*  @author    Kai Isozumi (kai.isozumi@kohyoung.com)
*  @date      February 25, 2025
*  @brief     This file is widget cpp file for VTKWidget GUI application.
*
*  @copyright Copyright (c) 2025 Koh Young Inc., All rights reserved.
*/

#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "NiiViewer.h"
#include "ControllerWidget.h"
#include "StatusViewer.h"
#include <QGridLayout>
#include <QSplitter>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    niiViewer = new NiiViewer(this);
    controllerWidget = new ControllerWidget(this);
    statusViewer = new StatusViewer(this);

    ui->m_niiViewerWidget->setLayout(new QGridLayout);
    ui->m_niiViewerWidget->layout()->addWidget(niiViewer);

    ui->m_controllerWidget->setLayout(new QGridLayout);
    ui->m_controllerWidget->layout()->addWidget(controllerWidget);

    ui->m_statusViewerWidget->setLayout(new QGridLayout);
    ui->m_statusViewerWidget->layout()->addWidget(statusViewer);

    ui->m_niiViewerWidget->setMaximumSize(2500, 1300);
    ui->m_controllerWidget->setMaximumSize(1000, 1300);
    ui->m_statusViewerWidget->setMaximumSize(5000, 500);

    connect(controllerWidget, &ControllerWidget::transparencyChanged, niiViewer, &NiiViewer::changeTransparency);
    connect(controllerWidget, &ControllerWidget::transparencyChanged, statusViewer, &StatusViewer::updateTransparencyValue);
}

MainWindow::~MainWindow()
{
    delete ui;
}
