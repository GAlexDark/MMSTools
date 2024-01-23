
#include "mainwindow.h"

#include <QApplication>
#include <QTranslator>
#include <QMessageBox>
#include <QDir>

//#include "Debug.h"
#include "CSingleApplication.h"
#include "CELCWSettings.h"
#include "elcUtils.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QCoreApplication::setApplicationVersion(QStringLiteral("%1 tag %2 %3").arg(BUILD_VER, BUILD_GIT, elcUtils::getFormattedDateTime( BUILD_DATE )));
    QString appName = QCoreApplication::applicationName();
#ifdef Q_OS_WIN
    QString appPath = a.applicationDirPath();
#else
    appPath = QStringLiteral("$XDG_DATA_HOME/%1").arg(appName);
#endif
    //QCoreApplication::addLibraryPath(QStringLiteral("%1/plugins").arg(appPath));

    QTranslator translator;
    bool retVal = translator.load(QLocale(), appName, QLatin1String("_"), QLatin1String(":/i18n"));
    if (retVal) {
        retVal = a.installTranslator(&translator);
    }
    if (!retVal) {
            QMessageBox::warning(nullptr, "Warning", "Error loading localization resources.");
    }

    CSingleApplication sa("elcw_instance");
    if (sa.isRunning()) {
        QMessageBox::information(nullptr, QObject::tr("Information"),
                                 QObject::tr("The another copy of the Utility is still running. Please close it."),
                                 QMessageBox::Ok);
        return 1;
    }


#ifdef Q_OS_WIN
    QString iniFile = QStringLiteral("%1.ini").arg(appName);
    quint32 errorCode = 0;
    retVal = elcUtils::isTerminalServerMode(errorCode);
    if (retVal && (errorCode == 0)) {
        appPath = QStringLiteral("%AppData%/Local/%1").arg(appName);
        elcUtils::expandEnvironmentStrings(appPath);
    }
#else
    QString iniFile = QStringLiteral("%1.conf").arg(appName);
    elcUtils::expandEnvironmentStrings(appPath);
    if (appPath.indexOf('$') != -1) { // the $XDG_DATA_HOME is not defined
        appPath = QStringLiteral("$HOME/.local/share/%1").arg(appName);
        elcUtils::expandEnvironmentStrings(appPath);
    }
    retval = false;
#endif
    if (!ELCWSettings::instance().init(appPath, iniFile, retVal)) {
        QMessageBox::critical(nullptr, QObject::tr("Error"), QObject::tr("The settings Class cannot be initialized."), QMessageBox::Ok);
        return 1;
    }

    //get path to the DB
    const ELCWSettings &settings = ELCWSettings::instance();
    QString dbName =  QDir::fromNativeSeparators(settings.getMain("SETTINGS/db_file_name").toString().trimmed());
    if (dbName.isEmpty()) {
        dbName = QStringLiteral("%1.db").arg(appName);
        QMessageBox::warning(nullptr, QObject::tr("Warning") ,QObject::tr("Unable to get database file name."), QMessageBox::Ok);
    }
    QString cleardb = settings.getMain("SETTINGS/clear_on_startup").toString().trimmed();
    if (cleardb.isEmpty() || (QString::compare(cleardb, "yes", Qt::CaseInsensitive) == 0)) {
        QString errorString;
        if (!elcUtils::trunvateDB(dbName, errorString)) {
            QMessageBox::critical(nullptr, QObject::tr("Error"), QObject::tr("Cannot open database: %1").arg(errorString), QMessageBox::Ok);
            return 1;
        }
    }


    MainWindow w;
    w.show();
    return a.exec();
}
