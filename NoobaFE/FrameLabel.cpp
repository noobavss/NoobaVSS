/************************************************************************/
/* qt-opencv-multithreaded:                                             */
/* A multithreaded OpenCV application using the Qt framework.           */
/*                                                                      */
/* FrameLabel.cpp                                                       */
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

#include "FrameLabel.h"
// Qt
#include <QtGui/QPainter>
#include <QtGui/QMouseEvent>

FrameLabel::FrameLabel(QWidget *parent) :
    QLabel(parent),
    _color(Qt::transparent)
{
    setMouseTracking(true);
    startPoint.setX(0);
    startPoint.setY(0);
    mouseCursorPos.setX(0);
    mouseCursorPos.setY(0);
    _draw=false;
    mouseData.leftButtonRelease=false;
    mouseData.rightButtonRelease=false;
    createContextMenu();
    setFocusPolicy(Qt::StrongFocus);
}

void FrameLabel::mouseMoveEvent(QMouseEvent *ev)
{
    // Save mouse cursor position
    setMouseCursorPos(ev->pos());
    // Update box width and height if box drawing is in progress
    if(_draw)
    {
        switch(_drawMode)
        {
        case nooba::lineDraw:
        {
            drawingLine.setP2(getMouseCursorPos());
            break;
        }
        default:
            break;
        }
    }
    // Inform main window of mouse move event
    emit onMouseMoveEvent();
}

void FrameLabel::setMouseCursorPos(QPoint input)
{
    mouseCursorPos=input;
}

QPoint FrameLabel::getMouseCursorPos()
{
    return mouseCursorPos;
}

void FrameLabel::setDrawMode(const QString &varName, const QColor &color, nooba::DrawMode drawMode)
{
    _varName = varName;
    _drawMode = drawMode;
    _color  = color;
}

void FrameLabel::mouseReleaseEvent(QMouseEvent *ev)
{
    // Update cursor position
    setMouseCursorPos(ev->pos());
    // On left mouse button release
    if(ev->button()==Qt::LeftButton)
    {
        // Set leftButtonRelease flag to TRUE
        mouseData.leftButtonRelease=true;
        if(_draw)
        {
            // Stop drawing box
            _draw=false;
        }
    }
    // On right mouse button release
    else if(ev->button()==Qt::RightButton)
    {
        // If user presses (and then releases) the right mouse button while drawing box, stop drawing box

        _draw=false;
        _varName.clear();
        _drawMode = nooba::noDraw;
        drawingLine = QLine();
        _color = QColor(Qt::transparent);
        //        else
        //        {
//            // Show context menu
//            menu->exec(ev->globalPos());
//        }
    }
}

void FrameLabel::mousePressEvent(QMouseEvent *ev)
{
    // Update cursor position
    setMouseCursorPos(ev->pos());;
    if(ev->button()==Qt::LeftButton)
    {
        // Start drawing box
        if(_varName.isEmpty())
            return;
        startPoint=ev->pos();
        switch(_drawMode)
        {
        case nooba::lineDraw:
        {
            _S_Line s_l = _lineMap.take(_varName);
            if(s_l._line.isNull())
            {
                if(drawingLine.isNull())
                {
                    drawingLine = QLine(startPoint, startPoint);
                }
              }
            else
            {
                drawingLine =s_l._line;
            }
            drawingLine.setPoints(startPoint, startPoint);
            _draw=true;
            break;
        }
        case nooba::boxDraw:
        {
            box = new QRect(startPoint.x(),startPoint.y(),0,0);
            _draw = true;
            break;
        }
        default:
            break;
        }
    }
}

void FrameLabel::paintEvent(QPaintEvent *ev)
{
    QLabel::paintEvent(ev);
    QPainter painter(this);

    foreach(_S_Line s_l, _lineMap)
    {
        painter.setPen(s_l._color);
        painter.drawLine(s_l._line);
    }
    painter.setPen(_color);
    painter.drawLine(drawingLine);
}

void FrameLabel::keyReleaseEvent(QKeyEvent *event)
{
    qDebug("key release event");
    if(event->key() == Qt::Key_Return)
    {
        qDebug("enter event");
        switch(_drawMode)
        {
        case nooba::lineDraw:
        {
            _lineMap.insert(_varName, _S_Line(drawingLine, _color));
            _varName.clear();
            emit lineUpdated(drawingLine);
            drawingLine = QLine();
        }
        default:
            break;
        }
        _drawMode = nooba::noDraw;
        _draw = false;
    }
    else
    {
        QLabel::keyReleaseEvent(event);
    }
}

void FrameLabel::createContextMenu()
{

}
