/*
*  @file      NiiViewer.cpp
*  @author    Kai Isozumi (kai.isozumi@kohyoung.com)
*  @date      March 13, 2025
*  @brief     This file is .nii file viewing for VTKWidget GUI application.
*
*  @copyright Copyright (c) 2025 Koh Young Inc., All rights reserved.
*/

#include "NiiViewer.h"
#include <vtkNIFTIImageReader.h>
#include <vtkGPUVolumeRayCastMapper.h>
#include <vtkPiecewiseFunction.h>
#include <vtkVolumeProperty.h>
#include <vtkImageData.h>
#include <QDebug>
#include <vtkSmartVolumeMapper.h>
#include <vtkColorTransferFunction.h>
#include <vtkImageGaussianSmooth.h>
#include <vtkMarchingCubes.h>
#include <vtkStripper.h>
#include <vtkPolyDataMapper.h>
#include <vtkImageThreshold.h>
#include <vtkSmoothPolyDataFilter.h>
#include <vtkWindowedSincPolyDataFilter.h>
#include <vtkPolyDataToImageStencil.h>
#include <vtkImageStencil.h>
#include <vtkCleanPolyData.h>
#include <vtkPolyDataNormals.h>
#include <vtkProperty.h>

NiiViewer::NiiViewer(QWidget *parent)
    : QMainWindow(parent)
    , vtkWidget(new QVTKOpenGLNativeWidget(this))
{
    renderWindow = vtkSmartPointer<vtkGenericOpenGLRenderWindow>::New();
    renderer = vtkSmartPointer<vtkRenderer>::New();
    renderWindow->AddRenderer(renderer);

    vtkWidget->setRenderWindow(renderWindow);
    renderer->SetBackground(0.0, 0.0, 0.0);

    setCentralWidget(vtkWidget);
    setWindowTitle("");
    loadFile("C:/Users/kai/projects/VTKWidget/resources/new_CT.nii");
    renderWindow->Render();
}

NiiViewer::~NiiViewer()
{
    delete vtkWidget;
}

void NiiViewer::loadFile(const std::string &filename)
{
    vtkSmartPointer<vtkNIFTIImageReader> reader = vtkSmartPointer<vtkNIFTIImageReader>::New();
    reader->SetFileName(filename.c_str());
    reader->Update();

    vtkSmartPointer<vtkImageThreshold> threshold = vtkSmartPointer<vtkImageThreshold>::New();
    threshold->SetInputConnection(reader->GetOutputPort());
    threshold->ThresholdByLower(900);
    threshold->ReplaceInOn();
    threshold->SetInValue(0);
    threshold->Update();

    vtkSmartPointer<vtkImageGaussianSmooth> volumeSmooth = vtkSmartPointer<vtkImageGaussianSmooth>::New();
    volumeSmooth->SetInputConnection(threshold->GetOutputPort());
    volumeSmooth->SetStandardDeviations(1.0, 1.0, 1.0);
    volumeSmooth->SetRadiusFactors(2.0, 2.0, 2.0);
    volumeSmooth->Update();

    vtkSmartPointer<vtkMarchingCubes> toPolyData = vtkSmartPointer<vtkMarchingCubes>::New();
    toPolyData->SetInputConnection(volumeSmooth->GetOutputPort());
    toPolyData->SetValue(0, 1150);
    toPolyData->Update();

    vtkSmartPointer<vtkCleanPolyData> cleanPoly = vtkSmartPointer<vtkCleanPolyData>::New();
    cleanPoly->SetInputConnection(toPolyData->GetOutputPort());
    cleanPoly->Update();

    vtkSmartPointer<vtkSmoothPolyDataFilter> surfaceSmooth = vtkSmartPointer<vtkSmoothPolyDataFilter>::New();
    surfaceSmooth->SetInputConnection(cleanPoly->GetOutputPort());
    surfaceSmooth->SetNumberOfIterations(10);
    surfaceSmooth->SetRelaxationFactor(0.1);
    surfaceSmooth->FeatureEdgeSmoothingOff();
    surfaceSmooth->BoundarySmoothingOn();
    surfaceSmooth->Update();

    for (int i = 0; i < 9; ++i)
    {
        vtkSmartPointer<vtkSmoothPolyDataFilter> smoothIteration = vtkSmartPointer<vtkSmoothPolyDataFilter>::New();
        smoothIteration->SetInputConnection(surfaceSmooth->GetOutputPort());
        smoothIteration->SetNumberOfIterations(15);
        smoothIteration->SetRelaxationFactor(0.1);
        smoothIteration->FeatureEdgeSmoothingOff();
        smoothIteration->BoundarySmoothingOn();
        smoothIteration->Update();
        surfaceSmooth = smoothIteration;
    }
    vtkSmartPointer<vtkGPUVolumeRayCastMapper> volumeMapper = vtkSmartPointer<vtkGPUVolumeRayCastMapper>::New();
    volumeMapper->SetInputConnection(volumeSmooth->GetOutputPort());

    vtkSmartPointer<vtkVolumeProperty> volumeProperty = vtkSmartPointer<vtkVolumeProperty>::New();
    volumeProperty->SetInterpolationTypeToLinear();
    volumeProperty->ShadeOn();

    vtkSmartPointer<vtkProperty> surfaceProperty = vtkSmartPointer<vtkProperty>::New();

    vtkSmartPointer<vtkPiecewiseFunction> opacityTransferFunction = vtkSmartPointer<vtkPiecewiseFunction>::New();
    opacityTransferFunction->AddPoint(0, 0.0);
    opacityTransferFunction->AddPoint(255, 1.0);
    volumeProperty->SetScalarOpacity(opacityTransferFunction);

    vtkSmartPointer<vtkColorTransferFunction> colorTransferFunction = vtkSmartPointer<vtkColorTransferFunction>::New();
    colorTransferFunction->AddRGBPoint(0, 0.0, 0.0, 0.0);
    colorTransferFunction->AddRGBPoint(128, 1.0, 0.5, 0.3);
    colorTransferFunction->AddRGBPoint(258, 1.0, 1.0, 1.0);
    colorTransferFunction->AddRGBPoint(3000, 1.0, 1.0, 1.0);

    volumeProperty->SetColor(colorTransferFunction);

    volume = vtkSmartPointer<vtkVolume>::New();
    volume->SetMapper(volumeMapper);
    volume->SetProperty(volumeProperty);

    vtkSmartPointer<vtkPolyDataMapper> surfaceMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    surfaceMapper->SetInputConnection(surfaceSmooth->GetOutputPort());
    surfaceMapper->ScalarVisibilityOn();
    surfaceMapper->SetScalarRange(0, 3071);

    surfaceProperty->SetColor(1.0, 1.0, 1.0);

    surfaceActor = vtkSmartPointer<vtkActor>::New();
    surfaceActor->SetMapper(surfaceMapper);
    surfaceActor->SetProperty(surfaceProperty);

    qDebug() << "Before Smoothing: " << toPolyData->GetOutput()->GetNumberOfPoints();
    qDebug() << "After Smoothing: " << surfaceSmooth->GetOutput()->GetNumberOfPoints();

    renderer->RemoveAllViewProps();
    renderer->AddVolume(volume);
    renderer->AddActor(surfaceActor);
    renderer->ResetCamera();
    renderWindow->Render();
}

void NiiViewer::changeTransparency(double value)
{
    vtkSmartPointer<vtkPiecewiseFunction> opacityFunction = vtkSmartPointer<vtkPiecewiseFunction>::New();
    opacityFunction->AddPoint(0, 0.0);
    opacityFunction->AddPoint(500, value);
    opacityFunction->AddPoint(3071, value);

    volume->GetProperty()->SetGradientOpacity(opacityFunction);
    surfaceActor->GetProperty()->SetOpacity(value);
    renderWindow->Render();
}

void NiiViewer::changeColor(const QColor &color)
{
    double r = color.redF();
    double g = color.greenF();
    double b = color.blueF();

    vtkSmartPointer<vtkColorTransferFunction> colorFunction = vtkSmartPointer<vtkColorTransferFunction>::New();
    colorFunction->AddRGBPoint(500, r, g, b);
    volume->GetProperty()->SetColor(colorFunction);
    surfaceActor->GetProperty()->SetColor(r, g, b);
}
