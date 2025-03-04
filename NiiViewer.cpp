#include "NiiViewer.h"
//#include <vtkImageSliceMapper.h>
#include <vtkImageData.h>
#include <QDebug>
#include <vtkSmartVolumeMapper.h>
#include <vtkColorTransferFunction.h>

NiiViewer::NiiViewer(QWidget *parent)
    : QMainWindow(parent)
    , vtkWidget(new QVTKOpenGLNativeWidget(this))
{
    renderWindow = vtkSmartPointer<vtkGenericOpenGLRenderWindow>::New();
    renderer = vtkSmartPointer<vtkRenderer>::New();
    renderWindow->AddRenderer(renderer);

    vtkWidget->setRenderWindow(renderWindow);
    renderer->SetBackground(0.1, 0.2, 0.3);

    transparencySlider = new QSlider(Qt::Horizontal, this);
    transparencySlider->setRange(0,100);
    transparencySlider->setValue(100);
    transparencySlider->setFixedHeight(30);

    connect(transparencySlider, &QSlider::valueChanged, this, &NiiViewer::changeTransparency);

    setCentralWidget(vtkWidget);
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

    volume = vtkSmartPointer<vtkVolume>::New();
    volume->SetMapper(volumeMapper);
    volume->SetProperty(volumeProperty);

    // Render
    renderer->RemoveAllViewProps();
    renderer->AddVolume(volume);
    renderer->ResetCamera();
    renderWindow->Render();
}

void NiiViewer::changeTransparency(int value)
{
    double opacityScale = value / 100.0;
    vtkSmartPointer<vtkPiecewiseFunction> opacityFunction = vtkSmartPointer<vtkPiecewiseFunction>::New();
    opacityFunction->AddPoint(0, 0.0);
    opacityFunction->AddPoint(255, opacityScale);
    volume->GetProperty()->SetScalarOpacity(opacityFunction);

    renderWindow->Render();
}
