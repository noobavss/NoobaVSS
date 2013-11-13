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
    ui->canvas->setPixmap(QPixmap::fromImage(in));
    return true;
}
