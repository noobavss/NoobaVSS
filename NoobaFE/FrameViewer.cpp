#include "FrameViewer.h"
#include "ui_FrameViewer.h"
// Qt
#include <QCloseEvent>
#include <QMdiSubWindow>
#include <QMenu>
#include <QAction>

FrameViewer::FrameViewer(const QString &title, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FrameViewer),
    _menu(new QMenu(parent)),
    _title(title)
{
    ui->setupUi(this);    
    ui->canvas->setProperty("canvas", true);    // for use with style sheet styling
    setWindowTitle(_title);
    // show video in center
    ui->drawLineToolButton->setDisabled(true);
    ui->canvas->setAlignment(Qt::AlignCenter);
    ui->canvas->setAlignment(Qt::AlignCenter);
    ui->canvas->setMinimumSize(1,1);
    ui->drawLineToolButton->setMenu(_menu);
    connect(ui->canvas, SIGNAL(lineUpdated(QLine)), this, SLOT(onLineParamChanged(QLine)));
}

FrameViewer::~FrameViewer()
{
    delete ui;
}

void FrameViewer::createLineParam(const QString &varName)
{
    ui->drawLineToolButton->setEnabled(true);
    QAction* act = _menu->addAction(varName,  this, SLOT(onToolMenuItemSelected()));
    QVariant v;
    v.setValue< nooba::DrawMode >(nooba::lineDraw);
    act->setData(v);
}

void FrameViewer::closeEvent(QCloseEvent * event)
{
    event->ignore();
}

bool FrameViewer::updateFrame(QImage in)
{
    if(in.isNull())
        return false;

    _pixmap = QPixmap::fromImage(in);
    ui->canvas->setPixmap(_pixmap.scaled(ui->canvasBorder->size(), Qt::KeepAspectRatio));
    return true;
}

void FrameViewer::resizeEvent(QResizeEvent *event)
{
    if(_pixmap.isNull())
        return;
    ui->canvas->setPixmap(_pixmap.scaled(event->size(), Qt::KeepAspectRatio));
}

void FrameViewer::setVisibility(bool isVisible)
{
    _mdiSubWindow->setVisible(isVisible);
    return;
}

void FrameViewer::onToolMenuItemSelected()
{
    QAction* act = qobject_cast<QAction* >(sender());
    ui->canvas->setDrawMode(act->text(), act->data().value<nooba::DrawMode>());
    _activeVarName = act->text();
}

void FrameViewer::onLineParamChanged(const QLine &line)
{
    emit lineParamChanged(_activeVarName, _title, line);
}




