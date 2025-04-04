/****************************************************************************
*
* The Event Log Conversion GUI Utility Main file.
* Copyright (C) 2023-2024  Oleksii Gaienko
* Contact: galexsoftware@gmail.com
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
****************************************************************************/

#include "mainwindow.h"
#include <QApplication>
#include <QTranslator>
#include <QMessageBox>
#include <QFileInfo>

#include "CSingleApplication.h"
#include "CElcGuiAppSettings.h"
#include "elcUtils.h"
#include "CParserManager.h"
#include "CSqliteDatabase.h"

void
initTranslation(QTranslator *translator, const QApplication *qa, const QStringList &list, const QString &fileName)
{
#ifdef Q_OS_LINUX
    QString locales = setlocale(LC_ALL, nullptr);
    if (locales.contains(QLatin1String("ru_RU"))) {
        QLocale::setDefault(QLocale::Russian);
    } else if (locales.contains(QLatin1String("uk_UA"))) {
        QLocale::setDefault(QLocale::Ukrainian);
    }
#endif

    if (list.contains(QLatin1String("--russian"), Qt::CaseSensitive)) {
        QLocale::setDefault(QLocale::Russian);
    } else if (list.contains(QLatin1String("--ukrainian"), Qt::CaseSensitive)) {
        QLocale::setDefault(QLocale::Ukrainian);
    }

#if QT_VERSION == QT_VERSION_CHECK(6, 3, 2)
    bool retVal = translator->load(QLocale(), fileName, QLatin1String("_"), QLatin1String(":/i18n"));
    if (retVal) {
        retVal = qa->installTranslator(translator);
    }
    if (!retVal) {
        QLocale loc;
        QStringList lang = loc.uiLanguages();
        if (lang.contains(QLatin1String("uk-UA"), Qt::CaseInsensitive) ||
            lang.contains(QLatin1String("ru-RU"), Qt::CaseInsensitive) ) {
            QMessageBox::warning(nullptr, "Warning", "Error loading localization resources.");
        }
    }
#else
    Q_ASSERT_X(false, Q_FUNC_INFO, "This code was tested and works correctly in Qt 6.3.2 only");
#endif
}

void
createConfigInfo(const QString &appName, const QStringList &list, QString &path, QString &fileName, bool &isRdsMode)
{
#ifdef Q_OS_WIN
    fileName = QStringLiteral("%1.ini").arg(appName);
    quint32 errorCode = 0;
    isRdsMode = elcUtils::isRdsMode(errorCode);
    if (errorCode != 0) {
        isRdsMode = false;
        QString msg = elcUtils::getWindowsApiErrorMessage(errorCode);
        QMessageBox::warning(nullptr, "Warning", QString("Error checking RDS mode: %1\nThe utility will be launched in the single user mode!").arg(msg));
    }
    if (isRdsMode || list.contains(QLatin1String("--enablerds"), Qt::CaseSensitive)) {
        path = QStringLiteral("%AppData%/%1").arg(appName); // %SystemDrive%:/Users/%UserName%/AppData/Roaming/%1
    }
#else
    fileName = QStringLiteral("%1.conf").arg(appName);
    isRdsMode = false;
#endif
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QStringList argsList;
    for (int i = 0; i < argc; ++i) {
        argsList.append(argv[i]);
    }

    QCoreApplication::setApplicationVersion(QStringLiteral("%1 tag %2 %3").arg(BUILD_VER, BUILD_GIT, elcUtils::getFormattedDateTime(BUILD_DATE)));
    QString appName = QCoreApplication::applicationName();
#ifdef Q_OS_WIN
    QString appPath = QApplication::applicationDirPath();
#else
    QString appPath = QStringLiteral("$HOME/.config/%1").arg(appName);
#endif

    QTranslator translator;
    initTranslation(&translator, &a, argsList, appName);

    CSingleApplication sa(QLatin1String("elcw_instance"));
    if (sa.isRunning()) {
        QMessageBox::information(nullptr, QObject::tr("Information"),
                                 QObject::tr("Another copy of the Utility is still running. Please close it."),
                                 QMessageBox::Ok);
        return 1;
    }

    QString iniFile;
    QString errorString;
    bool isRdsMode;
    createConfigInfo(appName, argsList, appPath, iniFile, isRdsMode);
    elcUtils::expandEnvironmentStrings(appPath);
    if (!elcUtils::mkPath(appPath, errorString)) {
        QMessageBox::critical(nullptr, QObject::tr("Error"), QObject::tr("Cannot create folder: %1\nDetails: %2").arg(appPath, errorString), QMessageBox::Ok);
        return 1;
    }
    if (!CElcGuiAppSettings::instance().init(appPath, iniFile, isRdsMode)) {
        QMessageBox::critical(nullptr, QObject::tr("Error"), QObject::tr("The settings Class cannot be initialized."), QMessageBox::Ok);
        return 1;
    }
    CParserManager::instance().init();

    // Get path to the DB
    CElcGuiAppSettings &settings = CElcGuiAppSettings::instance();
    QString dbName = settings.getDbFileName();
    if (dbName.isEmpty()) {
        dbName = QStringLiteral("%1/%2.db").arg(appPath, appName);
        settings.setMain(QLatin1String("SETTINGS"), QLatin1String("db_file_name"), dbName);
        QMessageBox::warning(nullptr, QObject::tr("Warning"), QObject::tr("Unable to get database file name.\nThe database file will be created on the default path."), QMessageBox::Ok);
    }
    elcUtils::expandEnvironmentStrings(dbName);
    QString dbPath = QFileInfo(dbName).absolutePath();
    if (!elcUtils::mkPath(dbPath, errorString)) {
        QMessageBox::critical(nullptr, QObject::tr("Error"), QObject::tr("Cannot create folder: %1\nDetails: %2").arg(dbPath, errorString), QMessageBox::Ok);
        return 1;
    }
    if (settings.isClearDbOnStartup()) {
        elcUtils::expandEnvironmentStrings(dbName);
        const CParserManager &parserManager = CParserManager::instance();
        QStringList tables = parserManager.getTablesList();
        if (!CSqliteDatabase::truncateDB(dbName, errorString, tables)) {
            QMessageBox::critical(nullptr, QObject::tr("Error"), QObject::tr("Cannot open database: %1\nDetails: %2").arg(dbName, errorString), QMessageBox::Ok);
            return 1;
        }
    }

    MainWindow w;
    w.show();
    return a.exec();
}
