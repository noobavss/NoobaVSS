#include "StatPanel.h"
#include "ui_StatPanel.h"

//Qt
#include <QPalette>

StatPanel::StatPanel(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::StatPanel)
{
    ui->setupUi(this);
    ui->imageBufferBar->setValue(0);
    ui->imageBufferLabel->setText("[00/00]");
    ui->captureRateLabel->setText("");
    ui->processingRateLabel->setText("");
    ui->deviceNumberLabel->setText("");
    ui->cameraResolutionLabel->setText("");
    ui->mouseCursorPosLabel->setText("");
}

StatPanel::~StatPanel()
{
    delete ui;
}

void StatPanel::setDeviceName(const QString &str)
{
    ui->deviceNumberLabel->setText(QString(" ").append(str));
}

void StatPanel::setImageBufferLabel(const QString &str)
{
    ui->imageBufferLabel->setText(str);
}

void StatPanel::setImageBufferBarValue(int size)
{
    QString style("QProgressBar {"
                  "text-align: center;"
                  "}""QProgressBar::chunk {"
                  "background: %1;"
                  "text-align: center;"
                  "margin: 2px 1px 1p 2px;"
                  "}");
    QString c;
    if(size == ui->imageBufferBar->maximum())
        c = "#ff4000";
    else
        c = "#00ff00";

    ui->imageBufferBar->setStyleSheet(style.arg(c));
    ui->imageBufferBar->setValue(size);
}

void StatPanel::setCaptureRateLabel(const QString &rate)
{
    ui->captureRateLabel->setText(rate);
}

void StatPanel::setnFramesCapturedLabel(const QString &nFrames)
{
    ui->nFramesCapturedLabel->setText(nFrames);
}

void StatPanel::setProcessingRate(const QString &str)
{
    ui->processingRateLabel->setText(str);
}

void StatPanel::setnFramesProcessedLabel(const QString &str)
{
    ui->nFramesProcessedLabel->setText(str);
}

void StatPanel::setImageBufferBarSize(int min, int max)
{
    ui->imageBufferBar->setMaximum(max);
    ui->imageBufferBar->setMinimum(min);
}
