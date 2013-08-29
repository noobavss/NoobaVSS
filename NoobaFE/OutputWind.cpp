#include "OutputWind.h"
#include "ui_OutputWind.h"

OutputWind::OutputWind(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::OutputWind)
{
    ui->setupUi(this);
}

OutputWind::~OutputWind()
{
    delete ui;
}

void OutputWind::onDebugMsg(const QString &msg)
{
    ui->textEdit->append(msg);
}
