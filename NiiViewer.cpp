#include "NiiViewer.h"
#include <vtkNIFTIImageReader.h>
//#include <vtkImageMapper3D.h>
//#include <QVTKOpenGLWidget.h>
#include <vtkGPUVolumeRayCastMapper.h>
#include <vtkPiecewiseFunction.h>
#include <vtkVolumeProperty.h>
#include <vtkImageData.h>
#include <QDebug>
#include <vtkSmartVolumeMapper.h>
#include <vtkColorTransferFunction.h>
//#include <vtkContourFilter.h>
#include <vtkImageGaussianSmooth.h>

NiiViewer::NiiViewer(QWidget *parent)
    : QMainWindow(parent)
    , vtkWidget(new QVTKOpenGLNativeWidget(this))
{
    renderWindow = vtkSmartPointer<vtkGenericOpenGLRenderWindow>::New();
    renderer = vtkSmartPointer<vtkRenderer>::New();
    renderWindow->AddRenderer(renderer);

    vtkWidget->setRenderWindow(renderWindow);
    renderer->SetBackground(0.1, 0.2, 0.3);

    setCentralWidget(vtkWidget);
    loadFile("C:/Users/kai/projects/VTKWidget/resources/new_CT.nii");
    renderWindow->Render();

}

NiiViewer::~NiiViewer()
{
    delete vtkWidget;
}

void NiiViewer::loadFile(const std::string &filename)
{
    qDebug() << "OpenGL Version: " << QString::fromStdString(renderWindow->ReportCapabilities()) << "END\n\n";

    // Load the NIfTI file
    vtkSmartPointer<vtkNIFTIImageReader> reader = vtkSmartPointer<vtkNIFTIImageReader>::New();
    reader->SetFileName(filename.c_str());
    reader->Update();

    vtkSmartPointer<vtkImageData> imageData = reader->GetOutput();

    // Volume Rendering
    vtkSmartPointer<vtkGPUVolumeRayCastMapper> volumeMapper = vtkSmartPointer<vtkGPUVolumeRayCastMapper>::New();
    //vtkSmartPointer<vtkSmartVolumeMapper> volumeMapper = vtkSmartPointer<vtkSmartVolumeMapper>::New();
    volumeMapper->SetInputData(imageData);
//    volumeMapper->SetRequestedRenderModeToRayCast();

    // Set Volume Properties (Opacity and Color)
    vtkSmartPointer<vtkVolumeProperty> volumeProperty = vtkSmartPointer<vtkVolumeProperty>::New();
    volumeProperty->SetInterpolationTypeToLinear();
    volumeProperty->ShadeOn();

    vtkSmartPointer<vtkPiecewiseFunction> opacityTransferFunction = vtkSmartPointer<vtkPiecewiseFunction>::New();
    opacityTransferFunction->AddPoint(0, 0.0);
    opacityTransferFunction->AddPoint(255, 1.0);
    volumeProperty->SetScalarOpacity(opacityTransferFunction);

    vtkSmartPointer<vtkColorTransferFunction> colorTransferFunction = vtkSmartPointer<vtkColorTransferFunction>::New();
    colorTransferFunction->AddRGBPoint(0, 0.0, 0.0, 0.0);
    colorTransferFunction->AddRGBPoint(128, 1.0, 0.5, 0.3);
    colorTransferFunction->AddRGBPoint(255, 1.0, 1.0, 1.0);
    volumeProperty->SetColor(colorTransferFunction);

    vtkNew<vtkImageGaussianSmooth> filter;
    filter->SetInputData(imageData);
    filter->SetStandardDeviations(4.0, 4.0); // Adjust as needed
    filter->SetRadiusFactors(2.0, 2.0); // Adjust as needed
    filter->Update();

    volume = vtkSmartPointer<vtkVolume>::New();
    volume->SetMapper(volumeMapper);
    volume->SetProperty(volumeProperty);

    // Render
    renderer->RemoveAllViewProps();
    renderer->AddVolume(volume);
    renderer->ResetCamera();
    renderWindow->Render();
}


void NiiViewer::changeTransparency(double value)
{
    vtkSmartPointer<vtkPiecewiseFunction> opacityFunction = vtkSmartPointer<vtkPiecewiseFunction>::New();
    opacityFunction->AddPoint(0, 0.0);
    opacityFunction->AddPoint(500, value);
    volume->GetProperty()->SetGradientOpacity(opacityFunction);

    renderWindow->Render();
}

