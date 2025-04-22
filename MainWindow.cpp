/*
*  @file      m_mainWindow.cpp
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
    , niiViewer(nullptr)
    , controllerWidget(nullptr)
    , statusViewer(nullptr)
{
    ui->setupUi(this);
    niiViewer = new NiiViewer(this);
    controllerWidget = new ControllerWidget(this);
    statusViewer = new StatusViewer(this);

    ui->m_niiViewerWidget->setLayout(new QGridLayout);
    ui->m_niiViewerWidget->layout()->setSpacing(0);
    ui->m_niiViewerWidget->layout()->setContentsMargins(0,0,0,0);
    ui->m_niiViewerWidget->layout()->addWidget(niiViewer);

    ui->m_controllerWidget->setLayout(new QGridLayout);
    ui->m_controllerWidget->layout()->setSpacing(0);
    ui->m_controllerWidget->layout()->setContentsMargins(0,0,0,0);
    ui->m_controllerWidget->layout()->addWidget(controllerWidget);

    ui->m_statusViewerWidget->setLayout(new QGridLayout);
    ui->m_statusViewerWidget->layout()->setSpacing(0);
    ui->m_statusViewerWidget->layout()->setContentsMargins(0,0,0,0);
    ui->m_statusViewerWidget->layout()->addWidget(statusViewer);

    ui->m_niiViewerWidget->setMaximumSize(2500, 1800);
    ui->m_controllerWidget->setMaximumSize(500, 1300);
    ui->m_statusViewerWidget->setMaximumSize(500, 500);

    controllerWidget->setStatusViewer(statusViewer);
    niiViewer->setStatusViewer(statusViewer);

    connect(controllerWidget, &ControllerWidget::transparencyChanged, niiViewer, &NiiViewer::changeTransparency);
    connect(controllerWidget, &ControllerWidget::colorChanged, niiViewer, &NiiViewer::changeColor);
    connect(controllerWidget, &ControllerWidget::addLineRequested, niiViewer, &NiiViewer::addElectrodeLine);
}

MainWindow::~MainWindow()
{
    disconnect(controllerWidget, &ControllerWidget::transparencyChanged, niiViewer, &NiiViewer::changeTransparency);
    disconnect(controllerWidget, &ControllerWidget::colorChanged, niiViewer, &NiiViewer::changeColor);

    delete ui;
}
