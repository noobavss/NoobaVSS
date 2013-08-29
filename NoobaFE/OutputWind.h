#ifndef OUTPUTWIND_H
#define OUTPUTWIND_H

#include <QWidget>

namespace Ui {
class OutputWind;
}

class OutputWind : public QWidget
{
    Q_OBJECT
    
public:
    explicit OutputWind(QWidget *parent = 0);
    ~OutputWind();

public slots:

    void onDebugMsg(const QString& msg);
    
private:
    Ui::OutputWind *ui;
};

#endif // OUTPUTWIND_H
