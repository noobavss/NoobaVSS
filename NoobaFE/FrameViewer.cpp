#include "FrameViewer.h"
#include "ui_FrameViewer.h"
// Qt
#include <QCloseEvent>
#include <QMdiSubWindow>
#include <QMenu>
#include <QAction>
#include <QDebug>

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

void FrameViewer::createLineParam(const QString &varName, const QColor& color)
{
    ui->drawLineToolButton->setEnabled(true);
    QPixmap p(15,15);
    p.fill(color);
    QAction* act = _menu->addAction(varName,  this, SLOT(onToolMenuItemSelected()));
    act->setIcon(p);
    QVariant v;
    _lineColorMap.insert(varName, color);
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

    QColor c;
    nooba::DrawMode dm =  act->data().value<nooba::DrawMode>();
    switch(dm)
    {
        case nooba::lineDraw:
        {
            c = _lineColorMap.value(act->text());
            break;
        }
    default:
    {
        c = QColor();
        break;
    }
    }

    _activeVarName = act->text();
    ui->canvas->setDrawMode(_activeVarName, c, dm);
}

void FrameViewer::onLineParamChanged(const QLine &line)
{
    emit lineParamChanged(_activeVarName, _title, line);
}




