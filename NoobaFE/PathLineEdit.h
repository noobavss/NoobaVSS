#ifndef PATHCHOOSER_H
#define PATHCHOOSER_H

#include <QWidget>

namespace Ui {class PathLineEdit;}

class PathLineEdit : public QWidget
{
	
	Q_OBJECT

public:

    PathLineEdit(QWidget *parent = 0);
    ~PathLineEdit();
    void setText(const QString& path);
    QString getText() const;

private slots:

	void on_browseButtonClicked();

protected:

	 void focusInEvent ( QFocusEvent * event );
	 void focusOutEvent( QFocusEvent * event);

private:

    Ui::PathLineEdit *ui;
	QString			path;
};

#endif // PATHCHOOSER_H
