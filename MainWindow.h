/*
*  @file      MainWindow.h
*  @author    Kai Isozumi (kai.isozumi@kohyoung.com)
*  @date      February 25, 2025
*  @brief     This file is a main widget with three child widget header file for VTKWidget GUI application.
*
*  @copyright Copyright (c) 2025 Koh Young Inc., All rights reserved.
*/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
    class MainWindow;
}

class NiiViewer;
class ControllerWidget;
class StatusViewer;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();


private:
    Ui::MainWindow* ui;
    NiiViewer* niiViewer;
    ControllerWidget* controllerWidget;
    StatusViewer* statusViewer;

};
#endif // MAINWINDOW_H
