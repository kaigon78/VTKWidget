#include "ControllerWidget.h"
#include "ui_ControllerWidget.h"
#include <vtkPiecewiseFunction.h>
#include <vtkSmartPointer.h>
#include <vtkVolume.h>

ControllerWidget::ControllerWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ControllerWidget)
{
    ui->setupUi(this);
    transparencySlider = new QSlider(Qt::Horizontal, this);
    transparencySlider->setRange(0,100);
    transparencySlider->setTickInterval(1);
    transparencySlider->setValue(100);
    transparencySlider->setFixedWidth(500);

    connect(transparencySlider, &QSlider::valueChanged, this, &ControllerWidget::changeTransparency);
}

ControllerWidget::~ControllerWidget()
{
    delete ui;
}

void ControllerWidget::changeTransparency(double value)
{
    double opacityScale = value / 1000.0;
    emit transparencyChanged(opacityScale);
}
