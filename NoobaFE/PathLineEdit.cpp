#if QT_VERSION >= 0x050000
#include <QStandardPaths>
#else
#include <QDesktopServices>
#endif
#include <QFileDialog>

#include "PathLineEdit.h"
#include "ui_PathLineEdit.h"

PathLineEdit::PathLineEdit(QWidget *parent)
    : QWidget(parent),
      ui(new Ui::PathLineEdit())
{
	ui->setupUi(this);
	connect(ui->browseButton, SIGNAL(clicked()), this, SLOT(on_browseButtonClicked()));
	ui->lineEdit->setFocus();
}

PathLineEdit::~PathLineEdit()
{
	delete ui;
}

void PathLineEdit::setText(const QString &path)
{
	this->path = path;
	ui->lineEdit->setText(path);
	ui->lineEdit->selectAll();	
	ui->lineEdit->setFocus();
}

QString PathLineEdit::getText() const
{
    return ui->lineEdit->text();
}

void PathLineEdit::on_browseButtonClicked()
{
#if QT_VERSION >= 0x050000
    QString path = QFileDialog::getOpenFileName(this, tr("Open file"),
                                                QStandardPaths::writableLocation(QStandardPaths::DesktopLocation));
#else
    QString path = QFileDialog::getOpenFileName(this, tr("Open file"),
                                                QDesktopServices::storageLocation(QDesktopServices::DesktopLocation));
#endif
    if(path.isEmpty())
        return;

    setText(path);
}

 void PathLineEdit::focusInEvent ( QFocusEvent * event )
 {
	 ui->lineEdit->setFocus();
 }

 void PathLineEdit::focusOutEvent( QFocusEvent * event)
 {
	 QWidget::focusOutEvent(event);	
 }
