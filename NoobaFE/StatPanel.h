#ifndef STATPANEL_H
#define STATPANEL_H

#include <QWidget>

namespace Ui {
class StatPanel;
}

class StatPanel : public QWidget
{
    Q_OBJECT

public:
    explicit StatPanel(QWidget *parent = 0);
    ~StatPanel();

    void setDeviceName(const QString& str);
    void setImageBufferLabel(const QString& str);
    void setImageBufferBarValue(int size);
    void setCaptureRateLabel(const QString& rate);
    void setnFramesCapturedLabel(const QString& nFrames);
    void setProcessingRate(const QString& str);
    void setnFramesProcessedLabel(const QString& str);
    void setImageBufferBarSize(int min, int max);

private:
    Ui::StatPanel *ui;
};

#endif // STATPANEL_H
