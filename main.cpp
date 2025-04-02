/*
*  @file      main.cpp
*  @author    Kai Isozumi (kai.isozumi@kohyoung.com)
*  @date      February 25, 2025
*  @brief     This file is main file for VTKWidget GUI application.
*
*  @copyright Copyright (c) 2025 Koh Young Inc., All rights reserved.
*/

//#include "NiiViewer.h"
#include "MainWindow.h"
#include <vtkAutoInit.h>
VTK_MODULE_INIT(vtkRenderingOpenGL2);
VTK_MODULE_INIT(vtkInteractionStyle);
VTK_MODULE_INIT(vtkRenderingVolumeOpenGL2);

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    //NiiViewer w;
    w.adjustSize();
    w.show();
    //w.loadFile("C:/Users/kai/projects/VTKWidget/resources/new_CT.nii");
    return a.exec();
}
