/*
*  @file      StatusViewer.h
*  @author    Kai Isozumi (kai.isozumi@kohyoung.com)
*  @date      March 13, 2025
*  @brief     This file is status viewing header file for VTKWidget GUI application.
*
*  @copyright Copyright (c) 2025 Koh Young Inc., All rights reserved.
*/

#ifndef STATUSVIEWER_H
#define STATUSVIEWER_H

#include <QWidget>

namespace Ui {
class StatusViewer;
}

class StatusViewer : public QWidget
{
    Q_OBJECT

public:
    explicit StatusViewer(QWidget *parent = nullptr);
    ~StatusViewer();
    void updateTransparencyValue(double value);
    void updateColorValue(const QColor &color);

private:
    Ui::StatusViewer *ui;
};

#endif // STATUSVIEWER_H
