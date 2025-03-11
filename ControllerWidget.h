#ifndef CONTROLLERWIDGET_H
#define CONTROLLERWIDGET_H

#include <QWidget>
#include <QSlider>

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

private slots:
    void changeTransparency(double value);

private:
    Ui::ControllerWidget *ui;
    QSlider* transparencySlider;
};

#endif // CONTROLLERWIDGET_H
