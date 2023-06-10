
#include "mainwindow.h"

#include <QApplication>
#include <QTranslator>

#include "ELCSettings.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QString appName = QCoreApplication::applicationName();
    QString appPath = a.applicationDirPath();
    QCoreApplication::addLibraryPath(QStringLiteral("%1/plugins").arg(appPath));

    QString iniFile = QStringLiteral("%1.ini").arg(appName);
    ELCSettings::instance().init(appPath, iniFile);

    QTranslator translator;
    if (translator.load(QLocale(), appName, QLatin1String("_"), QLatin1String(":/i18n"))) {
        QCoreApplication::installTranslator(&translator);
        qDebug() << "Translator files loaded";
    }

    MainWindow w;
    w.show();
    return a.exec();
}
