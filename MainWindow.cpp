/*
*  @file      MainWindow.cpp
*  @author    Kai Isozumi (kai.isozumi@kohyoung.com)
*  @date      February 25, 2025
*  @brief     This file is widget cpp file for VTKWidget GUI application.
*
*  @copyright Copyright (c) 2025 Koh Young Inc., All rights reserved.
*/

#include "MainWindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , vtkWidget(new QVTKOpenGLNativeWidget(this))
{
    renderWindow = vtkSmartPointer<vtkGenericOpenGLRenderWindow>::New();
    renderer = vtkSmartPointer<vtkRenderer>::New();
    renderWindow->AddRenderer(renderer);

    vtkWidget->setRenderWindow(renderWindow);
    renderer->SetBackground(0.1, 0.2, 0.3);
    setCentralWidget(vtkWidget);
}

MainWindow::~MainWindow()
{
}

void MainWindow::loadFile(const std::string &filename)
{
    vtkSmartPointer<vtkPolyDataMapper> mapper;
    vtkSmartPointer<vtkPolyData> polyData;

    vtkSmartPointer<vtkSTLReader> reader = vtkSmartPointer<vtkSTLReader>::New();
    reader->SetFileName(filename.c_str());
    reader->Update();
    polyData = reader->GetOutput();

    mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper->SetInputData(polyData);

    vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
    actor->SetMapper(mapper);

    renderer->RemoveAllViewProps();
    renderer->AddActor(actor);
    renderer->ResetCamera();

    renderWindow->Render();
}
