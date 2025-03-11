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

private:
    Ui::StatusViewer *ui;
};

#endif // STATUSVIEWER_H
