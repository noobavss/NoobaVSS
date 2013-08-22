#include <QApplication>
#include "MainWindow.h"
#include "NoobaEye.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QFile f(":/Resources/stylesheet.css");
    if(f.open(QFile::ReadOnly))
    {
        QString stylesheet = QLatin1String(f.readAll());
        a.setStyleSheet(stylesheet);
    }

    a.setApplicationName(nooba::ProgramName);
    a.setOrganizationName(nooba::Organisation);
    a.setApplicationVersion(QString("%1.%2").arg(nooba::MajorVersion).arg(nooba::MinorVersion));
    a.setWindowIcon(QIcon(":/Resources/logo.png"));

    MainWindow w;
    w.show();
    return a.exec();
}
