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
#include <vtkImageResample.h>
#include <vtkPolyLine.h>
#include <vtkSphereSource.h>
#include <vtkGlyph3DMapper.h>
#include <vtkNew.h>
#include <vtkCamera.h>

NiiViewer::NiiViewer(QWidget *parent)
    : QMainWindow(parent)
    , vtkWidget(new QVTKOpenGLNativeWidget(this))
    , statusViewer(nullptr)
{
    renderWindow = vtkSmartPointer<vtkGenericOpenGLRenderWindow>::New();
    renderer = vtkSmartPointer<vtkRenderer>::New();
    renderWindow->AddRenderer(renderer);

    vtkWidget->setRenderWindow(renderWindow);
    renderer->SetBackground(0.0, 0.0, 0.0);

    setCentralWidget(vtkWidget);
    setWindowTitle("");
    loadFile("C:/Users/kai/projects/VTKWidget/resources/Head_with_electrode.nii");
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

    vtkNew<vtkImageThreshold> threshold;
    threshold->SetInputConnection(reader->GetOutputPort());
    threshold->ThresholdByLower(900);
    threshold->ReplaceInOn();
    threshold->SetInValue(0);
    threshold->Update();

    vtkNew<vtkImageGaussianSmooth> volumeSmooth;
    volumeSmooth->SetInputConnection(threshold->GetOutputPort());
    volumeSmooth->SetStandardDeviations(1.0, 1.0, 1.0);
    volumeSmooth->SetRadiusFactors(2.0, 2.0, 2.0);
    volumeSmooth->Update();

    vtkNew<vtkImageData> smoothedVolumeData;
    smoothedVolumeData->DeepCopy(volumeSmooth->GetOutput());

    auto volumeMapper = vtkSmartPointer<vtkGPUVolumeRayCastMapper>::New();
    volumeMapper->SetInputData(smoothedVolumeData);
    volumeMapper->SetBlendModeToComposite();
    volumeMapper->AutoAdjustSampleDistancesOn();
    volumeMapper->SetAutoAdjustSampleDistances(1);

    vtkNew<vtkPiecewiseFunction> opacityTransferFunction;
    opacityTransferFunction->AddPoint(0, 0.0);
    opacityTransferFunction->AddPoint(255, 0.1);
    opacityTransferFunction->AddPoint(500, 0.3);
    opacityTransferFunction->AddPoint(1500, 0.5);
    opacityTransferFunction->AddPoint(3500, 1.0);

    vtkNew<vtkColorTransferFunction> colorTransferFunction;
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
    createELectrodeLine();
}

void NiiViewer::createELectrodeLine()
{
    const std::vector<std::array<double, 3>> pointList = {
                                                          {130.762, (260-114.956), 403*0.6},
                                                          {123.462, (260-116.321), 425*0.6},
                                                          {117.035, 260-117.051, 444*0.6},
                                                          {107.767, 260-118.368, 471*0.6},
                                                          {100.626, 260-118.844, 491*0.6},
                                                          {91.745, 260-119.251, 514*0.6},
                                                          {88.529, 260-119.040, 521*0.6},
                                                          {85.820, 260-118.912, 526*0.6},
                                                          {72.786, 260-117.135, 540*0.6},
                                                          {66.713, 260-115.781, 544*0.6},
                                                          {59.583, 260-113.326, 548*0.6},
                                                          {50.104, 260-110.195, 551*0.6},
                                                          {33.261, 260-103.085, 551*0.6},
                                                          {24.882, 260-98.938, 548*0.6},
                                                          {15.996, 260-93.712, 542*0.6},
                                                          };

    vtkNew<vtkPoints> points;
    vtkNew<vtkPolyLine> polyLine;

    polyLine->GetPointIds()->SetNumberOfIds(pointList.size());
    for (vtkIdType i = 0; i < static_cast<vtkIdType>(pointList.size()); ++i)
    {
        points->InsertNextPoint(pointList[i].data());
        polyLine->GetPointIds()->SetId(i, i);
    }

    vtkNew<vtkCellArray> cells;
    cells->InsertNextCell(polyLine);

    vtkNew<vtkPolyData> polyData;
    polyData->SetPoints(points);
    polyData->SetLines(cells);

    auto mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper->SetInputData(polyData);

    m_lineActor = vtkSmartPointer<vtkActor>::New();
    m_lineActor->SetMapper(mapper);
    m_lineActor->GetProperty()->SetColor(1.0, 0.0, 0.0);
    m_lineActor->GetProperty()->SetLineWidth(2.0);

    renderer->AddActor(m_lineActor);
    //renderWindow->Render();
}

void NiiViewer::addElectrodeLine(bool lineStatus)
{
    m_lineActor->SetVisibility(lineStatus);
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

void NiiViewer::setStatusViewer(StatusViewer* viewer)
{
    statusViewer = viewer;
}
