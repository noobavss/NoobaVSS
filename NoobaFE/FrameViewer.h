#ifndef VIDOUTPUTSUBWIND_H
#define VIDOUTPUTSUBWIND_H

#include <QWidget>
#include <QPixmap>

// forward declaration VidOutputSubWind
namespace Ui { class FrameViewer; }

class QImage;

class FrameViewer : public QWidget
{
    Q_OBJECT
    
public:

    explicit FrameViewer(const QString& title, QWidget *parent = 0);
    ~FrameViewer();

public slots:

    void closeEvent(QCloseEvent *event);
    bool updateFrame(QImage in);
    void resizeEvent(QResizeEvent *event);


private:

    QPixmap         _pixmap;
    Ui::FrameViewer *ui;
};

#endif // VIDOUTPUTSUBWIND_H
