/*
*  @file      ControllerWidget.h
*  @author    Kai Isozumi (kai.isozumi@kohyoung.com)
*  @date      March 13, 2025
*  @brief     This file is controller header file for VTKWidget GUI application.
*
*  @copyright Copyright (c) 2025 Koh Young Inc., All rights reserved.
*/

#ifndef CONTROLLERWIDGET_H
#define CONTROLLERWIDGET_H

#include <QWidget>
#include <QSlider>
#include <QColorDialog>

namespace Ui {
class ControllerWidget;
}

class ControllerWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ControllerWidget(QWidget *parent = nullptr);
    ~ControllerWidget();

signals:
    void transparencyChanged(double value);
    void colorChanged(const QColor &color);

private slots:
    void changeTransparency(double value);
    void openColorDialog();

private:
    Ui::ControllerWidget *ui;
    QPushButton *colorButton;
};

#endif // CONTROLLERWIDGET_H
