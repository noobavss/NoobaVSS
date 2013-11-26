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
    onShowHideButtonChecked(ui->showHideButton->isChecked());

    // show video in center
    ui->drawLineToolButton->setDisabled(true);
    ui->canvas->setAlignment(Qt::AlignCenter);
    ui->canvas->setAlignment(Qt::AlignCenter);
    ui->canvas->setMinimumSize(1,1);
    ui->drawLineToolButton->setMenu(_menu);
    connect(ui->canvas, SIGNAL(lineUpdated(QLine)), this, SLOT(onLineParamChanged(QLine)));
    _propertyAnimation.setTargetObject(this);
    _propertyAnimation.setPropertyName("bgColor");
    _propertyAnimation.setDuration(1500);
    _propertyAnimation.setStartValue(QColor(91, 91, 91));
    _propertyAnimation.setEndValue(QColor(91,91,91));
//    _propertyAnimation.setEasingCurve(QEasingCurve::OutCubic);
}

FrameViewer::~FrameViewer()
{
    delete ui;
}

void FrameViewer::setCanvasBGColor(const QColor &color)
{
    QRgb rgb = color.rgb();
    QString str(QString("background-color: rgb(%1, %2, %3);").arg(qRed(rgb)).arg(qGreen(rgb)).arg(qBlue(rgb)));
    ui->canvasBorder->setStyleSheet(str);
}

QColor FrameViewer::canvasBGColor() const
{
    return ui->canvasBorder->palette().color(QWidget::backgroundRole());
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
    ui->canvas->setImage(in);
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

void FrameViewer::triggerAlert(nooba::AlertType alert)
{
    switch (alert) {
    case nooba::RedAlert:
        _propertyAnimation.setKeyValueAt(0.4, QColor(Qt::red));
        break;
    case nooba::AmberAlert:
        _propertyAnimation.setKeyValueAt(0.4, QColor(240,75, 10));
        break;
    default:
        break;
    }
    _propertyAnimation.setLoopCount(-1);
    _propertyAnimation.start();
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

void FrameViewer::onShowHideButtonChecked(bool isChecked)
{
    ui->canvas->setShowSketches(isChecked);
    if(ui->showHideButton->isChecked())
    {
        ui->showHideButton->setToolTip(tr("hide sketches"));
    }
    else
    {
        ui->showHideButton->setToolTip((tr("show sketches")));
    }
}

void FrameViewer::mousePressEvent(QMouseEvent *event)
{
//    _propertyAnimation.
    _propertyAnimation.stop();
    setCanvasBGColor(QColor(91,91,91));
}




