#include "FrameViewer.h"
#include "ui_FrameViewer.h"

#include <QMutexLocker>
#include <QCloseEvent>

FrameViewer::FrameViewer(const QString &title, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FrameViewer)
{
    ui->setupUi(this);
    ui->canvas->setProperty("canvas", true);    // for use with style sheet styling
    setWindowTitle(title);
    _title = title;
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

bool FrameViewer::updateFrame(QImage in)
{
    if(in.isNull())
        return false;
    QString t = _title;
    _pixmap = QPixmap::fromImage(in);
    ui->canvas->setPixmap(_pixmap.scaled(ui->canvas->size() - QSize(5,5), Qt::KeepAspectRatio));
    return true;
}

void FrameViewer::resizeEvent(QResizeEvent *event)
{
    if(_pixmap.isNull())
        return;

    ui->canvas->setPixmap(_pixmap.scaled(event->size() - QSize(5,5), Qt::KeepAspectRatio));
}




