/*
*  @file      NiiViewer.h
*  @author    Kai Isozumi (kai.isozumi@kohyoung.com)
*  @date      February 25, 2025
*  @brief     This file is Nii file viewer header file for VTKWidget GUI application.
*
*  @copyright Copyright (c) 2025 Koh Young Inc., All rights reserved.
*/

#ifndef NIIVIEWER_H
#define NIIVIEWER_H

#include <QMainWindow>
#include <QVTKOpenGLNativeWidget.h>
#include <vtkSmartPointer.h>
#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkVolume.h>
#include <QSlider>

class NiiViewer : public QMainWindow
{
    Q_OBJECT

public:
    explicit NiiViewer(QWidget *parent = nullptr);
    ~NiiViewer();
    void loadFile(const std::string &filename);
    void addLine();

public slots:
    void changeTransparency(double value);
    void changeColor(const QColor &color);


private:
    QVTKOpenGLNativeWidget *vtkWidget;
    vtkSmartPointer<vtkGenericOpenGLRenderWindow> renderWindow;
    vtkSmartPointer<vtkRenderer> renderer;
    vtkSmartPointer<vtkVolume> volume;
    vtkSmartPointer<vtkActor> surfaceActor;
};
#endif // NIIVIEWER_H

