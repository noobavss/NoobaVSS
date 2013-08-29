#ifndef VIDOUTPUTSUBWIND_H
#define VIDOUTPUTSUBWIND_H

#include <QWidget>

// forward declaration
namespace Ui { class VidOutputSubWind; }
class QImage;

class VidOutputSubWind : public QWidget
{
    Q_OBJECT
    
public:

    explicit VidOutputSubWind(const QString& title, QWidget *parent = 0);
    ~VidOutputSubWind();

public slots:

    void closeEvent(QCloseEvent *event);
    bool updateFrame(const QImage& in);

private:

    Ui::VidOutputSubWind *ui;
};

#endif // VIDOUTPUTSUBWIND_H
