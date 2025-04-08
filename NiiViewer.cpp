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
#include <vtkColorTransferFunction.h>
#include <vtkImageGaussianSmooth.h>
#include <vtkPolyDataMapper.h>
#include <vtkImageThreshold.h>
#include <vtkProperty.h>
#include <vtkPolyLine.h>
#include <vtkParametricFunctionSource.h>
#include <vtkParametricSpline.h>
#include <vtkSphereSource.h>
#include <vtkGlyph3DMapper.h>

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
    auto reader = vtkSmartPointer<vtkNIFTIImageReader>::New();
    reader->SetFileName(filename.c_str());
    reader->Update();

    auto threshold = vtkSmartPointer<vtkImageThreshold>::New();
    threshold->SetInputConnection(reader->GetOutputPort());
    threshold->ThresholdByLower(900);
    threshold->ReplaceInOn();
    threshold->SetInValue(0);
    threshold->Update();

    auto volumeSmooth = vtkSmartPointer<vtkImageGaussianSmooth>::New();
    volumeSmooth->SetInputConnection(threshold->GetOutputPort());
    volumeSmooth->SetStandardDeviations(1.0, 1.0, 1.0);
    volumeSmooth->SetRadiusFactors(2.0, 2.0, 2.0);
    volumeSmooth->Update();

    auto smoothedVolumeData = vtkSmartPointer<vtkImageData>::New();
    smoothedVolumeData->DeepCopy(volumeSmooth->GetOutput());

    auto volumeMapper = vtkSmartPointer<vtkGPUVolumeRayCastMapper>::New();
    volumeMapper->SetInputData(smoothedVolumeData);
    volumeMapper->SetBlendModeToComposite();
    volumeMapper->AutoAdjustSampleDistancesOn();
    volumeMapper->SetAutoAdjustSampleDistances(1);
\
    auto opacityTransferFunction = vtkSmartPointer<vtkPiecewiseFunction>::New();
    opacityTransferFunction->AddPoint(0, 0.0);
    opacityTransferFunction->AddPoint(255, 0.1);
    opacityTransferFunction->AddPoint(500, 0.3);
    opacityTransferFunction->AddPoint(1500, 0.5);
    opacityTransferFunction->AddPoint(3500, 0.8);

    auto colorTransferFunction = vtkSmartPointer<vtkColorTransferFunction>::New();
    colorTransferFunction->AddRGBPoint(0, 0.0, 0.0, 0.0);
    colorTransferFunction->AddRGBPoint(128, 1.0, 0.5, 0.3);
    colorTransferFunction->AddRGBPoint(258, 1.0, 1.0, 1.0);
    colorTransferFunction->AddRGBPoint(3000, 1.0, 1.0, 1.0);

    auto volumeProperty = vtkSmartPointer<vtkVolumeProperty>::New();
    volumeProperty->SetInterpolationTypeToLinear();
    volumeProperty->ShadeOn();
    volumeProperty->SetScalarOpacity(opacityTransferFunction);
    volumeProperty->SetColor(colorTransferFunction);

    volume = vtkSmartPointer<vtkVolume>::New();
    volume->SetMapper(volumeMapper);
    volume->SetProperty(volumeProperty);

    renderer->RemoveAllViewProps();
    renderer->AddVolume(volume);
    renderer->ResetCamera();
    renderWindow->Render();

    addLine();
}

void NiiViewer::addLine()
{
    double origin[3] = {50.0, 0.0, 0.0};
    double p1[3] = {50.0, 100.0, 200.0};
    double p2[3] = {150.0, 150.0, 150.0};
    double p3[3] = {250, 250, 350};

    auto points = vtkSmartPointer<vtkPoints>::New();
    points->InsertNextPoint(origin);
    points->InsertNextPoint(p1);
    points->InsertNextPoint(p2);
    points->InsertNextPoint(p3);

    auto spline = vtkSmartPointer<vtkParametricSpline>::New();
    spline->SetPoints(points);

    auto functionSource = vtkSmartPointer<vtkParametricFunctionSource>::New();
    functionSource->SetParametricFunction(spline);
    functionSource->Update();

    auto mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper->SetInputConnection(functionSource->GetOutputPort());

    auto actor = vtkSmartPointer<vtkActor>::New();
    actor->SetMapper(mapper);
    actor->GetProperty()->SetColor(1.0, 0.0, 0.0);
    actor->GetProperty()->SetLineWidth(1.0);

    auto sphere = vtkSmartPointer<vtkSphereSource>::New();
    sphere->SetRadius(2.0);
    sphere->SetThetaResolution(12);
    sphere->SetPhiResolution(12);

    auto polyData = vtkSmartPointer<vtkPolyData>::New();
    polyData->SetPoints(points);

    auto pointMapper = vtkSmartPointer<vtkGlyph3DMapper>::New();
    pointMapper->SetInputData(polyData);
    pointMapper->SetSourceConnection(sphere->GetOutputPort());
    pointMapper->Update();

    auto pointActor = vtkSmartPointer<vtkActor>::New();
    pointActor->SetMapper(pointMapper);
    pointActor->GetProperty()->SetColor(0.0, 1.0, 0.0);

    renderer->AddActor(actor);
    renderer->AddActor(pointActor);
    renderWindow->Render();
}

void NiiViewer::changeTransparency(double value)
{
    auto opacityFunction = vtkSmartPointer<vtkPiecewiseFunction>::New();
    opacityFunction->AddPoint(0, 0.0);
    opacityFunction->AddPoint(500, value);
    opacityFunction->AddPoint(3071, value);

    volume->GetProperty()->SetGradientOpacity(opacityFunction);
    renderWindow->Render();
}

void NiiViewer::changeColor(const QColor &color)
{
    double r = color.redF();
    double g = color.greenF();
    double b = color.blueF();

    auto colorFunction = vtkSmartPointer<vtkColorTransferFunction>::New();
    colorFunction->AddRGBPoint(500, r, g, b);
    volume->GetProperty()->SetColor(colorFunction);
}
