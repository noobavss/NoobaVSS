/************************************************************************/
/* qt-opencv-multithreaded:                                             */
/* A multithreaded OpenCV application using the Qt framework.           */
/*                                                                      */
/* FrameLabel.h                                                         */
/*                                                                      */
/* Nick D'Ademo <nickdademo@gmail.com>                                  */
/* Modified to fit NoobaVSS by                                          */
/*      D.A.U.Nanayakkara <daun07@gmail.com>                            */
/*                                                                      */
/* Copyright (c) 2012-2013 Nick D'Ademo                                 */
/*                                                                      */
/* Permission is hereby granted, free of charge, to any person          */
/* obtaining a copy of this software and associated documentation       */
/* files (the "Software"), to deal in the Software without restriction, */
/* including without limitation the rights to use, copy, modify, merge, */
/* publish, distribute, sublicense, and/or sell copies of the Software, */
/* and to permit persons to whom the Software is furnished to do so,    */
/* subject to the following conditions:                                 */
/*                                                                      */
/* The above copyright notice and this permission notice shall be       */
/* included in all copies or substantial portions of the Software.      */
/*                                                                      */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,      */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF   */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND                */
/* NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS  */
/* BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN   */
/* ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN    */
/* CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE     */
/* SOFTWARE.                                                            */
/*                                                                      */
/************************************************************************/

#ifndef FRAMELABEL_H
#define FRAMELABEL_H

// Qt
#include <QtCore/QObject>
#include <QtCore/QPoint>
#include <QtCore/QRect>
#include <QLine>
#include <QLabel>
#include <QMenu>
// Local
#include "Structures.h"

namespace nooba
{
    enum DrawMode {
        noDraw      = 0,
        lineDraw    = 1,
        boxDraw     = 2
    };
}

Q_DECLARE_METATYPE(nooba::DrawMode)

class FrameLabel : public QLabel
{
    Q_OBJECT

    public:
        FrameLabel(QWidget *parent = 0);
        void setMouseCursorPos(QPointF);
        QPointF getMouseCursorPos();
        void setDrawMode(const QString& varName, const QColor& color, nooba::DrawMode drawMode);
        nooba::DrawMode getDrawMode() const { return _drawMode; }
        void setImage(const QImage& image);
        QMenu *menu;
        void setShowSketches(bool showSketches) { _showSketches = showSketches; }
        QSize sizeHint();

    private:
        struct _S_Line
        {
            _S_Line() {}

            _S_Line(const QLineF& line, const QColor& color):
                _line(line), _color(color) {}

            QLineF   _line;
            QColor  _color;
        };

        void createContextMenu();
        inline QPointF toOriginalImage(const QPointF& c_p);
        inline QPointF toCurrentImage(const QPointF& o_p);
        inline QLineF toCurrentImage(const QLineF& o_l);

        MouseData               mouseData;
        QPointF                 startPointOrg;
        QPointF                 mouseCursorPos;
        bool                    _draw;
        bool                    _showSketches;
        QRect                   *box;
        QLineF                  drawingLine;
        nooba::DrawMode         _drawMode;
        QString                 _varName;
        QColor                  _color;
        QMap<QString, _S_Line>  _lineMap;
        QImage                  _image;

    protected:
        void mouseMoveEvent(QMouseEvent *ev);
        void mousePressEvent(QMouseEvent *ev);
        void mouseReleaseEvent(QMouseEvent *ev);
        void paintEvent(QPaintEvent *ev);
        void keyReleaseEvent(QKeyEvent *event);
        void resizeEvent(QResizeEvent *event);

    signals:
        void newMouseData(struct MouseData mouseData);
        void onMouseMoveEvent();
        void lineUpdated(const QLine& line);
};

#endif // FRAMELABEL_H
