#ifndef VIDOUTPUTSUBWIND_H
#define VIDOUTPUTSUBWIND_H

#include <QWidget>
#include <QPixmap>

// forward declaration VidOutputSubWind
namespace Ui { class FrameViewer; }

class QImage;
class QMdiSubWindow;

class FrameViewer : public QWidget
{
    Q_OBJECT
    
public:

    explicit FrameViewer(const QString& title, QWidget *parent = 0);
    ~FrameViewer();
    void setMdiSubWindow(QMdiSubWindow* subWindow) { _mdiSubWindow = subWindow; }

public slots:

    void closeEvent(QCloseEvent *event);
    bool updateFrame(QImage in);
    void resizeEvent(QResizeEvent *event);
    void setVisibility(bool isVisible);

private:

    QPixmap         _pixmap;
    QMdiSubWindow   *_mdiSubWindow;
    Ui::FrameViewer *ui;
};

#endif // VIDOUTPUTSUBWIND_H
