#ifndef PATHCHOOSER_H
#define PATHCHOOSER_H

#include "noobapluginapi.h"

#include <QFileDialog>
#include <QWidget>

namespace Ui {class PathLineEdit;}

class PathLineEdit : public QWidget
{
	
	Q_OBJECT

public:

    PathLineEdit(QWidget *parent = 0);
    ~PathLineEdit();
    void setText(const QString& _path);
    QString getText() const;
    void setFileMode(nooba::PathType pathType);
    void setFilter(const QString& filter);

private slots:

	void on_browseButtonClicked();

protected:

	 void focusInEvent ( QFocusEvent * event );
	 void focusOutEvent( QFocusEvent * event);

private:

    Ui::PathLineEdit        *ui;
    nooba::PathType         _pathType;
    QString                 _path;
    QString                 _filter;

};

#endif // PATHCHOOSER_H
