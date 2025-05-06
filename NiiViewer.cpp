/*
*  @file      NiiViewer.cpp
*  @author    Kai Isozumi (kai.isozumi@kohyoung.com)
*  @date      May 6, 2025
*  @brief     This file set variables of vtk viewer for nii file for VTKWidget GUI application.
*
*  @copyright Copyright (c) 2025 Koh Young Inc., All rights reserved.
*/

#include "NiiViewer.h"
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
#include <vtkNew.h>
#include <vtkExtractVOI.h>
#include <vtkCenterOfMass.h>
#include <vtkPolyDataConnectivityFilter.h>
#include <vtkContourFilter.h>
#include <vtkNIFTIImageReader.h>

using RegionInfo = std::array<double, 6>;

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

    auto threshold = vtkSmartPointer<vtkImageThreshold>::New();
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

    auto smoothedVolumeData = vtkSmartPointer<vtkImageData>::New();
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
    createElectrodeLine(threshold);
}

void NiiViewer::createElectrodeLine(vtkSmartPointer<vtkImageThreshold>& threshold)
{
    vtkNew<vtkExtractVOI> extractVOI;
    extractVOI->SetInputConnection(threshold->GetOutputPort());
    extractVOI->SetVOI(35, 500, 200, 500, 300, 560);
    extractVOI->Update();

    vtkNew<vtkImageThreshold> metalThreshold;
    metalThreshold->SetInputConnection(extractVOI->GetOutputPort());
    metalThreshold->ThresholdByLower(3050);
    metalThreshold->ReplaceInOn();
    metalThreshold->SetInValue(0);
    metalThreshold->ReplaceOutOn();
    metalThreshold->SetOutValue(1);
    metalThreshold->Update();

    int dims[3];
    metalThreshold->GetOutput()->GetDimensions(dims);

    std::vector<std::array<double,3>> electrodePoints;

    for (int z = 240; /*dims[2];*/ z > 0; --z)
    {
        vtkNew<vtkExtractVOI> sliceVOI;
        sliceVOI->SetInputConnection(metalThreshold->GetOutputPort());
        sliceVOI->SetVOI(35, 500, 200, 500, z+300-1, z+300-1);
        sliceVOI->SetSampleRate(1, 1, 1);
        sliceVOI->Update();

        vtkNew<vtkContourFilter> contour;
        contour->SetInputConnection(sliceVOI->GetOutputPort());
        contour->SetValue(0, 1.0);
        contour->Update();

        vtkNew<vtkPolyDataConnectivityFilter> connectivity;
        connectivity->SetInputConnection(contour->GetOutputPort());
        connectivity->SetExtractionModeToAllRegions();
        connectivity->Update();

        int regionCount = connectivity->GetNumberOfExtractedRegions();

        for (int i = 0; i < regionCount; ++i)
        {
            connectivity->SetExtractionModeToSpecifiedRegions();
            connectivity->AddSpecifiedRegion(i);
            connectivity->Update();

            vtkPolyData* region = connectivity->GetOutput();
            double bounds[6];
            region->GetBounds(bounds);

            auto centerOfMass = vtkSmartPointer<vtkCenterOfMass>::New();
            centerOfMass->SetInputData(region);
            centerOfMass->SetUseScalarsAsWeights(false);
            centerOfMass->Update();

            double centerCoordinates[3];
            centerOfMass->GetCenter(centerCoordinates);
            std::array<double,3> center = {centerCoordinates[0], centerCoordinates[1], centerCoordinates[2]};

            bool keep = false;
            if (electrodePoints.empty())
                keep = true;
            else
            {
                auto last = electrodePoints.back();
                double dx = center[0] - last[0];
                double dy = center[1] - last[1];
                double distance = std::sqrt(dx*dx + dy*dy);
                if (distance < 5)
                    keep = true;
            }

            if (keep)
                electrodePoints.push_back(center);
        }
    }

    drawElectrodeLine(electrodePoints);
}

void NiiViewer::drawElectrodeLine(std::vector<std::array<double,3>> pointsVector)
{
    auto points = vtkSmartPointer<vtkPoints>::New();
    for (const auto& point : pointsVector)
    {
        points->InsertNextPoint(point[0], point[1], point[2]);
    }
    vtkNew<vtkPolyLine> polyLine;

    polyLine->GetPointIds()->SetNumberOfIds(pointsVector.size());
    for (vtkIdType i = 0; i < static_cast<vtkIdType>(pointsVector.size()); ++i)
    {
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
