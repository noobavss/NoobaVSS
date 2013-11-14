#include "CameraView.h"
#include "ui_CameraView.h"

CameraView::CameraView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CameraView)
{
    ui->setupUi(this);
}

CameraView::~CameraView()
{
    delete ui;
}
