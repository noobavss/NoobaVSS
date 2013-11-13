#include "VidOutputSubWind.h"
#include "ui_VidOutputSubWind.h"

#include <QCloseEvent>

VidOutputSubWind::VidOutputSubWind(const QString &title, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::VidOutputSubWind)
{
    ui->setupUi(this);
    ui->canvas->setProperty("canvas", true);    // for use with style sheet styling
    setWindowTitle(title);
    // show video in center
    ui->canvas->setAlignment(Qt::AlignCenter);
    ui->canvas->setAlignment(Qt::AlignCenter);
}

VidOutputSubWind::~VidOutputSubWind()
{
    delete ui;
}

void VidOutputSubWind::closeEvent(QCloseEvent * event)
{
    event->ignore();
}

bool VidOutputSubWind::updateFrame(const QImage &in)
{
    ui->canvas->setPixmap(QPixmap::fromImage(in));
    return true;
}
