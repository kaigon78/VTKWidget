#include "StatusViewer.h"
#include "ui_StatusViewer.h"

StatusViewer::StatusViewer(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::StatusViewer)
{
    ui->setupUi(this);
    ui->m_transparencyLabel->setText("Transparency: 0");
}

void StatusViewer::updateTransparencyValue(double value) {
    //ui->m_transparencyLabel->setText(QString("Transparency: ").append(QString::number(value)));
    QString transparencyText = "Transparency: ";
    transparencyText.append(QString::number(value, 'f', 3));
    ui->m_transparencyLabel->setText(transparencyText);
}

StatusViewer::~StatusViewer()
{
    delete ui;
}

