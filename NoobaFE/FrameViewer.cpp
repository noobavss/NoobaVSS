#include "FrameViewer.h"
#include "ui_FrameViewer.h"

#include <QCloseEvent>

FrameViewer::FrameViewer(const QString &title, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FrameViewer)
{
    ui->setupUi(this);
    ui->canvas->setProperty("canvas", true);    // for use with style sheet styling
    setWindowTitle(title);
    // show video in center
    ui->canvas->setAlignment(Qt::AlignCenter);
    ui->canvas->setAlignment(Qt::AlignCenter);
    ui->canvas->setMinimumSize(1,1);
}

FrameViewer::~FrameViewer()
{
    delete ui;
}

void FrameViewer::closeEvent(QCloseEvent * event)
{
    event->ignore();
}

bool FrameViewer::updateFrame(const QImage &in)
{    
    ui->canvas->setPixmap(QPixmap::fromImage(in).scaled(ui->canvas->size(), Qt::KeepAspectRatio));
    return true;
}

void FrameViewer::resizeEvent(QResizeEvent *event)
{
    if(!ui->canvas->pixmap())
        return;

    ui->canvas->setPixmap(ui->canvas->pixmap()->scaled(event->size(), Qt::KeepAspectRatio));
}


