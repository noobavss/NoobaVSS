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
    _path = path;
	ui->lineEdit->setText(path);
	ui->lineEdit->selectAll();	
	ui->lineEdit->setFocus();
}

QString PathLineEdit::getText() const
{
    return ui->lineEdit->text();
}

void PathLineEdit::setFileMode(nooba::PathType pathType)
{
    _pathType = pathType;
}

void PathLineEdit::setFilter(const QString &filter)
{
    _filter = filter;
}

void PathLineEdit::on_browseButtonClicked()
{
    QString startDir;
#if QT_VERSION >= 0x050000
    startDir = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
#else
    startDir = QDesktopServices::storageLocation(QDesktopServices::DesktopLocation);
#endif

    QString path;
    switch (_pathType)
    {
    case nooba::DirPath:
        path = QFileDialog::getExistingDirectory(this, tr("Select a Directory"), startDir);
        break;
    case nooba::FilePath:
    default:
        path = QFileDialog::getOpenFileName(this, tr("Select a File"), startDir, _filter);
        break;
    }
    if(path.isEmpty())
        return;

    setText(path);
    return;
}

 void PathLineEdit::focusInEvent ( QFocusEvent * event )
 {
	 ui->lineEdit->setFocus();
 }

 void PathLineEdit::focusOutEvent( QFocusEvent * event)
 {
	 QWidget::focusOutEvent(event);	
 }
