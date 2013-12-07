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

#ifndef VIDOUTPUTSUBWIND_H
#define VIDOUTPUTSUBWIND_H
// Qt
#include <QWidget>
#include <QPixmap>
#include <QPropertyAnimation>

#include "FrameLabel.h"
#include "noobapluginapi.h"

// forward declaration VidOutputSubWind
namespace Ui { class FrameViewer; }

class QImage;
class QMdiSubWindow;
class QMenu;

class FrameViewer : public QWidget
{
    Q_OBJECT

    Q_PROPERTY(QColor bgColor READ canvasBGColor WRITE setCanvasBGColor)
public:

    explicit FrameViewer(const QString& title, QWidget *parent = 0);
    ~FrameViewer();
    void setMdiSubWindow(QMdiSubWindow* subWindow) { _mdiSubWindow = subWindow; }

    void setCanvasBGColor(const QColor& color);
    QColor canvasBGColor() const;

signals:

    void lineParamChanged(const QString& varName, const QString& frameViewerTitle, const QLine& line);

public slots:

    void createLineParam(const QString& varName, const QColor &color);
    void closeEvent(QCloseEvent *event);
    bool updateFrame(QImage in);
    void resizeEvent(QResizeEvent *event);
    void setVisibility(bool isVisible);
    void triggerAlert(nooba::AlertType alert);

private slots:

    void onToolMenuItemSelected();
    void onLineParamChanged(const QLine& line);
    void onShowHideButtonChecked(bool isChecked);
    void stopAlert();
    void mousePressEvent(QMouseEvent *event);
private:

    Ui::FrameViewer         *ui;
    QMdiSubWindow           *_mdiSubWindow;
    QMenu                   *_menu;
    const QString           _title;
    QString                 _activeVarName;
    QColor                  _color;
    QMap<QString, QColor>   _lineColorMap;
    QPropertyAnimation      _propertyAnimation;

};

#endif // VIDOUTPUTSUBWIND_H
