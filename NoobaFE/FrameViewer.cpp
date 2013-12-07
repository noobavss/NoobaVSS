/**
    @author  Asitha Nanayakkara <daun07@gmail.com>

    @section LICENSE
    Nooba Plugin API source file
    Copyright (C) 2013 Developed by Team Nooba

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

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
    _propertyAnimation.setStartValue(QColor(0,0,0));
    _propertyAnimation.setEndValue(QColor(0,0,0));
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
    ui->canvas->setImage(in);
    return true;
}

void FrameViewer::resizeEvent(QResizeEvent *event)
{
    ui->canvas->resize(event->size() - QSize(9,9));
}

void FrameViewer::setVisibility(bool isVisible)
{
    _mdiSubWindow->setVisible(isVisible);
    return;
}

void FrameViewer::triggerAlert(nooba::AlertType alert)
{
    stopAlert();    // stop any previous alert
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

void FrameViewer::stopAlert()
{
    setCanvasBGColor(QColor(91,91,91));
    if(_propertyAnimation.state() == QAbstractAnimation::Stopped)
        return;

    _propertyAnimation.stop();
    return;
}

void FrameViewer::mousePressEvent(QMouseEvent *event)
{
    Q_UNUSED(event)
    stopAlert();
    return;
}
