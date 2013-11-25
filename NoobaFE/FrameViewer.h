#ifndef VIDOUTPUTSUBWIND_H
#define VIDOUTPUTSUBWIND_H
// Qt
#include <QWidget>
#include <QPixmap>

#include "FrameLabel.h"

// forward declaration VidOutputSubWind
namespace Ui { class FrameViewer; }

class QImage;
class QMdiSubWindow;
class QMenu;

class FrameViewer : public QWidget
{
    Q_OBJECT
    
public:

    explicit FrameViewer(const QString& title, QWidget *parent = 0);
    ~FrameViewer();
    void setMdiSubWindow(QMdiSubWindow* subWindow) { _mdiSubWindow = subWindow; }

signals:

    void lineParamChanged(const QString& varName, const QString& frameViewerTitle, const QLine& line);

public slots:

    void createLineParam(const QString& varName);
    void closeEvent(QCloseEvent *event);
    bool updateFrame(QImage in);
    void resizeEvent(QResizeEvent *event);
    void setVisibility(bool isVisible);

private slots:

    void onToolMenuItemSelected();
    void onLineParamChanged(const QLine& line);

private:

    Ui::FrameViewer     *ui;
    QMdiSubWindow       *_mdiSubWindow;
    QMenu               *_menu;
    const QString       _title;
    QString             _activeVarName;
    QPixmap             _pixmap;
};

#endif // VIDOUTPUTSUBWIND_H
