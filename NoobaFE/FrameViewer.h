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
    QPixmap                 _pixmap;
    QColor                  _color;
    QMap<QString, QColor>   _lineColorMap;
    QPropertyAnimation      _propertyAnimation;

};

#endif // VIDOUTPUTSUBWIND_H
