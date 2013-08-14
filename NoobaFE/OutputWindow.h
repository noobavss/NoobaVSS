#ifndef OUTPUTWINDOW_H
#define OUTPUTWINDOW_H

#include <QWidget>
#include "ui_OutputWindow.h"

class OutputWindow : public QWidget
{
	Q_OBJECT

public:
	OutputWindow(QWidget *parent = 0);
	~OutputWindow();

private:
	Ui::OutputWindow ui;
};

#endif // OUTPUTWINDOW_H
